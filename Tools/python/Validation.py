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

#parser.add_option ('-o', "--opencv", dest='opencv', action='store_true', help="Run using opencv RTrees")
#parser.add_option ('-n', "--noRoc", dest='noROC', action='store_true', help="Do not calculate ROC to save time")
parser.add_option ('-c', "--disc", dest='discCut', action='store', default=0.6, type=float, help="Discriminator cut")
parser.add_option ('-k', "--sklrf", dest='sklrf', action='store_true', help="Use skl random forest instead of tensorflow")
parser.add_option ('-x', "--xgboost", dest='xgboost', action='store_true', help="Run using xgboost")
parser.add_option ('-a', "--mvaFile", dest='mvaFile', action='store', default="", help="Mva training file")
parser.add_option ('-f', "--dataFilePath",      dest="dataFilePath",      action='store',                     help="Path where the input datafiles are stored (default: \"data\")")
parser.add_option ('-d', "--directory", dest='directory', action='store', default="", help="Directory to store outputs")
parser.add_option ('-v', "--variables", dest='variables', action='store', help="Input features to use")
parser.add_option ('-m', "--modelCfg",          dest="modelJSON",         action='store',      help="JSON with model definitions")
parser.add_option ('-j', "--trainingCfg", dest="trainingCfg", action='store', help="config file produced by training")

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
    #saver = tf.train.import_meta_graph('models/model.ckpt.meta')
    #sess = tf.Session()
    ## To initialize values with saved data
    #saver.restore(sess, './models/model.ckpt')
    ## Restrieve useful variables
    #trainInfo = tf.get_collection('TrainInfo')
    #x = trainInfo[0]
    #y_train = trainInfo[1]

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
  inputVariables, jetVariables = StandardVariables(options.variables)
  trainingOptions.netOp.inputVariables   = inputVariables
  trainingOptions.netOp.jetVariables     = jetVariables

  trainingOptions.netOp.jetVariablesList = [jet+var for var in jetVariables for jet in ["j1_","j2_","j3_"]]

  trainingOptions.netOp.numPassThru      = len(inputVariables)
  trainingOptions.netOp.vNames           = trainingOptions.netOp.inputVariables+trainingOptions.netOp.jetVariablesList

dg = DataGetter.DefinedVariables(trainingOptions.netOp.vNames)
varsname = dg.getList()

#dataTTbarAll = pd.read_pickle("trainingTuple_division_1_TTbarSingleLep_validation_2bseed.pkl.gz")
#dataTTbarAll = pd.read_pickle("trainingTuple_division_1_TTbarSingleLep_validation_TeamASel.pkl.gz")
#dataTTbarAll = pd.read_pickle("trainingTuple_division_1_TTbarSingleLep_validation_jpt20_nocone.pkl.gz")
#dataTTbarAll = pd.read_pickle("trainingTuple_division_1_TTbarSingleLep_validation_100k.pkl.gz")

if options.dataFilePath != None: trainingOptions.runOp.dataPath = options.dataFilePath

if options.sklrf:
    dataTTbarName = trainingOptions.runOp.dataPath + "/trainingTuple_division_1_TTbarSingleLep_validation_100K_0.h5"
elif options.xgboost:
    dataTTbarName = trainingOptions.runOp.dataPath + "/trainingTuple_division_1_TTbarSingleLep_validation.pkl.gz"
else:
    dataTTbarName = trainingOptions.runOp.dataPath + "/trainingTuple_TTbarSingleLepT_0_division_2_TTbarSingleLepT_test_0.h5"


def getData(dataName):
    if ".pkl" in dataName:
        dataAll = pd.read_pickle(dataName)
    elif ".h5" in dataName:
        import h5py
        f = h5py.File(dataName, "r")
        npData = f["reco_candidates"][:]
        columnHeaders = f["reco_candidates"].attrs["column_headers"]
        indices = [npData[:,0].astype(numpy.int), npData[:,1].astype(numpy.int)]
        dataAll = pd.DataFrame(npData[:,2:], index=pd.MultiIndex.from_arrays(indices), columns=columnHeaders[2:])
    
        npDataGen = f["gen_tops"][:]
        columnHeadersGen = f["gen_tops"].attrs["column_headers"]
        indicesGen = [npDataGen[:,0].astype(numpy.int), npDataGen[:,1].astype(numpy.int)]
        dataGen = pd.DataFrame(npDataGen[:,2:], index=pd.MultiIndex.from_arrays(indicesGen), columns=columnHeadersGen[2:])
    
        f.close()
    
    return dataAll, dataGen
            
dataTTbarAll, dataTTbarGen = getData(dataTTbarName)

#dataTTbarGen = pd.read_pickle("trainingTuple_division_1_TTbarSingleLep_validation_100k_gen.pkl.gz")
#dataTTbarGen = pd.read_pickle("trainingTuple_division_1_TTbarSingleLep_validation_gen.pkl.gz")

#print list(dataTTbar.columns.values)

#Apply baseline cuts
dataTTbarAll = dataTTbarAll[dataTTbarAll.Njet >= 4]
dataTTbar = dataTTbarAll[dataTTbarAll.ncand > 0]

dataTTbarGen = dataTTbarGen[dataTTbarGen.Njet >= 4]

#Training data for overfitting check
if options.sklrf:
    dataTTbarNameTrain = trainingOptions.runOp.dataPath + "/trainingTuple_division_0_TTbarSingleLep_training_1M_0.h5"
elif options.xgboost:
    dataTTbarNameTrain = trainingOptions.runOp.dataPath + "/trainingTuple_division_0_TTbarSingleLep_training.pkl.gz"
else:
    dataTTbarNameTrain = trainingOptions.runOp.dataPath + "/trainingTuple_TTbarSingleLepT_0_division_0_TTbarSingleLepT_training_0.h5"

dataTTbarAllTrain, _ = getData(dataTTbarNameTrain)

#Apply baseline cuts
dataTTbarAllTrain = dataTTbarAllTrain[dataTTbarAllTrain.Njet >= 4]
dataTTbarTrain = dataTTbarAllTrain[dataTTbarAllTrain.ncand > 0]

print "CALCULATING TTBAR DISCRIMINATORS"

if options.sklrf:
    dataTTbarAns = clf1.predict_proba(dataTTbar.as_matrix(varsname))[:,1]
elif options.xgboost:
    xgData = xgb.DMatrix(dataTTbar.as_matrix(varsname))
    dataTTbarAns = bst.predict(xgData)
else:
    dataTTbarAns = sess.run(y_train, feed_dict={x: dataTTbar.as_matrix(varsname)})[:,0]
    dataTTbarAnsTrain = sess.run(y_train, feed_dict={x: dataTTbarTrain.as_matrix(varsname)})[:,0]

print "CREATING HISTOGRAMS"

#Discriminator plot

inputLabels = dataTTbar.as_matrix(["genConstiuentMatchesVec", "genTopMatchesVec"])
genMatches = (inputLabels[:,0] == 3) & (inputLabels[:,1] == 1)

inputLabelsTrain = dataTTbarTrain.as_matrix(["genConstiuentMatchesVec", "genTopMatchesVec"])
genMatchesTrain = (inputLabelsTrain[:,0] == 3) & (inputLabelsTrain[:,1] == 1)

plt.clf()
plt.hist(dataTTbarAns[genMatches == 1], weights=dataTTbar["sampleWgt"][genMatches == 1], bins=50, normed=True, label="Gen Matched",     fill=False, histtype='step', edgecolor="red")
plt.hist(dataTTbarAns[genMatches != 1], weights=dataTTbar["sampleWgt"][genMatches != 1], bins=50, normed=True, label="Not gen matched", fill=False, histtype='step', edgecolor="blue")
plt.legend(loc='upper right')
plt.xlabel("Discriminator")
plt.ylabel("Normalized events")
plt.savefig(outputDirectory + "discriminator.png")
plt.close()

plt.clf()
plt.hist(dataTTbarAns[genMatches == 1], weights=dataTTbar["sampleWgt"][genMatches == 1], bins=50, normed=True, label="Gen Matched Validation",     fill=False, histtype='step', edgecolor="red")
plt.hist(dataTTbarAns[genMatches != 1], weights=dataTTbar["sampleWgt"][genMatches != 1], bins=50, normed=True, label="Not gen matched Validation", fill=False, histtype='step', edgecolor="blue")
plt.hist(dataTTbarAnsTrain[genMatchesTrain == 1], weights=dataTTbarTrain["sampleWgt"][genMatchesTrain == 1], bins=50, normed=True, label="Gen Matched Train",     fill=False, histtype='step', linestyle="dotted", linewidth=2, edgecolor="red")
plt.hist(dataTTbarAnsTrain[genMatchesTrain != 1], weights=dataTTbarTrain["sampleWgt"][genMatchesTrain != 1], bins=50, normed=True, label="Not gen matched Train", fill=False, histtype='step', linestyle="dotted", linewidth=2, edgecolor="blue")
plt.legend(loc='upper right')
plt.xlabel("Discriminator")
plt.ylabel("Normalized events")
plt.savefig(outputDirectory + "discriminator_validVsTrain.png")
plt.close()

#plot efficiency

effPtBins = numpy.hstack([[0], numpy.linspace(50, 200, 7), numpy.linspace(250, 700, 10), [800, 1000]])
ptNum, _ = numpy.histogram(dataTTbar[genMatches == 1][dataTTbarAns[genMatches == 1] > discCut]["genConstMatchGenPtVec"], bins=effPtBins, weights=dataTTbar[genMatches == 1][dataTTbarAns[genMatches == 1] > discCut]["sampleWgt"])
ptDen, _ = numpy.histogram(dataTTbarGen["genTopPt"], bins=effPtBins, weights=dataTTbarGen["sampleWgt"])

effPt = ptNum/ptDen

plt.clf()
plt.hist(effPtBins[:-1], bins=effPtBins, weights=effPt, fill=False, histtype='step')
#plt.legend(loc='upper right')
plt.xlabel("Candidate Pt [GeV]")
plt.ylabel("Efficiency")
plt.savefig(outputDirectory + "efficiency.png")
plt.close()

effPtBins = numpy.hstack([[0], numpy.linspace(50, 200, 7), numpy.linspace(250, 500, 6), [600, 700, 800, 1000]])
ptNum, _ = numpy.histogram(dataTTbar[genMatches == 1][dataTTbarAns[genMatches == 1] > discCut]["genConstMatchGenPtVec"], bins=effPtBins, weights=dataTTbar[genMatches == 1][dataTTbarAns[genMatches == 1] > discCut]["sampleWgt"])
ptDen, _ = numpy.histogram(dataTTbarGen["genTopPt"], bins=effPtBins, weights=dataTTbarGen["sampleWgt"])

effPt = ptNum/ptDen

plt.clf()
plt.hist(effPtBins[:-1], bins=effPtBins, weights=effPt, fill=False, histtype='step')
#plt.legend(loc='upper right')
plt.xlabel("Candidate Pt [GeV]")
plt.ylabel("Efficiency")
plt.savefig(outputDirectory + "efficiency.png")
plt.close()

#input variable histograms

genTopData = dataTTbar[genMatches == 1]
genBGData = dataTTbar[genMatches != 1]
recoTopData = dataTTbar[dataTTbarAns > discCut]
recoBGData = dataTTbar[dataTTbarAns < discCut]
minTTbar = dataTTbar.min()
maxTTbar = dataTTbar.max()

for var in varsname:
    plt.clf()
    bins = numpy.linspace(minTTbar[var], maxTTbar[var], 21)
    ax = recoTopData .hist(column=var, weights=recoTopData["sampleWgt"], bins=bins, grid=False, normed=True, fill=False, histtype='step',                     label="reco top")
    recoBGData       .hist(column=var, weights=recoBGData["sampleWgt"],  bins=bins, grid=False, normed=True, fill=False, histtype='step',                     label="reco bg", ax=ax)
    genTopData       .hist(column=var, weights=genTopData["sampleWgt"],  bins=bins, grid=False, normed=True, fill=False, histtype='step', linestyle="dotted", label="gen top", ax=ax)
    genBGData        .hist(column=var, weights=genBGData["sampleWgt"],   bins=bins, grid=False, normed=True, fill=False, histtype='step', linestyle="dotted", label="gen bkg", ax=ax)
    plt.legend()
    plt.xlabel(var)
    plt.ylabel("Normalized events")
    plt.yscale('log')
    plt.savefig(outputDirectory + var + ".png")
    plt.close()


#purity plots
#dataTTbar.genConstMatchGenPtVec

plt.clf()

ptBins = numpy.hstack([[0], numpy.linspace(50, 200, 7), numpy.linspace(250, 500, 6), [600, 700, 800, 1000]])
purityNum, _ = numpy.histogram(dataTTbar["cand_pt"][dataTTbarAns > discCut][genMatches[dataTTbarAns > discCut] == 1], bins=ptBins, weights=dataTTbar["sampleWgt"][dataTTbarAns > discCut][genMatches[dataTTbarAns > discCut] == 1])
purityDen,_  = numpy.histogram(dataTTbar["cand_pt"][dataTTbarAns > discCut],                  bins=ptBins, weights=dataTTbar["sampleWgt"][dataTTbarAns > discCut])

purity = purityNum/purityDen

plt.hist(ptBins[:-1], bins=ptBins, weights=purity, fill=False, histtype='step')
#plt.legend(loc='upper right')
plt.xlabel("pT [GeV]")
plt.ylabel("Purity")
plt.savefig(outputDirectory + "purity.png")
plt.close()

plt.clf()

discBins = numpy.linspace(0, 1, 21)
purityDiscNum, _ = numpy.histogram(dataTTbarAns[genMatches == 1], bins=discBins, weights=dataTTbar["sampleWgt"][genMatches == 1])
purityDiscDen,_  = numpy.histogram(dataTTbarAns,                  bins=discBins, weights=dataTTbar["sampleWgt"])

purityDisc = purityDiscNum/purityDiscDen

plt.hist(discBins[:-1], bins=discBins, weights=purityDisc, fill=False, histtype='step')
#plt.legend(loc='upper right')
plt.xlabel("Discriminator")
plt.ylabel("Purity")
plt.savefig(outputDirectory + "purity_disc.png")
plt.close()

print "PROCESSING ZNUNU VALIDATION DATA"

if options.sklrf:
    dataZnunuName = trainingOptions.runOp.dataPath + "/trainingTuple_division_1_ZJetsToNuNu_validation_700K_0.h5"
elif options.xgboost:
    dataZnunuName = trainingOptions.runOp.dataPath + "/trainingTuple_division_1_ZJetsToNuNu_validation_700K_0.h5"
else:
    dataZnunuName = trainingOptions.runOp.dataPath + "/trainingTuple_ZJetsToNuNu_HT_100to200_0_division_2_ZJetsToNuNu_HT_100to200_test_0.h5"

dataZnunuAll, _ = getData(dataZnunuName)

dataZnunuAll = dataZnunuAll[dataZnunuAll.Njet >= 4]
dataZnunu = dataZnunuAll[dataZnunuAll.ncand > 0]

print "CALCULATING ZNUNU DISCRIMINATORS"

if options.sklrf:
    dataZnunuAns = clf1.predict_proba(dataZnunu.as_matrix(varsname))[:,1]
elif options.xgboost:
    xgData = xgb.DMatrix(dataZnunu.as_matrix(varsname))
    dataZnunuAns = bst.predict(xgData)
else:
    dataZnunuAns = sess.run(y_train, feed_dict={x: dataZnunu.as_matrix(varsname)})[:,0]

#calculate fake rate 

plt.clf()

metBins = numpy.linspace(0, 1000, 21)
frMETNum, _ = numpy.histogram(dataZnunu[dataZnunuAns > discCut]["MET"].ix[:,0], bins=metBins, weights=dataZnunu[dataZnunuAns > discCut]["sampleWgt"].ix[:,0])
frMETDen,_  = numpy.histogram(dataZnunuAll["MET"].ix[:,0],                      bins=metBins, weights=dataZnunuAll["sampleWgt"].ix[:,0])

frMETNum[frMETDen < 1e-10] = 0.0
frMETDen[frMETDen < 1e-10] = 1.0
frMET = frMETNum/frMETDen

plt.hist(metBins[:-1], bins=metBins, weights=frMET, fill=False, histtype='step')
#plt.legend(loc='upper right')
plt.xlabel("MET [GeV]")
plt.ylabel("Fake rate")
plt.savefig(outputDirectory + "fakerate_met.png")
plt.close()

plt.clf()

njBins = numpy.linspace(0, 20, 21)
frNjNum, _ = numpy.histogram(dataZnunu[dataZnunuAns > discCut]["Njet"].ix[:,0], bins=njBins, weights=dataZnunu[dataZnunuAns > discCut]["sampleWgt"].ix[:,0])
frNjDen,_  = numpy.histogram(dataZnunuAll["Njet"].ix[:,0],                      bins=njBins, weights=dataZnunuAll["sampleWgt"].ix[:,0])

frNjNum[frNjDen < 1e-10] = 0.0
frNjDen[frNjDen < 1e-10] = 1.0
frNj = frNjNum/frNjDen

plt.hist(njBins[:-1], bins=njBins, weights=frNj, fill=False, histtype='step')
#plt.legend(loc='upper right')
plt.xlabel("N jets")
plt.ylabel("Fake rate")
plt.savefig(outputDirectory + "fakerate_njets.png")
plt.close()

plt.clf()

frCandPtNum, _ = numpy.histogram(dataZnunu[dataZnunuAns > discCut]["cand_pt"].ix[:,0], bins=ptBins, weights=dataZnunu[dataZnunuAns > discCut]["sampleWgt"].ix[:,0])
frCandPtDen, _ = numpy.histogram(dataZnunu["cand_pt"].ix[:,0],                         bins=ptBins, weights=dataZnunu["sampleWgt"].ix[:,0])

frCandPtNum[frCandPtDen < 1e-10] = 0.0
frCandPtDen[frCandPtDen < 1e-10] = 1.0
frCandPt = frCandPtNum/frCandPtDen

plt.hist(ptBins[:-1], bins=ptBins, weights=frCandPt, fill=False, histtype='step')
#plt.legend(loc='upper right')
plt.xlabel("Candidate pt")
plt.ylabel("Fake rate")
plt.savefig(outputDirectory + "fakerate_cand_pt.png")
plt.close()

print "CALCULATING ROC CURVES"

cuts = numpy.hstack([numpy.arange(0.0, 0.05, 0.005), numpy.arange(0.05, 0.95, 0.01), numpy.arange(0.95, 1.00, 0.005)])

FPR = []
TPR = []
FPRZ = []

FPRPtCut = []
TPRPtCut = []
FPRZPtCut = []

evtwgt = dataTTbar["sampleWgt"]
evtwgtZnunu = dataZnunu["sampleWgt"]

NevtTPR = evtwgt[dataTTbar.genConstiuentMatchesVec==3].sum()
NevtFPR = evtwgt[dataTTbar.genConstiuentMatchesVec!=3].sum()
NevtZ = evtwgtZnunu.sum()

ptCut = 200

candPtTTbar = dataTTbar["cand_pt"]
cantPtZnunu = dataZnunu["cand_pt"]

NevtTPRPtCut = evtwgt[(dataTTbar.genConstiuentMatchesVec==3) & (candPtTTbar>ptCut)].sum()
NevtFPRPtCut = evtwgt[(dataTTbar.genConstiuentMatchesVec!=3) & (candPtTTbar>ptCut)].sum()
NevtZPtCut = evtwgtZnunu[cantPtZnunu > ptCut].sum()

dataTTbarAnsRoc = (dataTTbarAns - dataTTbarAns.min()) / dataTTbarAns.ptp()
dataZnunuAnsRoc = (dataZnunuAns - dataZnunuAns.min()) / dataZnunuAns.ptp()

for cut in cuts:
    FPR.append(  evtwgt[(dataTTbarAnsRoc > cut) & (dataTTbar.genConstiuentMatchesVec!=3)].sum() / NevtFPR    )
    TPR.append(  evtwgt[(dataTTbarAnsRoc > cut) & (dataTTbar.genConstiuentMatchesVec==3)].sum() / NevtTPR    )
    FPRZ.append( evtwgtZnunu[(dataZnunuAnsRoc > cut)].sum() / NevtZ )

    FPRPtCut.append(  evtwgt[(dataTTbarAnsRoc > cut) & (dataTTbar.genConstiuentMatchesVec!=3) & (candPtTTbar > ptCut)].sum() / NevtFPRPtCut    )
    TPRPtCut.append(  evtwgt[(dataTTbarAnsRoc > cut) & (dataTTbar.genConstiuentMatchesVec==3) & (candPtTTbar > ptCut)].sum() / NevtTPRPtCut    )
    FPRZPtCut.append( evtwgtZnunu[(dataZnunuAnsRoc > cut) & (cantPtZnunu > ptCut)].sum() / NevtZPtCut )

#prepare tuples
    rocTTbar = list(zip(FPR,TPR))
    rocZnunu = list(zip(FPRZ,TPR))

#Dump roc to file
fileObject = open(outputDirectory + "roc.pkl",'wb')
pickle.dump(TPR, fileObject)
pickle.dump(FPR, fileObject)
pickle.dump(FPRZ, fileObject)
pickle.dump(TPRPtCut, fileObject)
pickle.dump(FPRPtCut, fileObject)
pickle.dump(FPRZPtCut, fileObject)
fileObject.close()

plt.clf()
plt.plot(FPR,TPR)
plt.xlabel("FPR (ttbar)")
plt.ylabel("TPR (ttbar)")
plt.savefig(outputDirectory + "roc.png")
plt.close()

plt.clf()
plt.plot(FPRZ,TPR)
plt.xlabel("FPR (Znunu)")
plt.ylabel("TPR (ttbar)")
plt.savefig(outputDirectory + "rocZ.png")
plt.close()



print "VALIDATION DONE!"
