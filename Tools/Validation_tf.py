import sys
import os
import errno
import pandas as pd
import numpy as np
import math
from MVAcommon_tf import *
import optparse
import matplotlib.pyplot as plt
import pickle

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
parser.add_option ('-c', "--disc", dest='discCut', action='store', default=0.6, help="Discriminator cut")
parser.add_option ('-k', "--sklrf", dest='sklrf', action='store_true', help="Use skl random forest instead of tensorflow")
parser.add_option ('-x', "--xgboost", dest='xgboost', action='store_true', help="Run using xgboost")
parser.add_option ('-f', "--mvaFile", dest='mvaFile', action='store', default="", help="Mva training file")
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

    ##TODO: Switchen to frozen model file 
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
    y_train = graph.get_tensor_by_name('y:0')



print "PROCESSING TTBAR VALIDATION DATA"

varsname = DataGetter().getList()

#dataTTbarAll = pd.read_pickle("trainingTuple_division_1_TTbarSingleLep_validation_2bseed.pkl.gz")
#dataTTbarAll = pd.read_pickle("trainingTuple_division_1_TTbarSingleLep_validation_TeamASel.pkl.gz")
#dataTTbarAll = pd.read_pickle("trainingTuple_division_1_TTbarSingleLep_validation_jpt20_nocone.pkl.gz")
#dataTTbarAll = pd.read_pickle("trainingTuple_division_1_TTbarSingleLep_validation_100k.pkl.gz")

if options.sklrf:
    dataTTbarAll = pd.read_pickle("trainingTuple_division_1_TTbarSingleLep_validation_100k.pkl.gz")
elif options.xgboost:
    dataTTbarAll = pd.read_pickle("trainingTuple_division_1_TTbarSingleLep_validation.pkl.gz")
else:
    dataTTbarAll = pd.read_pickle("trainingTuple_division_1_TTbarSingleLep_validation_100k.pkl.gz")

numDataTTbar = dataTTbarAll._get_numeric_data()
numDataTTbar[numDataTTbar < 0.0] = 0.0

#dataTTbarGen = pd.read_pickle("trainingTuple_division_1_TTbarSingleLep_validation_100k_gen.pkl.gz")
dataTTbarGen = pd.read_pickle("trainingTuple_division_1_TTbarSingleLep_validation_gen.pkl.gz")

#print list(dataTTbar.columns.values)

#Apply baseline cuts
dataTTbarAll = dataTTbarAll[dataTTbarAll.Njet >= 4]
dataTTbar = dataTTbarAll[dataTTbarAll.ncand > 0]

dataTTbarGen = dataTTbarGen[dataTTbarGen.Njet >= 4]

print "CALCULATING TTBAR DISCRIMINATORS"

if options.sklrf:
    dataTTbarAns = clf1.predict_proba(dataTTbar.as_matrix(varsname))[:,1]
elif options.xgboost:
    xgData = xgb.DMatrix(dataTTbar.as_matrix(varsname))
    dataTTbarAns = bst.predict(xgData)
else:
    dataTTbarAns = sess.run(y_train, feed_dict={x: dataTTbar.as_matrix(varsname)})[:,0]

print "CREATING HISTOGRAMS"

#Discriminator plot

inputLabels = dataTTbar.as_matrix(["genConstiuentMatchesVec", "genTopMatchesVec"])
genMatches = (inputLabels[:,0] == 3) & (inputLabels[:,1] == 1)

plt.clf()
plt.hist(dataTTbarAns[genMatches == 1], weights=dataTTbar["sampleWgt"][genMatches == 1], bins=50, normed=True, label="Gen Matched",     fill=False, histtype='step', edgecolor="red")
plt.hist(dataTTbarAns[genMatches != 1], weights=dataTTbar["sampleWgt"][genMatches != 1], bins=50, normed=True, label="Not gen matched", fill=False, histtype='step', edgecolor="blue")
plt.legend(loc='upper right')
plt.xlabel("Discriminator")
plt.ylabel("Normalized events")
plt.savefig(outputDirectory + "discriminator.png")
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
    dataZnunuAll = pd.read_pickle("trainingTuple_division_1_ZJetsToNuNu_validation_700k.pkl.gz")
elif options.xgboost:
    dataZnunuAll = pd.read_pickle("trainingTuple_division_1_ZJetsToNuNu_validation_700k.pkl.gz")
else:
    dataZnunuAll = pd.read_pickle("trainingTuple_division_1_ZJetsToNuNu_validation_700k.pkl.gz")

#dataZnunuAll = pd.read_pickle("trainingTuple_division_1_ZJetsToNuNu_validation_2bseed.pkl.gz")
numDataZnunu = dataZnunuAll._get_numeric_data()
numDataZnunu[numDataZnunu < 0.0] = 0.0

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

print "CALCULATING ROC CURVES"

cuts = np.hstack([np.arange(0.0, 0.05, 0.005), np.arange(0.05, 0.95, 0.01), np.arange(0.95, 1.00, 0.005)])

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
