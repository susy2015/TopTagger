import os
import ROOT
import numpy
import pandas as pd
import math
from MVAcommon_tf import *
import optparse
from math import sqrt

parser = optparse.OptionParser("usage: %prog [options]\n")

parser.add_option ('-k', "--sklearnrf", dest='sklearnrf', action='store_true', help="Run using sklearn RF")

options, args = parser.parse_args()

if options.sklearnrf:
  from sklearn.ensemble import RandomForestClassifier
  import pickle
else:
  import tensorflow as tf
  from tensorflow.python.framework import graph_io
  from tensorflow.python.tools import freeze_graph

def prescaleBackground(input, answer, prescale):
  return numpy.vstack([input[answer == 1], input[answer != 1][::prescale]])

def importData(prescale = True, bgnorm=True, reluInputs=True):
  #variables to train
  vars = DataGetter().getList()
  
  print "PROCESSING TRAINING DATA"

  #files to use for inputs
  samplesToRun = ["trainingTuple_division_0_TTbarSingleLep_training.pkl", "trainingTuple_division_0_ZJetsToNuNu_training.pkl"]

  inputData = numpy.empty([0])
  
  for sample in samplesToRun:
    data = pd.read_pickle(sample)
    if len(inputData) == 0:
      inputData = data
    else:
      pd.concat([inputData, data])

  #parse pandas dataframe into training data
  npyInputData = inputData.as_matrix(vars).astype(numpy.float32)
  npyInputLabels = inputData.as_matrix(["genConstiuentMatchesVec", "genTopMatchesVec"])
  npyInputAnswer = (npyInputLabels[:,0] == 3) & npyInputLabels[:,1]
  npyInputAnswers = numpy.vstack([npyInputAnswer,numpy.logical_not(npyInputAnswer)]).transpose()
  npyInputSampleWgts = inputData.as_matrix(["sampleWgt"]).astype(numpy.float32)

  #calculate pt weights
  ptHist, ptBins = numpy.histogram(inputData["cand_pt"], bins=numpy.hstack([[0], numpy.linspace(50, 400, 36), numpy.linspace(450, 700, 6), [800, 10000]]), weights=npyInputSampleWgts[:,0])

  npyInputWgts = (1.0/ptHist[numpy.digitize(inputData["cand_pt"], ptBins) - 1]).reshape([-1,1])

  if prescale:
    #Remove background events so that bg and signal are roughly equally represented
    prescale = (npyInputAnswer != 1).sum()/(npyInputAnswer == 1).sum()
    npyInputData = prescaleBackground(npyInputData, npyInputAnswer, prescale)
    npyInputAnswers = prescaleBackground(npyInputAnswers, npyInputAnswer, prescale)
    npyInputWgts = prescaleBackground(npyInputWgts, npyInputAnswer, prescale)
    npyInputSampleWgts = prescaleBackground(npyInputSampleWgts, npyInputAnswer, prescale)

  if bgnorm:
    #equalize bg and signal weights 
    nsig = npyInputWgts[npyInputAnswer].sum()
    nbg = npyInputWgts[~npyInputAnswer].sum()
    npyInputWgts[~npyInputAnswer] *= nsig / nbg

  if reluInputs:
    #ensure data is all greater than one
    npyInputData[npyInputData < 0] = 0.0
  
  return npyInputData, npyInputAnswers, npyInputWgts, npyInputSampleWgts
    

def mainSKL():

  # Import data
  npyInputData, npyInputAnswer, npyInputWgts, npyInputSampleWgts = importData(False)

  # Create random forest
  clf = RandomForestClassifier(n_estimators=500, max_depth=10, n_jobs = 4, verbose = True)
  
  print "TRAINING RF"

  # Train random forest 
  clf = clf.fit(npyInputData, npyInputAnswer[:,0], sample_weight=npyInputWgts[:,0])
  
  #Dump output from training
  fileObject = open("TrainingOutput.pkl",'wb')
  out = pickle.dump(clf, fileObject)
  fileObject.close()
      
  output = clf.predict_proba(npyInputData)[:,1]
  
  fout = ROOT.TFile("TrainingOut.root", "RECREATE")
  hDiscNoMatch = ROOT.TH1D("discNoMatch", "discNoMatch", 100, 0, 1.0)
  hDiscMatch   = ROOT.TH1D("discMatch",   "discMatch", 100, 0, 1.0)
  for i in xrange(len(output)):
      if npyInputAnswer[i,0] == 1:
          hDiscMatch.Fill(output[i], npyInputSampleWgts[i])
      else:
          hDiscNoMatch.Fill(output[i], npyInputSampleWgts[i])
  hDiscNoMatch.Write()
  hDiscMatch.Write()


def mainTF(_):
  # Import data
  npyInputData, npyInputAnswer, npyInputWgts, npyInputSampleWgts = importData()

  #scale data inputs to range 0-1
  mins = npyInputData.min(0)
  ptps = npyInputData.ptp(0)
  #npyInputData = (npyInputData - mins)/ptps

  # Build the graph
  x, y_, y, yt, w_fc, b_fc = createMLP([npyInputData.shape[1], 100, 50, 50, npyInputAnswer.shape[1]], mins, 1.0/ptps)

  reg = tf.placeholder(tf.float32)

  wgt = tf.placeholder(tf.float32, [None, 1])

  tf.add_to_collection('TrainInfo', x)
  tf.add_to_collection('TrainInfo', y)

  #cross_entropy = tf.divide(tf.reduce_sum(tf.multiply(tf.nn.softmax_cross_entropy_with_logits(labels=y_, logits=yt), wgt)), tf.reduce_sum(wgt))
  cross_entropy = tf.reduce_mean(tf.nn.softmax_cross_entropy_with_logits(labels=y_, logits=yt))
  l2_norm = tf.constant(0.0)
  for w in w_fc.values():
    l2_norm += tf.nn.l2_loss(w)
  loss = cross_entropy + l2_norm*reg
  #train_step = tf.train.GradientDescentOptimizer(1.0).minimize(cross_entropy)
  train_step = tf.train.AdamOptimizer(1e-3).minimize(loss, var_list=w_fc.values() + b_fc.values())
  correct_prediction = tf.equal(tf.argmax(y, 1), tf.argmax(y_, 1))
  accuracy = tf.reduce_mean(tf.cast(correct_prediction, tf.float32))

  # Create a summary to monitor cost tensor
  tf.summary.scalar("cross_entropy", cross_entropy)
  tf.summary.scalar("l2_norm", l2_norm)
  tf.summary.scalar("loss", loss)
  # Create a summary to monitor accuracy tensor
  tf.summary.scalar("accuracy", accuracy)
  # Merge all summaries into a single op
  merged_summary_op = tf.summary.merge_all()

  print "TRAINING MLP"

  #Create saver object to same variables 
  saver = tf.train.Saver()

  with tf.Session() as sess:
    summary_writer = tf.summary.FileWriter("log_graph", graph=tf.get_default_graph())
    sess.run(tf.global_variables_initializer())

    #Training parameters
    trainThreshold = 1e-6
    NSteps = 100
    MaxEpoch = 500

    stopCnt = 0
    lastLoss = 10e10
    NData = len(npyInputData)
    stepSize = NData/NSteps
    for epoch in xrange(0, MaxEpoch):

      #randomize input data
      perms = numpy.random.permutation(npyInputData.shape[0])
      npyInputData = npyInputData[perms]
      npyInputAnswer = npyInputAnswer[perms]
      npyInputWgts = npyInputWgts[perms]
      npyInputSampleWgts = npyInputSampleWgts[perms]
      
      losses = []
      accs = []
      for i in xrange(NSteps):
        batch = [npyInputData[0+i*stepSize:stepSize+i*stepSize,:], npyInputAnswer[0+i*stepSize:stepSize+i*stepSize,:], npyInputWgts[0+i*stepSize:stepSize+i*stepSize]]
        step_loss, _, acc, summary = sess.run([loss, train_step, accuracy, merged_summary_op], feed_dict={x: batch[0], y_: batch[1], wgt: batch[2], reg: 0.0001})
        losses.append(step_loss)
        accs.append(acc)
        summary_writer.add_summary(summary, epoch*NSteps + i)
      currentLoss = numpy.mean(losses)
      print('epoch %d, training accuracy %0.6f, training loss %0.6f' % (epoch, numpy.mean(accs), currentLoss))
      if (lastLoss - currentLoss) < trainThreshold or (lastLoss - currentLoss) < 0:
        stopCnt += 1
      else:
        stopCnt = 0
      #if stopCnt >= 3:
      #  break
      lastLoss = currentLoss

    #Save training checkpoint (contains a copy of the model and the weights 
    try:
      os.mkdir("models")
    except OSError:
      pass
    checkpoint_path = "models/model.ckpt"
    save_path = saver.save(sess, checkpoint_path)

    input_graph_path = "tfModel.pb"
    graph_io.write_graph(sess.graph, "./", input_graph_path)

    #create frozen version of graph for distribution
    input_saver_def_path = ""
    input_binary = False
    output_node_names = "y"
    restore_op_name = "save/restore_all"
    filename_tensor_name = "save/Const:0"
    output_graph_path = "tfModel_frozen.pb"
    clear_devices = False

    freeze_graph.freeze_graph(input_graph_path, input_saver_def_path,
                              input_binary, checkpoint_path, output_node_names,
                              restore_op_name, filename_tensor_name,
                              output_graph_path, clear_devices, "")

    print("Model checkpoint saved in file: %s" % save_path)
    print("Frozen model (model and weights) saved in file: %s" % output_graph_path)

    y_out, yt_out = sess.run([y, yt], feed_dict={x: npyInputData, y_: npyInputAnswer, reg: 0.0001})
    
    fout = ROOT.TFile("TrainingOut.root", "RECREATE")
    hDiscNoMatch = ROOT.TH1D("discNoMatch", "discNoMatch", 100, 0, 1.0)
    hDiscMatch   = ROOT.TH1D("discMatch",   "discMatch", 100, 0, 1.0)
    output = y_out[:,0]
    for i in xrange(len(output)):
        if npyInputAnswer[i,0] == 1:
            hDiscMatch.Fill(output[i], npyInputSampleWgts[i])
        else:
            hDiscNoMatch.Fill(output[i], npyInputSampleWgts[i])
    hDiscNoMatch.Write()
    hDiscMatch.Write()

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
  else:
    tf.app.run(main=mainTF)

  print "TRAINING DONE!"
