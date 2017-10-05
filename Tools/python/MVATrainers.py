import numpy
import pandas as pd
from DataGetter import DataGetter
from math import sqrt
from time import sleep
from glob import glob

def mainSKL(options):

  from sklearn.ensemble import RandomForestClassifier
  import pickle

  print "PROCESSING TRAINING DATA"

  # Import data
  dg = DataGetter(options.variables)
  trainData = dg.importData(options,samplesToRun = glob(options.dataFilePath + "/trainingTuple_division_0_TTbarSingleLep_training_1M_*.h5"), prescale=True, ptReweight=options.ptReweight)

  # Create random forest
  clf = RandomForestClassifier(n_estimators=500, max_depth=10, n_jobs = 4, verbose = True)
  
  print "TRAINING RF"

  # Train random forest 
  clf = clf.fit(trainData["data"], trainData["labels"][:,0], sample_weight=trainData["weights"][:,0])
  
  #Dump output from training
  fileObject = open(options.directory + "TrainingOutput.pkl",'wb')
  out = pickle.dump(clf, fileObject)
  fileObject.close()
      
  #output = clf.predict_proba(trainData["data"])[:,1]

def mainXGB(options):

  import xgboost as xgb

  print "PROCESSING TRAINING DATA"

  # Import data
  dg = DataGetter(options.variables)
  trainData = dg.importData(options, samplesToRun = glob(options.dataFilePath + "/trainingTuple_division_0_TTbarSingleLep_training_1M_*.h5"), prescale=False, ptReweight=options.ptReweight)

  print "TRAINING XGB"

  # Create xgboost classifier
  # Train random forest 
  xgData = xgb.DMatrix(trainData["data"], label=trainData["labels"][:,0], weight=trainData["weights"][:,0])
  param = {'max_depth':4 }
  gbm = xgb.train(param, xgData, num_boost_round=1000)
  
  #Dump output from training
  gbm.save_model(options.directory + 'TrainingModel.xgb')

  output = gbm.predict(xgData)


def mainTF(options):

  import tensorflow as tf
  from CreateModel import CreateModel
  from FileNameQueue import FileNameQueue
  from CustomQueueRunner import CustomQueueRunner

  print "PROCESSING TRAINING DATA"

  dg = DataGetter.DefinedVariables(options.netOp.vNames)

  print "Input Variables: ",len(dg.getList())

  # Import data
  validData = dg.importData(options, samplesToRun = options.runOp.validationSamples, ptReweight=options.runOp.ptReweight)

  #get input/output sizes
  nFeatures = validData["data"].shape[1]
  nLabels = validData["labels"].shape[1]
  nWeigts = validData["weights"].shape[1]

  #Training parameters
  l2Reg = options.runOp.l2Reg
  MiniBatchSize = options.runOp.minibatchSize
  NEpoch = options.runOp.nepoch
  ReportInterval = options.runOp.reportInterval
  validationCount = min(options.runOp.nValidationEvents, validData["data"].shape[0])

  #scale data inputs to mean 0, stddev 1
  mins = validData["data"].mean(0)
  ptps = validData["data"].std(0)
  ptps[ptps < 1e-10] = 1.0
  #npyInputData = (npyInputData - mins)/ptps

  #Create filename queue
  fnq = FileNameQueue(options.runOp.trainingSamples, NEpoch, nFeatures, nLabels, nWeigts, options.runOp.nReaders, MiniBatchSize)

  #Create CustomQueueRunner object to manage data loading 
  print "PT reweight: ", options.runOp.ptReweight
  crs = [CustomQueueRunner(MiniBatchSize, dg.getList(), fnq, ptReweight=options.runOp.ptReweight) for i in xrange(options.runOp.nReaders)]

  # Build the graph
  denseNetwork = [nFeatures]+options.netOp.denseLayers+[nLabels]
  convLayers = options.netOp.convLayers
  rnnNodes = options.netOp.rnnNodes
  rnnLayers = options.netOp.rnnLayers
  mlp = CreateModel(options, denseNetwork, convLayers, rnnNodes, rnnLayers, fnq.inputDataQueue, MiniBatchSize, mins, 1.0/ptps)

  #summary writer
  summary_writer = tf.summary.FileWriter(options.runOp.directory + "log_graph", graph=tf.get_default_graph())

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
      cr.start_threads(sess, n_threads=options.runOp.nThreadperReader)


    print "Reporting validation loss every %i batchces with %i events per batch for %i epochs"%(ReportInterval, MiniBatchSize, NEpoch)

    i = 0
    try:
      while not coord.should_stop():
        _, summary = sess.run([mlp.train_step, mlp.merged_train_summary_op], feed_dict={mlp.reg: l2Reg, mlp.keep_prob:options.runOp.keepProb})
        summary_writer.add_summary(summary, i)
        i += 1

        if i == 1 or not i % ReportInterval:
          validation_loss, accuracy, summary_vl = sess.run([mlp.loss_ph, mlp.accuracy, mlp.merged_valid_summary_op], feed_dict={mlp.x_ph: validData["data"][:validationCount], mlp.y_ph_: validData["labels"][:validationCount], mlp.reg: l2Reg, mlp.wgt_ph: validData["weights"][:validationCount]})
          summary_writer.add_summary(summary_vl, i)
          print('Interval %d, validation accuracy %0.6f, validation loss %0.6f' % (i/ReportInterval, accuracy, validation_loss))
          
    except tf.errors.OutOfRangeError:
      print('Done training -- epoch limit reached')
    finally:
      # When done, ask the threads to stop.
      coord.request_stop()

    coord.join(qrthreads)

    mlp.saveCheckpoint(sess, options.runOp.directory)
    mlp.saveModel(sess, options.runOp.directory)

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
