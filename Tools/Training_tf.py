import os
import errno
import numpy
import pandas as pd
import math
from MVAcommon_tf import *
import optparse
from math import sqrt
from time import sleep
from glob import glob

parser = optparse.OptionParser("usage: %prog [options]\n")

parser.add_option ('-k', "--sklearnrf",         dest='sklearnrf',         action='store_true',                                  help="Run using sklearn RF")
parser.add_option ('-x', "--xgboost",           dest='xgboost',           action='store_true',                                  help="Run using xgboost")
parser.add_option ('-d', "--directory",         dest='directory',         action='store',      default="",                      help="Directory to store outputs (default .)")
parser.add_option ('-v', "--variables",         dest='variables',         action='store',      default="TeamAlpha",             help="Input features to use (default TeamAlpha)")
parser.add_option ('-e', "--nepoch",            dest='nepoch',            action='store',      default=100,         type="int", help="Number of training epoch (default 100)")
parser.add_option ('-n', "--nReaders",          dest="nReaders",          action='store',      default=4,           type="int", help="Number of file readers to use (default 4)")
parser.add_option ('-q', "--nThreadperReader",  dest="nThreadperReader",  action='store',      default=1,           type="int", help="Number of threads for each flie reader (default 1)")
parser.add_option ('-p', "--ptReweight",        dest="ptReweight",        action='store_true',                                  help="Reweight pt spectrum of events durring training")

options, args = parser.parse_args()

outputDirectory = ""
if len(options.directory):
  outputDirectory = options.directory + "/"
  try:
      os.mkdir(outputDirectory)
  except OSError as exc:
      if exc.errno == errno.EEXIST and os.path.isdir(outputDirectory):
          pass
      else:
          raise

def mainSKL():

  from sklearn.ensemble import RandomForestClassifier
  import pickle

  print "PROCESSING TRAINING DATA"

  # Import data
  dg = DataGetter(options.variables)
  npyInputData, npyInputAnswer, npyInputWgts, npyInputSampleWgts = dg.importData(samplesToRun = ["trainingTuple_division_0_TTbarSingleLep_training_1M.h5"], prescale=True, ptReweight=True)

  # Create random forest
  clf = RandomForestClassifier(n_estimators=500, max_depth=10, n_jobs = 4, verbose = True)
  
  print "TRAINING RF"

  # Train random forest 
  clf = clf.fit(npyInputData, npyInputAnswer[:,0], sample_weight=npyInputWgts[:,0])
  
  #Dump output from training
  fileObject = open(outputDirectory + "TrainingOutput.pkl",'wb')
  out = pickle.dump(clf, fileObject)
  fileObject.close()
      
  output = clf.predict_proba(npyInputData)[:,1]

def mainXGB():

  import xgboost as xgb

  print "PROCESSING TRAINING DATA"

  # Import data
  dg = DataGetter(options.variables)
  npyInputData, npyInputAnswer, npyInputWgts, npyInputSampleWgts = dg.importData(prescale=False, ptReweight=True)

  print "TRAINING XGB"

  # Create xgboost classifier
  # Train random forest 
  xgData = xgb.DMatrix(npyInputData, label=npyInputAnswer[:,0], weight=npyInputWgts)
  param = {'max_depth':4 }
  gbm = xgb.train(param, xgData, num_boost_round=1000)
  
  #Dump output from training
  gbm.save_model(outputDirectory + 'TrainingModel.xgb')

  output = gbm.predict(xgData)


def mainTF(_):

  import tensorflow as tf

  print "PROCESSING TRAINING DATA"

  # Import data
  dg = DataGetter(options.variables)
  validData = dg.importData(samplesToRun = ["trainingTuple_division_1_TTbarSingleLep_validation_100k.h5"])

  #get input/output sizes
  nFeatures = validData["data"].shape[1]
  nLabels = validData["labels"].shape[1]
  nWeigts = validData["weights"].shape[1]

  #Training parameters
  l2Reg = 0.0001
  MiniBatchSize = 128
  NEpoch = options.nepoch
  ReportInterval = 1000
  validationCount = min(10000, validData["data"].shape[0])

  #scale data inputs to range 0-1
  mins = validData["data"].min(0)
  ptps = validData["data"].ptp(0)
  #npyInputData = (npyInputData - mins)/ptps

  #Create filename queue
  fnq = FileNameQueue(glob("trainingTuple_division_0_TTbarSingleLep_training_1M_*.h5"), NEpoch, nFeatures, nLabels, nWeigts, options.nReaders, MiniBatchSize)

  #Create CustomRunner object to manage data loading 
  crs = [CustomRunner(MiniBatchSize, options.variables, fnq, ptReweight=options.ptReweight) for i in xrange(options.nReaders)]

  # Build the graph
  mlp = createModel([nFeatures, 100, 50, 50, nLabels], fnq.inputDataQueue, MiniBatchSize, mins, 1.0/ptps)

  #summary writer
  summary_writer = tf.summary.FileWriter(outputDirectory + "log_graph", graph=tf.get_default_graph())

  print "TRAINING MLP"

  with tf.Session(config=tf.ConfigProto(intra_op_parallelism_threads=8) ) as sess:
    sess.run(tf.global_variables_initializer())

    #start queue runners
    coord = tf.train.Coordinator()
    # start the tensorflow QueueRunner's
    qrthreads = tf.train.start_queue_runners(coord=coord, sess=sess)

    # start the file queue running
    fnq.startQueueProcess(sess)
    # we must sleep to ensure that the file queue is filled before 
    # starting the feeder queues 
    sleep(2)
    # start our custom queue runner's threads
    for cr in crs:
      cr.start_threads(sess, n_threads=options.nThreadperReader)


    print "Reporting validation loss every %i batchces with %i events per batch for %i epochs"%(ReportInterval, MiniBatchSize, NEpoch)

    i = 0
    try:
      while not coord.should_stop():
        _, summary = sess.run([mlp.train_step, mlp.merged_train_summary_op], feed_dict={mlp.reg: l2Reg})
        summary_writer.add_summary(summary, i)
        i += 1

        if not i % ReportInterval:
          validation_loss, accuracy, summary_vl = sess.run([mlp.loss_ph, mlp.accuracy, mlp.merged_valid_summary_op], feed_dict={mlp.x_ph: validData["data"][:validationCount], mlp.y_ph_: validData["labels"][:validationCount], mlp.reg: l2Reg, mlp.wgt_ph: validData["weights"][:validationCount]})
          summary_writer.add_summary(summary_vl, i)
          print('Interval %d, validation accuracy %0.6f, validation loss %0.6f' % (i/ReportInterval, accuracy, validation_loss))
          
    except tf.errors.OutOfRangeError:
      print('Done training -- epoch limit reached')
    finally:
      # When done, ask the threads to stop.
      coord.request_stop()

    coord.join(qrthreads)

    mlp.saveCheckpoint(sess, outputDirectory)
    mlp.saveModel(sess, outputDirectory)

    #y_out, yt_out = sess.run([mlp.y_ph, mlp.yt_ph], feed_dict={mlp.x_ph: npyInputData, mlp.y_ph_: npyInputAnswer, mlp.reg: l2Reg})

    #try:
    #  import matplotlib.pyplot as plt
    #  
    #  labels = DataGetter().getList()
    #  for i in xrange(0,len(labels)):
    #    for j in xrange(0,i):
    #      plt.clf()
    #      plt.xlabel(labels[i])
    #      plt.ylabel(labels[j])
    #      plt.scatter(npyInputData[:,i], npyInputData[:,j], c=y_out[:,0], s=3, cmap='coolwarm', alpha=0.8)
    #      plt.savefig("decission_boundary_%s_%s.png"%(labels[i], labels[j]))
    #except ImportError:
    #  print "matplotlib not found"

if __name__ == '__main__':
  if options.sklearnrf:
    mainSKL()
  elif options.xgboost:
    mainXGB()
  else:
    tf.app.run(main=mainTF)

  print "TRAINING DONE!"
