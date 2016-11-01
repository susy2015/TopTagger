import sys
import ROOT
import numpy
import math
from sklearn.ensemble import RandomForestClassifier
from sklearn.ensemble import RandomForestRegressor
import sklearn.tree as tree
from sklearn.tree import DecisionTreeClassifier
from sklearn.tree import DecisionTreeRegressor
from sklearn import svm
from sklearn.ensemble import AdaBoostRegressor
from sklearn.ensemble import GradientBoostingClassifier
from sklearn.ensemble import GradientBoostingRegressor
import pickle
from MVAcommon import *
try:
    import cv2
except ImportError:
    sys.path.append("../../opencv/lib/")
    import cv2
import optparse

parser = optparse.OptionParser("usage: %prog [options]\n")

parser.add_option ('-o', "--opencv", dest='opencv', action='store_true', help="Run using opencv RTrees")
parser.add_option ('-n', "--noRoc", dest='noROC', action='store_true', help="Do not calculate ROC to save time")

options, args = parser.parse_args()

dg = DataGetter()

print "PROCESSING TRAINING DATA"

samplesToRun = ["trainingTuple_division_0_TTbarSingleLep_training.root", "trainingTuple_division_0_ZJetsToNuNu_training.root", "trainingTuple_division_0_Signal_training.root"]

inputData = []
inputAnswer = []
inputWgts = []

for datasetName in samplesToRun:
    dataset = ROOT.TFile.Open(datasetName)

    hPtMatch   = ROOT.TH1D("hPtMatch" + datasetName, "hPtMatch", 50, 0.0, 2000.0)
    hPtNoMatch = ROOT.TH1D("hPtNoMatch" + datasetName, "hPtNoMatch", 50, 0.0, 2000.0)
    
    Nevts = 0
    for event in dataset.slimmedTuple:
        if Nevts >= NEVTS:
            break
        Nevts +=1
        for i in xrange(len(event.genConstiuentMatchesVec)):
            if event.genConstiuentMatchesVec[i] == 3:
                hPtMatch.Fill(event.cand_pt[i])
            else:
                hPtNoMatch.Fill(event.cand_pt[i])
    
    Nevts = 0
    for event in dataset.slimmedTuple:
        if Nevts >= NEVTS:
            break
        Nevts +=1
        for i in xrange(len(event.cand_m)):
            inputData.append(dg.getData(event, i))
            nmatch = event.genConstiuentMatchesVec[i]
            inputAnswer.append(int(nmatch == 3))
            if nmatch == 3:
                if hPtMatch.GetBinContent(hPtMatch.FindBin(event.cand_pt[i])) > 10:
                    inputWgts.append(1.0 / hPtMatch.GetBinContent(hPtMatch.FindBin(event.cand_pt[i])))
                else:
                    inputWgts.append(0.0)
            else:
                if hPtNoMatch.GetBinContent(hPtNoMatch.FindBin(event.cand_pt[i])) > 10:
                    inputWgts.append(1.0 / hPtNoMatch.GetBinContent(hPtNoMatch.FindBin(event.cand_pt[i])))
                else:
                    inputWgts.append(0.0)

                
npyInputData = numpy.array(inputData, numpy.float32)
npyInputAnswer = numpy.array(inputAnswer, numpy.float32)
npyInputWgts = numpy.array(inputWgts, numpy.float32)

nSig = npyInputWgts[npyInputAnswer==1].sum()
nBg = npyInputWgts[npyInputAnswer==0].sum()

#Equalize the relative weights of signal and bg
for i in xrange(len(npyInputAnswer)):
    if npyInputAnswer[i] == 0:
        npyInputWgts[i] *= nSig/nBg

nSig = npyInputWgts[npyInputAnswer==1].sum()
nBg = npyInputWgts[npyInputAnswer==0].sum()

#randomize input data
perms = numpy.random.permutation(npyInputData.shape[0])
npyInputData = npyInputData[perms]
npyInputAnswer = npyInputAnswer[perms]
npyInputWgts = npyInputWgts[perms]

print "TRAINING MVA"

if options.opencv:
    clf = cv2.ml.RTrees_create()

    n_estimators = 100
    clf.setTermCriteria((cv2.TERM_CRITERIA_COUNT, n_estimators, 0.3)); #Do not make the 3rd arguement smaller, it can crash the code
    clf.setMaxCategories(2);
    clf.setMaxDepth(15);
    clf.setMinSampleCount(5);

    #make opencv TrainData container
    cvTrainData = cv2.ml.TrainData_create(npyInputData, cv2.ml.ROW_SAMPLE, npyInputAnswer, sampleWeights = npyInputWgts)

    clf.train(cvTrainData)

    clf.save("TrainingOutput.model")

else:
    clf = RandomForestClassifier(n_estimators=100, max_depth=10, n_jobs = 4)
    #clf = RandomForestRegressor(n_estimators=100, max_depth=10, n_jobs = 4)
    #clf = AdaBoostRegressor(n_estimators=100)
    #clf = GradientBoostingClassifier(n_estimators=100, max_depth=10, learning_rate=0.1, random_state=0)
    #clf = GradientBoostingRegressor(n_estimators=100, max_depth=10, learning_rate=0.1, random_state=0, loss='ls')
    #clf = DecisionTreeRegressor()
    #clf = DecisionTreeClassifier()
    #clf = svm.SVC()

    clf = clf.fit(npyInputData, npyInputAnswer, npyInputWgts)

    #Dump output from training
    fileObject = open("TrainingOutput.pkl",'wb')
    out = pickle.dump(clf, fileObject)
    fileObject.close()

    # Plot feature importance
    feature_importance = clf.feature_importances_
    feature_names = numpy.array(dg.getList())
    feature_importance = 100.0 * (feature_importance / feature_importance.max())
    sorted_idx = numpy.argsort(feature_importance)
    
    #try to plot it with matplotlib
    try:
        import matplotlib.pyplot as plt
    
        # make importances relative to max importance
        pos = numpy.arange(sorted_idx.shape[0]) + .5
        #plt.subplot(1, 2, 2)
        plt.barh(pos, feature_importance[sorted_idx], align='center')
        plt.yticks(pos, feature_names[sorted_idx])
        plt.xlabel('Relative Importance')
        plt.title('Variable Importance')
        #plt.show()
        plt.savefig("feature_importance.png")
    except ImportError:
        #I guess no matplotlib is installed, just print to screen?
        featureImportanceandNames = zip(feature_names, feature_importance)
        print [featureImportanceandNames[a] for a in sorted_idx].reverse()

print "TRAINING DONE!"
