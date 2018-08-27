import sys
import os
import errno
import pandas as pd
import numpy
import math
from DataGetter import DataGetter
import optparse
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import pickle
from taggerOptions import *
from glob import glob

dataPath = "/cms/data/cmadrid/EventShapeTrainingData_V2"

def load_graph(frozen_graph_filename):
    # We load the protobuf file from the disk and parse it to retrieve the 
    # unserialized graph_def
    with tf.gfile.GFile(frozen_graph_filename, "rb") as f:
        graph_def = tf.GraphDef()
        graph_def.ParseFromString(f.read())

    # Then, we can use again a convenient built-in function to import a graph_def into the 
    # current default Graph
    with tf.Graph().as_default() as graph:
        tf.import_graph_def(
            graph_def, 
            input_map=None, 
            return_elements=None, 
            name="", 
            op_dict=None, 
            producer_op_list=None
        )
    return graph

parser = optparse.OptionParser("usage: %prog [options]\n")

parser.add_option ('-c', "--disc",         dest='discCut',      action='store', default=0.6, type=float, help="Discriminator cut")
parser.add_option ('-k', "--sklrf",        dest='sklrf',        action='store_true',                     help="Use skl random forest instead of tensorflow")
parser.add_option ('-x', "--xgboost",      dest='xgboost',      action='store_true',                     help="Run using xgboost")
parser.add_option ('-a', "--mvaFile",      dest='mvaFile',      action='store', default="",              help="Mva training file")
parser.add_option ('-f', "--dataFilePath", dest="dataFilePath", action='store',                          help="Path where the input datafiles are stored (default: \"data\")")
parser.add_option ('-d', "--directory",    dest='directory',    action='store', default="",              help="Directory to store outputs")
parser.add_option ('-v', "--variables",    dest='variables',    action='store',                          help="Input features to use")
parser.add_option ('-m', "--modelCfg",     dest="modelJSON",    action='store',                          help="JSON with model definitions")
parser.add_option ('-j', "--trainingCfg",  dest="trainingCfg",  action='store',                          help="config file produced by training")
 
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

  
#disc cut
discCut = options.discCut

def makeDiscPlots(dataTTbar, dataTTbarTrain, dataTTbarAns, dataTTbarAnsTrain, genMatches, genMatchesTrain, suffix=""):

    bins=discBins = numpy.linspace(0, 1.0, 51)

    plt.clf()
    filterVec = (dataTTbar.cand_pt < 100).as_matrix()
    filterVecTrain = (dataTTbarTrain.cand_pt < 100).as_matrix()
    plt.hist(dataTTbarAns[filterVec & (genMatches == 1)], weights=dataTTbar["sampleWgt"][filterVec & (genMatches == 1)], bins=discBins, normed=True, label="Gen Matched",     fill=False, histtype='step', edgecolor="red")
    plt.hist(dataTTbarAns[filterVec & (genMatches != 1)], weights=dataTTbar["sampleWgt"][filterVec & (genMatches != 1)], bins=discBins, normed=True, label="Not gen matched", fill=False, histtype='step', edgecolor="blue")
    plt.hist(dataTTbarAnsTrain[filterVecTrain & (genMatchesTrain == 1)], weights=dataTTbarTrain["sampleWgt"][filterVecTrain & (genMatchesTrain == 1)], bins=discBins, normed=True, label="Gen Matched Train",     fill=False, histtype='step', linestyle="dotted", edgecolor="red")
    plt.hist(dataTTbarAnsTrain[filterVecTrain & (genMatchesTrain != 1)], weights=dataTTbarTrain["sampleWgt"][filterVecTrain & (genMatchesTrain != 1)], bins=discBins, normed=True, label="Not gen matched Train", fill=False, histtype='step', linestyle="dotted", edgecolor="blue")
    plt.legend(loc='upper right')
    plt.xlabel("Discriminator")
    plt.ylabel("Normalized events")
    plt.savefig(outputDirectory + "discriminator_0_100" + suffix + ".png")
    plt.close()
    
    plt.clf()
    filterVec = ((100 <= dataTTbar.cand_pt) & (dataTTbar.cand_pt < 200)).as_matrix()
    filterVecTrain = ((100 <= dataTTbarTrain.cand_pt) & (dataTTbarTrain.cand_pt < 200)).as_matrix()
    plt.hist(dataTTbarAns[filterVec & (genMatches == 1)], weights=dataTTbar["sampleWgt"][filterVec & (genMatches == 1)], bins=discBins, normed=True, label="Gen Matched",     fill=False, histtype='step', edgecolor="red")
    plt.hist(dataTTbarAns[filterVec & (genMatches != 1)], weights=dataTTbar["sampleWgt"][filterVec & (genMatches != 1)], bins=discBins, normed=True, label="Not gen matched", fill=False, histtype='step', edgecolor="blue")
    plt.hist(dataTTbarAnsTrain[filterVecTrain & (genMatchesTrain == 1)], weights=dataTTbarTrain["sampleWgt"][filterVecTrain & (genMatchesTrain == 1)], bins=discBins, normed=True, label="Gen Matched Train",     fill=False, histtype='step', linestyle="dotted", edgecolor="red")
    plt.hist(dataTTbarAnsTrain[filterVecTrain & (genMatchesTrain != 1)], weights=dataTTbarTrain["sampleWgt"][filterVecTrain & (genMatchesTrain != 1)], bins=discBins, normed=True, label="Not gen matched Train", fill=False, histtype='step', linestyle="dotted", edgecolor="blue")
    plt.legend(loc='upper right')
    plt.xlabel("Discriminator")
    plt.ylabel("Normalized events")
    plt.savefig(outputDirectory + "discriminator_100_200" + suffix + ".png")
    plt.close()
    
    plt.clf()
    filterVec = ((200 <= dataTTbar.cand_pt) & (dataTTbar.cand_pt < 300)).as_matrix()
    filterVecTrain = ((200 <= dataTTbarTrain.cand_pt) & (dataTTbarTrain.cand_pt < 300)).as_matrix()
    plt.hist(dataTTbarAns[filterVec & (genMatches == 1)], weights=dataTTbar["sampleWgt"][filterVec & (genMatches == 1)], bins=discBins, normed=True, label="Gen Matched",     fill=False, histtype='step', edgecolor="red")
    plt.hist(dataTTbarAns[filterVec & (genMatches != 1)], weights=dataTTbar["sampleWgt"][filterVec & (genMatches != 1)], bins=discBins, normed=True, label="Not gen matched", fill=False, histtype='step', edgecolor="blue")
    plt.hist(dataTTbarAnsTrain[filterVecTrain & (genMatchesTrain == 1)], weights=dataTTbarTrain["sampleWgt"][filterVecTrain & (genMatchesTrain == 1)], bins=discBins, normed=True, label="Gen Matched Train",     fill=False, histtype='step', linestyle="dotted", edgecolor="red")
    plt.hist(dataTTbarAnsTrain[filterVecTrain & (genMatchesTrain != 1)], weights=dataTTbarTrain["sampleWgt"][filterVecTrain & (genMatchesTrain != 1)], bins=discBins, normed=True, label="Not gen matched Train", fill=False, histtype='step', linestyle="dotted", edgecolor="blue")
    plt.legend(loc='upper right')
    plt.xlabel("Discriminator")
    plt.ylabel("Normalized events")
    plt.savefig(outputDirectory + "discriminator_200_300" + suffix + ".png")
    plt.close()
    
    plt.clf()
    filterVec = ((300 <= dataTTbar.cand_pt) & (dataTTbar.cand_pt < 400)).as_matrix()
    filterVecTrain = ((300 <= dataTTbarTrain.cand_pt) & (dataTTbarTrain.cand_pt < 400)).as_matrix()
    plt.hist(dataTTbarAns[filterVec & (genMatches == 1)], weights=dataTTbar["sampleWgt"][filterVec & (genMatches == 1)], bins=discBins, normed=True, label="Gen Matched",     fill=False, histtype='step', edgecolor="red")
    plt.hist(dataTTbarAns[filterVec & (genMatches != 1)], weights=dataTTbar["sampleWgt"][filterVec & (genMatches != 1)], bins=discBins, normed=True, label="Not gen matched", fill=False, histtype='step', edgecolor="blue")
    plt.hist(dataTTbarAnsTrain[filterVecTrain & (genMatchesTrain == 1)], weights=dataTTbarTrain["sampleWgt"][filterVecTrain & (genMatchesTrain == 1)], bins=discBins, normed=True, label="Gen Matched Train",     fill=False, histtype='step', linestyle="dotted", edgecolor="red")
    plt.hist(dataTTbarAnsTrain[filterVecTrain & (genMatchesTrain != 1)], weights=dataTTbarTrain["sampleWgt"][filterVecTrain & (genMatchesTrain != 1)], bins=discBins, normed=True, label="Not gen matched Train", fill=False, histtype='step', linestyle="dotted", edgecolor="blue")
    plt.legend(loc='upper right')
    plt.xlabel("Discriminator")
    plt.ylabel("Normalized events")
    plt.savefig(outputDirectory + "discriminator_300_400" + suffix + ".png")
    plt.close()
    
    plt.clf()
    filterVec = ((400 <= dataTTbar.cand_pt)).as_matrix()
    filterVecTrain = ((400 <= dataTTbarTrain.cand_pt)).as_matrix()
    plt.hist(dataTTbarAns[filterVec & (genMatches == 1)], weights=dataTTbar["sampleWgt"][filterVec & (genMatches == 1)], bins=discBins, normed=True, label="Gen Matched",     fill=False, histtype='step', edgecolor="red")
    plt.hist(dataTTbarAns[filterVec & (genMatches != 1)], weights=dataTTbar["sampleWgt"][filterVec & (genMatches != 1)], bins=discBins, normed=True, label="Not gen matched", fill=False, histtype='step', edgecolor="blue")
    plt.hist(dataTTbarAnsTrain[filterVecTrain & (genMatchesTrain == 1)], weights=dataTTbarTrain["sampleWgt"][filterVecTrain & (genMatchesTrain == 1)], bins=discBins, normed=True, label="Gen Matched Train",     fill=False, histtype='step', linestyle="dotted", edgecolor="red")
    plt.hist(dataTTbarAnsTrain[filterVecTrain & (genMatchesTrain != 1)], weights=dataTTbarTrain["sampleWgt"][filterVecTrain & (genMatchesTrain != 1)], bins=discBins, normed=True, label="Not gen matched Train", fill=False, histtype='step', linestyle="dotted", edgecolor="blue")
    plt.legend(loc='upper right')
    plt.xlabel("Discriminator")
    plt.ylabel("Normalized events")
    plt.savefig(outputDirectory + "discriminator_400_Inf" + suffix + ".png")
    plt.close()
    
    plt.clf()
    plt.hist(dataTTbarAns[genMatches == 1], weights=dataTTbar["sampleWgt"][genMatches == 1], bins=discBins, normed=True, label="Gen Matched Validation",     fill=False, histtype='step', edgecolor="red")
    plt.hist(dataTTbarAns[genMatches != 1], weights=dataTTbar["sampleWgt"][genMatches != 1], bins=discBins, normed=True, label="Not gen matched Validation", fill=False, histtype='step', edgecolor="blue")
    plt.hist(dataTTbarAnsTrain[genMatchesTrain == 1], weights=dataTTbarTrain["sampleWgt"][genMatchesTrain == 1], bins=discBins, normed=True, label="Gen Matched Train",     fill=False, histtype='step', linestyle="dotted", linewidth=2, edgecolor="red")
    plt.hist(dataTTbarAnsTrain[genMatchesTrain != 1], weights=dataTTbarTrain["sampleWgt"][genMatchesTrain != 1], bins=discBins, normed=True, label="Not gen matched Train", fill=False, histtype='step', linestyle="dotted", linewidth=2, edgecolor="blue")
    plt.legend(loc='upper right')
    plt.xlabel("Discriminator")
    plt.ylabel("Normalized events")
    plt.savefig(outputDirectory + "discriminator_validVsTrain" + suffix + ".png")
    plt.close()
    
    plt.clf()
    plt.hist(dataTTbarAns[dataTTbar.cand_pt.as_matrix() > 200][genMatches[dataTTbar.cand_pt.as_matrix() > 200] == 1], weights=dataTTbar[dataTTbar.cand_pt.as_matrix() > 200]["sampleWgt"][genMatches[dataTTbar.cand_pt.as_matrix() > 200] == 1], bins=discBins, normed=True, label="Gen Matched Validation",     fill=False, histtype='step', edgecolor="red")
    plt.hist(dataTTbarAns[dataTTbar.cand_pt.as_matrix() > 200][genMatches[dataTTbar.cand_pt.as_matrix() > 200] != 1], weights=dataTTbar[dataTTbar.cand_pt.as_matrix() > 200]["sampleWgt"][genMatches[dataTTbar.cand_pt.as_matrix() > 200] != 1], bins=discBins, normed=True, label="Not gen matched Validation", fill=False, histtype='step', edgecolor="blue")
    plt.hist(dataTTbarAnsTrain[dataTTbarTrain.cand_pt.as_matrix() > 200][genMatchesTrain[dataTTbarTrain.cand_pt.as_matrix() > 200] == 1], weights=dataTTbarTrain[dataTTbarTrain.cand_pt.as_matrix() > 200]["sampleWgt"][genMatchesTrain[dataTTbarTrain.cand_pt.as_matrix() > 200] == 1], bins=discBins, normed=True, label="Gen Matched Train",     fill=False, histtype='step', linestyle="dotted", linewidth=2, edgecolor="red")
    plt.hist(dataTTbarAnsTrain[dataTTbarTrain.cand_pt.as_matrix() > 200][genMatchesTrain[dataTTbarTrain.cand_pt.as_matrix() > 200] != 1], weights=dataTTbarTrain[dataTTbarTrain.cand_pt.as_matrix() > 200]["sampleWgt"][genMatchesTrain[dataTTbarTrain.cand_pt.as_matrix() > 200] != 1], bins=discBins, normed=True, label="Not gen matched Train", fill=False, histtype='step', linestyle="dotted", linewidth=2, edgecolor="blue")
    plt.legend(loc='upper right')
    plt.xlabel("Discriminator")
    plt.ylabel("Normalized events")
    plt.savefig(outputDirectory + "discriminator_validVsTrain_highpt" + suffix + ".png")
    plt.close()

print "RETRIEVING MODEL FILE"

if options.sklrf:
    from sklearn.ensemble import RandomForestClassifier
    
    if len(options.mvaFile):
        fileTraining = open(options.mvaFile,'r')
    else:
        fileTraining = open(outputDirectory + "TrainingOutput.pkl",'r')
    clf1 = pickle.load(fileTraining)
    fileTraining.close()

elif options.xgboost:
    import xgboost as xgb

    if len(options.mvaFile):
        bst = xgb.Booster(model_file=options.mvaFile) # load data
    else:
        bst = xgb.Booster(model_file=outputDirectory+"./TrainingModel.xgb") # load data

else:
    import tensorflow as tf

    ##TODONE: Switchen to frozen model file 
    ##Get training output
    #saver = tf.train.import_meta_graph(outputDirectory + 'models/model.ckpt.meta')
    #sess = tf.Session()
    ## To initialize values with saved data
    #saver.restore(sess, outputDirectory + './models/model.ckpt')
    ## Restrieve useful variables
    #trainInfo = tf.get_collection('TrainInfo')
    #graph = tf.get_default_graph()
    #x = graph.get_tensor_by_name('x:0')
    #y_train = graph.get_tensor_by_name('y_ph:0')
    ##x = trainInfo[0]
    ##y_train = trainInfo[1]

    # We use our "load_graph" function
    if len(options.mvaFile):
        graph = load_graph(options.mvaFile)
    else:
        graph = load_graph(outputDirectory + "./tfModel_frozen.pb")
    
    # create the tf session
    sess = tf.Session(graph=graph)
    
    # We access the input and output nodes 
    x = graph.get_tensor_by_name('x:0')
    y_train = graph.get_tensor_by_name('y_ph:0')



print "PROCESSING TTBAR VALIDATION DATA"

#Let us see if the input variables have been set by JSON
import json

#It is better to load the Training configuration file, then to override the configuration at the command-line
if options.trainingCfg != None:
   jsonName = outputDirectory+options.trainingCfg
else:
   jsonName = outputDirectory+"config.json"

trainingOptions = taggerOptions.loadJSON(jsonName)


if options.modelJSON != None:
  try:
    f = open(options.modelJSON,"r")
    cfgs = json.load(f)
  except IOError:
    print "Unable to open", options.modelJSON
  else:
    print "Loading",options.variables,"from",options.modelJSON
    trainingOptions.netOp.vNames = cfgs[options.variables] #the json file is a dictionary, cloptions.variables specifies the key to use

elif options.variables != None:
  inputVariables, __ = StandardVariables(options.variables)

  trainingOptions.netOp.numPassThru      = len(inputVariables)
  trainingOptions.netOp.vNames           = inputVariables

dg = DataGetter.DefinedVariables(trainingOptions.netOp.vNames)
varsname = dg.getList()

print varsname

#dataTTbarAll = pd.read_pickle("trainingTuple_division_1_TTbarSingleLep_validation_2bseed.pkl.gz")
#dataTTbarAll = pd.read_pickle("trainingTuple_division_1_TTbarSingleLep_validation_TeamASel.pkl.gz")
#dataTTbarAll = pd.read_pickle("trainingTuple_division_1_TTbarSingleLep_validation_jpt20_nocone.pkl.gz")
#dataTTbarAll = pd.read_pickle("trainingTuple_division_1_TTbarSingleLep_validation_100k.pkl.gz")

if options.dataFilePath != None: 
    if options.dataFilePath[-1] != "/": # check for /
        options.dataFilePath += "/"
    trainingOptions.runOp.dataPath = options.dataFilePath


def getData(dataName):
    if ".pkl" in dataName:
        dataAll = pd.read_pickle(dataName)
    elif ".h5" in dataName:
        import h5py
        #f = h5py.File(trainingOptions.runOp.dataPath + dataName, "r")
        f = h5py.File(dataName, "r")
        npData = f["EventShapeVar"][:]
        columnHeaders = f["EventShapeVar"].attrs["column_headers"]
        #indices = [npData[:,0].astype(numpy.int), npData[:,1].astype(numpy.int)]
        #dataAll = pd.DataFrame(npData[:,2:], index=pd.MultiIndex.from_arrays(indices), columns=columnHeaders[2:])
        dataAll = pd.DataFrame(npData, columns=columnHeaders)
    
        f.close()
    
    return dataAll

def getDataTTbar(ttsig):
    dataTTbarAllSamples = []
    for f in ttsig:
        dataTTbarTAll = getData(f)
        #print dataTTbarTAll[0:100]
        #print dataTTbarTGen[0:100]
        dataTTbarAllSamples.append(dataTTbarTAll)

    dataTTbarAll = pd.concat(dataTTbarAllSamples)

#    NTEVENTS = 60995379.0 + 60125087.0
#    dataTTbarAll.sampleWgt *= NTEVENTS/dataTTbarTAll.shape[0]

    return dataTTbarAll

dataTTbarAll = getDataTTbar(glob(dataPath + "/trainingTuple_*_division_2_TT_test_0.h5"))
dataTTbarAllTrain = getDataTTbar(glob(dataPath + "/trainingTuple_*_division_0_TT_training_0.h5"))

dataSigAll = getDataTTbar(glob(dataPath + "/trainingTuple_*_division_2_rpv_stop_*_test_0.h5"))
dataSigAllTrain = getDataTTbar(glob(dataPath + "/trainingTuple_*_division_0_rpv_stop_*_training_0.h5"))

print "CALCULATING TTBAR DISCRIMINATORS"

discNum = 4

def npSigmoid(x, pt):
    return x#1 / (1 + numpy.exp(-(x - 0.5 - 0.3/400*pt)*8))
    

if options.sklrf:
    dataTTbarAns = clf1.predict_proba(dataTTbarAll.as_matrix(varsname))[:,1]
    dataTTbarAnsTrain = clf1.predict_proba(dataTTbarAllTrain.as_matrix(varsname))[:,1]

    dataSigAns = clf1.predict_proba(dataSigAll.as_matrix(varsname))[:,1]
    dataSigAnsTrain = clf1.predict_proba(dataSigAllTrain.as_matrix(varsname))[:,1]

    discCutTTbar = numpy.array([discCut]*len(dataTTbarAns))
    discCutTTbarTrain = numpy.array([discCut]*len(dataTTbarAnsTrain))

    discCutSig = numpy.array([discCut]*len(dataSigAns))
    discCutSigTrain = numpy.array([discCut]*len(dataSigAnsTrain))

elif options.xgboost:
    xgData = xgb.DMatrix(dataTTbar.as_matrix(varsname))
    dataTTbarAns = bst.predict(xgData)

    xgData = xgb.DMatrix(dataTTbar.as_matrix(varsname))
    dataTTbarAnsTrain = bst.predict(xgb.DMatrix(dataTTbarTrain.as_matrix(varsname)))

    discCutTTbar = numpy.array([discCut]*len(dataTTbarAns))

    discCutTTbarTrain = numpy.array([discCut]*len(dataTTbarAnsTrain))

else:
    dataTTbarAns = sess.run(y_train, feed_dict={x: dataTTbarAll.as_matrix(varsname)})[:,0]
    dataTTbarAnsTrain = sess.run(y_train, feed_dict={x: dataTTbarAllTrain.as_matrix(varsname)})[:,0]

    dataSigAns = sess.run(y_train, feed_dict={x: dataSigAll.as_matrix(varsname)})[:,0]
    dataSigAnsTrain = sess.run(y_train, feed_dict={x: dataSigAllTrain.as_matrix(varsname)})[:,0]

    discCutTTbar = numpy.array([discCut]*len(dataTTbarAns))
    discCutTTbarTrain = numpy.array([discCut]*len(dataTTbarAnsTrain))

    discCutSig = numpy.array([discCut]*len(dataSigAns))
    discCutSigTrain = numpy.array([discCut]*len(dataSigAnsTrain))

    #discCutTTbar = 0.75 + (dataTTbar.cand_pt * 0.22/300.0)
    #discCutTTbar[discCutTTbar > 0.97] = 0.97
    #
    #discCutTTbarTrain = 0.75 + (dataTTbarTrain.cand_pt * 0.22/300.0)
    #discCutTTbarTrain[discCutTTbarTrain > 0.97] = 0.97

print "CREATING HISTOGRAMS"

#Discriminator plot

bins=discBins = numpy.linspace(0, 1.0, 51)

print dataTTbarAns.shape, dataTTbarAll["sampleWgt"].shape
plt.clf()
plt.hist(dataTTbarAns,      weights=dataTTbarAll["sampleWgt"],      bins=discBins, normed=True, label="Background Val",     fill=False, histtype='step', edgecolor="red")
plt.hist(dataTTbarAnsTrain, weights=dataTTbarAllTrain["sampleWgt"], bins=discBins, normed=True, label="Background Train", fill=False, histtype='step', edgecolor="blue")
plt.hist(dataSigAns,        weights=dataSigAll["sampleWgt"],        bins=discBins, normed=True, label="Sig Val",     fill=False, histtype='step', edgecolor="green")
plt.hist(dataSigAnsTrain,   weights=dataSigAllTrain["sampleWgt"],   bins=discBins, normed=True, label="Sig Train", fill=False, histtype='step', edgecolor="xkcd:aubergine")
plt.legend(loc='upper right')
plt.xlabel("Discriminator")
plt.ylabel("Normalized events")
plt.savefig(outputDirectory + "discriminator.png")
plt.close()


#makeDiscPlots(dataTTbar, dataTTbarTrain, dataTTbarAns, dataTTbarAnsTrain, genMatches, genMatchesTrain)
#
#
##input variable histograms
#
#genTopData = dataTTbar[genMatches == 1]
#genBGData = dataTTbar[genMatches != 1]
#recoTopData = dataTTbar[dataTTbarAns > discCutTTbar]
#recoBGData = dataTTbar[dataTTbarAns < discCutTTbar]
#minTTbar = dataTTbar.min()
#maxTTbar = dataTTbar.max()
#
#for var in varsname:
#    plt.clf()
#    bins = numpy.linspace(minTTbar[var], maxTTbar[var], 21)
#    ax = recoTopData .hist(column=var, weights=recoTopData["sampleWgt"], bins=bins, grid=False, normed=True, fill=False, histtype='step',                     label="reco top")
#    recoBGData       .hist(column=var, weights=recoBGData["sampleWgt"],  bins=bins, grid=False, normed=True, fill=False, histtype='step',                     label="reco bg", ax=ax)
#    genTopData       .hist(column=var, weights=genTopData["sampleWgt"],  bins=bins, grid=False, normed=True, fill=False, histtype='step', linestyle="dotted", label="gen top", ax=ax)
#    genBGData        .hist(column=var, weights=genBGData["sampleWgt"],   bins=bins, grid=False, normed=True, fill=False, histtype='step', linestyle="dotted", label="gen bkg", ax=ax)
#    plt.legend()
#    plt.xlabel(var)
#    plt.ylabel("Normalized events")
#    plt.yscale('log')
#    plt.savefig(outputDirectory + var + ".png")
#    plt.close()
#

print "VALIDATION DONE!"
