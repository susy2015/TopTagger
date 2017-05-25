import sys
import pandas as pd
import numpy as np
import math
from MVAcommon_tf import *
import optparse
import matplotlib.pyplot as plt

parser = optparse.OptionParser("usage: %prog [options]\n")

#parser.add_option ('-o', "--opencv", dest='opencv', action='store_true', help="Run using opencv RTrees")
#parser.add_option ('-n', "--noRoc", dest='noROC', action='store_true', help="Do not calculate ROC to save time")
parser.add_option ('-d', "--disc", dest='discCut', action='store', default=0.6, help="Discriminator cut")
parser.add_option ('-k', "--sklrf", dest='sklrf', action='store_true', help="Use skl random forest instead of tensorflow")

options, args = parser.parse_args()

#disc cut
discCut = options.discCut

print "RETRIEVING MODEL FILE"

if options.sklrf:
    from sklearn.ensemble import RandomForestClassifier
    import pickle
    
    fileTraining = open("TrainingOutput.pkl",'r')
    clf1 = pickle.load(fileTraining)
    fileTraining.close()

else:
    import tensorflow as tf

    #Get training output
    saver = tf.train.import_meta_graph('models/model.ckpt.meta')
    sess = tf.Session()
    # To initialize values with saved data
    saver.restore(sess, './models/model.ckpt')
    # Restrieve useful variables
    trainInfo = tf.get_collection('TrainInfo')
    x = trainInfo[0]
    y_train = trainInfo[1]


print "PROCESSING TTBAR VALIDATION DATA"

varsname = DataGetter().getList()

dataTTbar = pd.read_pickle("trainingTuple_division_1_TTbarSingleLep_validation.pkl")
numDataTTbar = dataTTbar._get_numeric_data()
numDataTTbar[numDataTTbar < 0.0] = 0.0

#Apply baseline cuts
dataTTbar = dataTTbar[dataTTbar.Njet >= 4]

print "CALCULATING TTBAR DISCRIMINATORS"

if options.sklrf:
    dataTTbarAns = clf1.predict_proba(dataTTbar.as_matrix(varsname))[:,1]
else:
    dataTTbarAns = sess.run(y_train, feed_dict={x: dataTTbar.as_matrix(varsname)})[:,0]

print "CREATING HISTOGRAMS"

#Discriminator plot

inputLabels = dataTTbar.as_matrix(["genConstiuentMatchesVec", "genTopMatchesVec"])
genMatches = (inputLabels[:,0] == 3) & inputLabels[:,1]

plt.clf()
plt.figure()
plt.hist(dataTTbarAns[genMatches == 1], weights=dataTTbar["sampleWgt"][genMatches == 1], bins=50, normed=True, label="Gen Matched",     fill=False, histtype='step', edgecolor="red")
plt.hist(dataTTbarAns[genMatches != 1], weights=dataTTbar["sampleWgt"][genMatches != 1], bins=50, normed=True, label="Not gen matched", fill=False, histtype='step', edgecolor="blue")
plt.legend(loc='upper right')
plt.xlabel("Discriminator")
plt.ylabel("Normalized events")
plt.savefig("discriminator.png")
plt.close()

#plot efficiency

#ptNum, ptNumBins = numpy.histogram(dataTTbar[]["cand_pt"], bins=numpy.hstack([[0], numpy.linspace(50, 400, 36), numpy.linspace(450, 700, 6), [800, 1000]]), weights=npyInputSampleWgts[:,0])

#input variable histograms

genTopData = dataTTbar[genMatches == 1]
genBGData = dataTTbar[genMatches != 1]
recoTopData = dataTTbar[dataTTbarAns > discCut]
recoBGData = dataTTbar[dataTTbarAns < discCut]

for var in varsname:
    plt.clf()
    plt.figure()
    ax = recoTopData .hist(column=var, weights=recoTopData["sampleWgt"], bins=20, grid=False, normed=True, fill=False, histtype='step',                     label="reco top")
    recoBGData       .hist(column=var, weights=recoBGData["sampleWgt"],  bins=20, grid=False, normed=True, fill=False, histtype='step',                     label="reco bg", ax=ax)
    genTopData       .hist(column=var, weights=genTopData["sampleWgt"],  bins=20, grid=False, normed=True, fill=False, histtype='step', linestyle="dotted", label="gen top", ax=ax)
    genBGData        .hist(column=var, weights=genBGData["sampleWgt"],   bins=20, grid=False, normed=True, fill=False, histtype='step', linestyle="dotted", label="gen bkg", ax=ax)
    plt.legend()
    plt.xlabel(var)
    plt.ylabel("Normalized events")
    plt.savefig(var + ".png")
    plt.close()

print "PROCESSING ZNUNU VALIDATION DATA"

dataZnunu = pd.read_pickle("trainingTuple_division_1_ZJetsToNuNu_validation.pkl")
numDataZnunu = dataZnunu._get_numeric_data()
numDataZnunu[numDataZnunu < 0.0] = 0.0

print "CALCULATING ZNUNU DISCRIMINATORS"

if options.sklrf:
    dataZnunuAns = clf1.predict_proba(dataZnunu.as_matrix(varsname))[:,1]
else:
    dataZnunuAns = sess.run(y_train, feed_dict={x: dataZnunu.as_matrix(varsname)})[:,0]

print "CALCULATING ROC CURVES"

cuts = np.hstack([np.arange(0.0, 0.05, 0.005), np.arange(0.05, 0.95, 0.01), np.arange(0.95, 1.00, 0.005)])

FPR = []
TPR = []
FPRZ = []

evtwgt = dataTTbar["sampleWgt"]
evtwgtZnunu = dataZnunu["sampleWgt"]

NevtTPR = evtwgt[dataTTbar.genConstiuentMatchesVec==3].sum()
NevtFPR = evtwgt[dataTTbar.genConstiuentMatchesVec!=3].sum()
NevtZ = evtwgtZnunu.sum()

for cut in cuts:
    FPR.append(  evtwgt[(dataTTbarAns > cut) & (dataTTbar.genConstiuentMatchesVec!=3)].sum() / NevtFPR    )
    TPR.append(  evtwgt[(dataTTbarAns > cut) & (dataTTbar.genConstiuentMatchesVec==3)].sum() / NevtTPR    )
    FPRZ.append( evtwgtZnunu[(dataZnunuAns > cut) & (dataZnunu.genConstiuentMatchesVec!=3)].sum() / NevtZ )

plt.clf()
plt.figure()
plt.plot(FPR,TPR)
plt.xlabel("FPR (ttbar)")
plt.ylabel("TPR (ttbar)")
plt.savefig("roc.png")
plt.close()

plt.clf()
plt.figure()
plt.plot(FPRZ,TPR)
plt.xlabel("FPR (Znunu)")
plt.ylabel("TPR (ttbar)")
plt.savefig("rocZ.png")
plt.close()

print "VALIDATION DONE!"
