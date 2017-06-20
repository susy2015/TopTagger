import os
import errno
import ROOT
import numpy
import pandas as pd
import math
from MVAcommon_tf import *
import optparse
from math import sqrt
import threading

parser = optparse.OptionParser("usage: %prog [options]\n")

parser.add_option ('-k', "--sklearnrf", dest='sklearnrf', action='store_true', help="Run using sklearn RF")
parser.add_option ('-x', "--xgboost", dest='xgboost', action='store_true', help="Run using xgboost")
parser.add_option ('-d', "--directory", dest='directory', action='store', default="", help="Directory to store outputs")

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


if options.sklearnrf:
  from sklearn.ensemble import RandomForestClassifier
  import pickle
elif options.xgboost:
  import xgboost as xgb
else:
  import tensorflow as tf
  from tensorflow.python.framework import graph_io
  from tensorflow.python.tools import freeze_graph

class CustomRunner(object):
    """
    This class manages the the background threads needed to fill
        a queue full of data.
    """
    def __init__(self, batchSize, data, answers, queueX, queueY):
        self.dataX = tf.placeholder(dtype=tf.float32, shape=[None, 16])
        self.dataY = tf.placeholder(dtype=tf.float32, shape=[None, 2])

        self.data = data
        self.answers = answers

        self.batch_size = batchSize
        self.length = len(self.data)
        # The actual queue of data. 
        self.queueX = queueX

        # The symbolic operation to add data to the queue
        self.enqueue_opX = self.queueX.enqueue_many([self.dataX, self.dataY])

    def data_iterator(self):
      """ A simple data iterator """
      batch_idx = 0
      while True:
        yield self.data[batch_idx:batch_idx+self.batch_size], self.answers[batch_idx:batch_idx+self.batch_size]
        batch_idx += self.batch_size
        if batch_idx + self.batch_size > self.length:
          batch_idx = 0

    def thread_main(self, sess):
      """
      Function run on alternate thread. Basically, keep adding data to the queue.
      """
      for dataX, dataY in self.data_iterator():
        sess.run([self.enqueue_opX], feed_dict={self.dataX:dataX, self.dataY:dataY})

    def start_threads(self, sess, n_threads=1):
      qrx = tf.train.QueueRunner(self.queueX, [self.enqueue_opX] * n_threads)
      tf.train.add_queue_runner(qrx)
      
      """ Start background threads to feed queue """
      threads = []
      for n in range(n_threads):
        t = threading.Thread(target=self.thread_main, args=(sess,))
        t.daemon = True # thread will close when parent quits
        t.start()
      threads.append(t)
      return threads


def prescaleBackground(input, answer, prescale):
  return numpy.vstack([input[answer == 1], input[answer != 1][::prescale]])

def importData(samplesToRun = ["trainingTuple_division_0_TTbarSingleLep_training_1M.pkl.gz"], prescale = True, reluInputs=True, ptReweight=True):
  #variables to train
  vars = DataGetter().getList()
  
  inputData = numpy.empty([0])
  npyInputWgts = numpy.empty([0])

  for sample in samplesToRun:
    print sample
    data = pd.read_pickle(sample)
    #remove partial tops 
    inputLabels = data.as_matrix(["genConstiuentMatchesVec", "genTopMatchesVec"])
    inputAnswer = (inputLabels[:,0] == 3) & (inputLabels[:,1] == 1)
    inputBackground = (inputLabels[:,0] == 0) & numpy.logical_not(inputLabels[:,1])
    filterArray = ((inputAnswer == 1) | (inputBackground == 1)) & (data["ncand"] > 0)
    data = data[filterArray]
    inputAnswer = inputAnswer[filterArray]
    inputWgts = numpy.copy(data.as_matrix(["sampleWgt"]).astype(numpy.float32))
    
    if ptReweight:
      #calculate pt weights
      #inputWgts = numpy.empty([len(inputAnswer), 1])
      #ptBins = numpy.hstack([[0], numpy.linspace(50, 400, 36), numpy.linspace(450, 700, 6), [800, 10000]])
      ptBins = numpy.hstack([numpy.linspace(0, 2000, 51), [10000]])
      dataPt = data["cand_pt"]
      inputSampleWgts = data["sampleWgt"]
      ptHistSig, _ = numpy.histogram(dataPt[inputAnswer == 1], bins=ptBins, weights=inputSampleWgts[inputAnswer == 1])
      ptHistBg,  _ = numpy.histogram(dataPt[inputAnswer != 1], bins=ptBins, weights=inputSampleWgts[inputAnswer != 1])
      ptHistSig[ptHistSig < 10] = ptHistSig.max()
      ptHistBg[ptHistBg < 10] = ptHistBg.max()
      inputWgts[inputAnswer == 1] *= (1.0/ptHistSig[numpy.digitize(dataPt[inputAnswer == 1], ptBins) - 1]).reshape([-1,1])
      inputWgts[inputAnswer != 1] *= (1.0/ptHistBg [numpy.digitize(dataPt[inputAnswer != 1], ptBins) - 1]).reshape([-1,1])

    if len(inputData) == 0:
      inputData = data
      npyInputWgts = inputWgts
    else:
      inputData = pd.concat([inputData, data])
      npyInputWgts = numpy.vstack([npyInputWgts, inputWgts])

  #parse pandas dataframe into training data
  npyInputData = inputData.as_matrix(vars).astype(numpy.float32)
  npyInputLabels = inputData.as_matrix(["genConstiuentMatchesVec", "genTopMatchesVec"])
  npyInputAnswer = (npyInputLabels[:,0] == 3) & (npyInputLabels[:,1] == 1)
  npyInputAnswers = numpy.vstack([npyInputAnswer,numpy.logical_not(npyInputAnswer)]).transpose()
  npyInputSampleWgts = inputData.as_matrix(["sampleWgt"]).astype(numpy.float32)

  if reluInputs:
    #ensure data is all greater than one
    npyInputData[npyInputData < 0] = 0.0

  if prescale:
    #Remove background events so that bg and signal are roughly equally represented
    prescaleRatio = (npyInputAnswer != 1).sum()/(npyInputAnswer == 1).sum()

    npyInputData = prescaleBackground(npyInputData, npyInputAnswer, prescaleRatio)
    npyInputAnswers = prescaleBackground(npyInputAnswers, npyInputAnswer, prescaleRatio)
    npyInputWgts = prescaleBackground(npyInputWgts, npyInputAnswer, prescaleRatio)
    npyInputSampleWgts = prescaleBackground(npyInputSampleWgts, npyInputAnswer, prescaleRatio)

  #equalize bg and signal weights 
  nsig = npyInputWgts[npyInputAnswers[:,0] == 1].sum()
  nbg  = npyInputWgts[npyInputAnswers[:,0] != 1].sum()
  npyInputWgts[npyInputAnswers[:,0] != 1] *= nsig / nbg

  return npyInputData, npyInputAnswers, npyInputWgts, npyInputSampleWgts
    

def mainSKL():

  print "PROCESSING TRAINING DATA"

  # Import data
  npyInputData, npyInputAnswer, npyInputWgts, npyInputSampleWgts = importData(samplesToRun = ["trainingTuple_division_0_TTbarSingleLep_training_1M.pkl.gz"], prescale=True, ptReweight=True)

  #randomize input data
  perms = numpy.random.permutation(npyInputData.shape[0])
  npyInputData = npyInputData[perms]
  npyInputAnswer = npyInputAnswer[perms]
  npyInputWgts = npyInputWgts[perms]
  npyInputSampleWgts = npyInputSampleWgts[perms]

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

  print "PROCESSING TRAINING DATA"

  # Import data
  npyInputData, npyInputAnswer, npyInputWgts, npyInputSampleWgts = importData(prescale=False, ptReweight=True)

  #randomize input data
  perms = numpy.random.permutation(npyInputData.shape[0])
  npyInputData = npyInputData[perms]
  npyInputAnswer = npyInputAnswer[perms]
  npyInputWgts = npyInputWgts[perms]
  npyInputSampleWgts = npyInputSampleWgts[perms]

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

  print "PROCESSING TRAINING DATA"

  # Import data
  #npyInputData, npyInputAnswer, npyInputWgts, _       = importData(samplesToRun = ["trainingTuple_division_0_TTbarSingleLep_training_1M.pkl.gz", "trainingTuple_division_1_ZJetsToNuNu_validation_700k.pkl.gz"])
  npyInputData, npyInputAnswer, npyInputWgts, _       = importData(samplesToRun = ["trainingTuple_division_0_TTbarSingleLep_training.pkl.gz", "trainingTuple_division_1_ZJetsToNuNu_validation.pkl.gz"])
  npyValidData, npyValidAnswer, _, npyInputSampleWgts = importData(samplesToRun = ["trainingTuple_division_1_TTbarSingleLep_validation_100k.pkl.gz"])

  #scale data inputs to range 0-1
  mins = npyInputData.min(0)
  ptps = npyInputData.ptp(0)
  #npyInputData = (npyInputData - mins)/ptps

  # Build the graph
  x, y_, y, yt, w_fc, b_fc, inputDataQueue, inputAnsQueue, x_ph, y_ph_, yt_ph, y_ph = createMLP([npyInputData.shape[1], 100, 50, 50, npyInputAnswer.shape[1]], mins, 1.0/ptps)

  #randomize input data
  perms = numpy.random.permutation(npyInputData.shape[0])
  npyInputData = npyInputData[perms]
  npyInputAnswer = npyInputAnswer[perms]
  npyInputWgts = npyInputWgts[perms]
  #npyInputSampleWgts = npyInputSampleWgts[perms]

  #Create cusromRunner object to manage data loading 
  cr = CustomRunner(128, npyInputData, npyInputAnswer, inputDataQueue, inputAnsQueue)

  # other placeholders 
  reg = tf.placeholder(tf.float32)

  wgt = tf.placeholder(tf.float32, [None, 1])

  tf.add_to_collection('TrainInfo', x)
  tf.add_to_collection('TrainInfo', y)

  #cross_entropy = tf.divide(tf.reduce_sum(tf.multiply(tf.nn.softmax_cross_entropy_with_logits(labels=y_, logits=yt), wgt)), tf.reduce_sum(wgt))
  cross_entropy = tf.reduce_mean(tf.nn.softmax_cross_entropy_with_logits(labels=y_, logits=yt))
  cross_entropy_ph = tf.reduce_mean(tf.nn.softmax_cross_entropy_with_logits(labels=y_ph_, logits=yt_ph))
  l2_norm = tf.constant(0.0)
  for w in w_fc.values():
    l2_norm += tf.nn.l2_loss(w)
  loss = cross_entropy + l2_norm*reg
  loss_ph = cross_entropy_ph + l2_norm*reg
  #train_step = tf.train.GradientDescentOptimizer(1.0).minimize(cross_entropy)
  train_step = tf.train.AdamOptimizer(1e-3).minimize(loss, var_list=w_fc.values() + b_fc.values())
  correct_prediction = tf.equal(tf.argmax(y, 1), tf.argmax(y_, 1))
  accuracy = tf.reduce_mean(tf.cast(correct_prediction, tf.float32))

  # Create a summary to monitor cost tensor
  summary_ce = tf.summary.scalar("cross_entropy", cross_entropy)
  summary_l2n = tf.summary.scalar("l2_norm", l2_norm)
  summary_loss = tf.summary.scalar("loss", loss)
  summary_tloss = tf.summary.scalar("train_loss", loss)
  summary_vloss = tf.summary.scalar("valid_loss", loss_ph)
  # Create a summary to monitor accuracy tensor
  #tf.summary.scalar("accuracy", accuracy)
  # Merge all summaries into a single op
  merged_summary_op = tf.summary.merge([summary_ce, summary_l2n, summary_loss])

  print "TRAINING MLP"

  #Create saver object to same variables 
  saver = tf.train.Saver()

  with tf.Session() as sess:
    summary_writer = tf.summary.FileWriter(outputDirectory + "log_graph", graph=tf.get_default_graph())
    sess.run(tf.global_variables_initializer())

    #start queue runners
    coord = tf.train.Coordinator()
    # start the tensorflow QueueRunner's
    tf.train.start_queue_runners(coord=coord, sess=sess)
    # start our custom queue runner's threads
    cr.start_threads(sess)

    #Training parameters
    trainThreshold = 1e-6
    stepSize = 128
    MaxEpoch = 100

    stopCnt = 0
    lastLoss = 10e10
    NData = len(npyInputData)
    NSteps = NData/stepSize
    for epoch in xrange(0, MaxEpoch):

      l2Reg = 0.0001
      for i in xrange(NSteps):
        #batch = [npyInputData[0+i*stepSize:stepSize+i*stepSize,:], npyInputAnswer[0+i*stepSize:stepSize+i*stepSize,:], npyInputWgts[0+i*stepSize:stepSize+i*stepSize]]
        _, summary = sess.run([train_step, merged_summary_op], feed_dict={reg: l2Reg})
        summary_writer.add_summary(summary, epoch*NSteps + i)

      train_loss, summary_tl = sess.run([loss, summary_tloss], feed_dict={reg: l2Reg})
      validation_loss, summary_vl = sess.run([loss_ph, summary_vloss], feed_dict={x_ph: npyValidData, y_ph_: npyValidAnswer, reg: l2Reg})
      summary_writer.add_summary(summary_tl, epoch)
      summary_writer.add_summary(summary_vl, epoch)
      print('epoch %d, training loss %0.6f, validation loss %0.6f' % (epoch, train_loss, validation_loss))


    #Save training checkpoint (contains a copy of the model and the weights)
    try:
      os.mkdir(outputDirectory + "models")
    except OSError:
      pass
    checkpoint_path = outputDirectory + "models/model.ckpt"
    save_path = saver.save(sess, checkpoint_path)

    input_graph_path = outputDirectory + "tfModel.pb"
    graph_io.write_graph(sess.graph, "./", input_graph_path)

    #create frozen version of graph for distribution
    input_saver_def_path = ""
    input_binary = False
    output_node_names = "y_ph"
    restore_op_name = "save/restore_all"
    filename_tensor_name = "save/Const:0"
    output_graph_path = outputDirectory + "tfModel_frozen.pb"
    clear_devices = False

    freeze_graph.freeze_graph(input_graph_path, input_saver_def_path,
                              input_binary, checkpoint_path, output_node_names,
                              restore_op_name, filename_tensor_name,
                              output_graph_path, clear_devices, "")

    print("Model checkpoint saved in file: %s" % save_path)
    print("Frozen model (model and weights) saved in file: %s" % output_graph_path)

    y_out, yt_out = sess.run([y_ph, yt_ph], feed_dict={x_ph: npyInputData, y_ph_: npyInputAnswer, reg: 0.0001})
    
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
