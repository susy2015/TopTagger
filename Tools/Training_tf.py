import os
import ROOT
import numpy
import math
import tensorflow as tf
from tensorflow.python.framework import graph_io
from tensorflow.python.tools import freeze_graph
from MVAcommon_tf import *
import optparse
from math import sqrt

#parser = optparse.OptionParser("usage: %prog [options]\n")
#
#parser.add_option ('-o', "--opencv", dest='opencv', action='store_true', help="Run using opencv RTrees")
#parser.add_option ('-n', "--noRoc", dest='noROC', action='store_true', help="Do not calculate ROC to save time")
#
#options, args = parser.parse_args()

def importData():
  dg = DataGetter()
  
  print "PROCESSING TRAINING DATA"
  
  samplesToRun = ["trainingTuple_division_0_TTbarSingleLep_training.root", "trainingTuple_division_0_ZJetsToNuNu_training.root"]
  
  inputData = []
  inputAnswer = []
  inputWgts = []
  inputSampleWgts = []
  
  for datasetName in samplesToRun:
      dataset = ROOT.TFile.Open(datasetName)
      print datasetName
  
      hPtMatch   = ROOT.TH1D("hPtMatch" + datasetName, "hPtMatch", 50, 0.0, 2000.0)
      hPtMatch.Sumw2()
      hPtNoMatch = ROOT.TH1D("hPtNoMatch" + datasetName, "hPtNoMatch", 50, 0.0, 2000.0)
      hPtNoMatch.Sumw2()
  
      #Nevts = 0
      #for event in dataset.slimmedTuple:
      #    if Nevts >= NEVTS:
      #        break
      #    Nevts +=1
      #    for i in xrange(len(event.genConstiuentMatchesVec)):
      #        nmatch = event.genConstiuentMatchesVec[i]
      #        if (int(nmatch == 3) and event.genTopMatchesVec[i]) or (int(nmatch == 0) and not event.genTopMatchesVec[i]):
      #            if nmatch == 3 and event.genTopMatchesVec[i]:
      #                #            if event.genTopMatchesVec[i]:
      #                hPtMatch.Fill(event.cand_pt[i], event.sampleWgt)
      #            else:
      #                hPtNoMatch.Fill(event.cand_pt[i], event.sampleWgt)
      
      Nevts = 0
      count = 0
      for event in dataset.slimmedTuple:
          if Nevts >= NEVTS:
              break
          Nevts +=1
          for i in xrange(len(event.cand_m)):
              nmatch = event.genConstiuentMatchesVec[i]
              if True:#(int(nmatch == 3) and event.genTopMatchesVec[i]) or (int(nmatch == 0) and not event.genTopMatchesVec[i]):
                  if (int(nmatch != 3) or not event.genTopMatchesVec[i]):
                      count += 1
                      if count%15:
                          continue
                  inputData.append(dg.getData(event, i))
                  answer = nmatch == 3 and event.genTopMatchesVec[i]
                  inputAnswer.append([int(answer), int((nmatch == 3 and not event.genTopMatchesVec[i]) or nmatch == 2), int(nmatch == 1), int(nmatch == 0)])
                  inputWgts.append(event.sampleWgt)
                  inputSampleWgts.append(event.sampleWgt)
  #                if int(nmatch == 3) and event.genTopMatchesVec[i]:
  #                    if hPtMatch.GetBinContent(hPtMatch.FindBin(event.cand_pt[i])) > 10:
  #                        inputWgts.append(1.0 / hPtMatch.GetBinContent(hPtMatch.FindBin(event.cand_pt[i])))
  #                    else:
  #                        inputWgts.append(0.0)
  #                else:
  #                    if hPtNoMatch.GetBinContent(hPtNoMatch.FindBin(event.cand_pt[i])) > 10:
  #                        inputWgts.append(1.0 / hPtNoMatch.GetBinContent(hPtNoMatch.FindBin(event.cand_pt[i])))
  #                    else:
  #                        inputWgts.append(0.0)
  
                  
  npyInputData = numpy.array(inputData, numpy.float32)
  npyInputAnswer = numpy.array(inputAnswer, numpy.float32)
  npyInputWgts = numpy.array(inputWgts, numpy.float32)
  npyInputSampleWgts = numpy.array(inputSampleWgts, numpy.float32)
  
  nSig = len(npyInputWgts[npyInputAnswer[:,0]==1])#sum()
  nBg = len(npyInputWgts[npyInputAnswer[:,0]==0])#sum()
  #
  print nSig
  print nBg
  #
  ##Equalize the relative weights of signal and bg
  #for i in xrange(len(npyInputAnswer)):
  #    if npyInputAnswer[i] == 0:
  #        npyInputWgts[i] *= nSig/nBg
  #
  #nSig = npyInputWgts[npyInputAnswer==1].sum()
  #nBg = npyInputWgts[npyInputAnswer==0].sum()
  #
  #print nSig
  #print nBg
  
  #randomize input data
  perms = numpy.random.permutation(npyInputData.shape[0])
  npyInputData = npyInputData[perms]
  npyInputAnswer = npyInputAnswer[perms]
  npyInputWgts = npyInputWgts[perms]
  npyInputSampleWgts = npyInputSampleWgts[perms]

  #scale data inputs to range 0-1
  mins = npyInputData.min(0)
  ptps = npyInputData.ptp(0)
  npyInputData = (npyInputData - mins) / ptps

  #save data scales
  with open("DataScales.txt", "w") as f:
      f.write("mins: ")
      f.write(" ".join([str(s) for s in mins]))
      f.write("\n")
      f.write("ptps: ")
      f.write(" ".join([str(s) for s in ptps]))
      f.write("\n")

  return npyInputData, npyInputAnswer, npyInputWgts, npyInputSampleWgts
    

def main(_):
  # Import data
  npyInputData, npyInputAnswer, npyInputWgts, npyInputSampleWgts = importData()

  # Build the graph for the deep net
  x, y_, y = createMLP([npyInputData.shape[1], 50, npyInputAnswer.shape[1]])

  reg = tf.placeholder(tf.float32)

  tf.add_to_collection('TrainInfo', x)
  tf.add_to_collection('TrainInfo', y)

  cross_entropy = tf.reduce_mean(tf.nn.softmax_cross_entropy_with_logits(labels=y_, logits=y))
  l2_norm = tf.nn.l2_loss(y, name=None)
  loss = cross_entropy + l2_norm*reg
  #train_step = tf.train.GradientDescentOptimizer(1.0).minimize(cross_entropy)
  train_step = tf.train.AdamOptimizer(1e-3).minimize(loss)
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

  print "TRAINING MVA"

  #Create saver object to same variables 
  saver = tf.train.Saver()

  with tf.Session() as sess:
    summary_writer = tf.summary.FileWriter("log_graph", graph=tf.get_default_graph())
    sess.run(tf.global_variables_initializer())

    trainThreshold = 1e-4
    stopCnt = 0
    lastLoss = 10e10
    NData = len(npyInputData)
    NSteps = 10
    stepSize = NData/NSteps
    MaxEpoch = 500
    for epoch in xrange(0, MaxEpoch):
      losses = []
      accs = []
      for i in xrange(NSteps):
        batch = [npyInputData[0+i*stepSize:stepSize+i*stepSize,:], npyInputAnswer[0+i*stepSize:stepSize+i*stepSize,:]]
        #train_step.run(feed_dict={x: batch[0], y_: batch[1], keep_prob: 0.5})
        step_loss, _, acc, summary = sess.run([loss, train_step, accuracy, merged_summary_op], feed_dict={x: batch[0], y_: batch[1], reg: 0.0001})
        losses.append(step_loss)
        accs.append(acc)
        summary_writer.add_summary(summary, epoch*NSteps + i)
      currentLoss = numpy.mean(losses)
      print('epoch %d, training accuracy %s, training loss %s' % (epoch, numpy.mean(accs), currentLoss))
      if (lastLoss - currentLoss) < trainThreshold or (lastLoss - currentLoss) < 0:
        stopCnt += 1
      else:
        stopCnt = 0
      if stopCnt >= 3:
        break
      lastLoss = currentLoss

    #Save training checkout (contains a copy of the model and the weights 
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

if __name__ == '__main__':
  tf.app.run(main=main)

  print "TRAINING DONE!"
