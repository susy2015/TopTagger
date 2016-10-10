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

class DataGetter:

    def __init__(self):
        #self.list = ["cand_m", "cand_dRMax", "cand_pt", "j12_m", "j13_m", "j23_m", "dPhi12", "dPhi23", "dPhi13", "j1_pt", "j2_pt", "j3_pt", "j1_CSV", "j2_CSV", "j3_CSV", "j1_QGL", "j2_QGL", "j3_QGL"]
        self.list = ["cand_m", "j12_m", "j13_m", "j23_m", "dPhi12", "dPhi23", "dPhi13", "j1_pt", "j2_pt", "j3_pt", "j1_eta", "j2_eta", "j3_eta", "j1_CSV", "j2_CSV", "j3_CSV", "j1_QGL", "j2_QGL", "j3_QGL"]
        self.list2 = ["event." + v + "[i]" for v in self.list]
        self.theStrCommand = "[" + ", ".join(self.list2) + "]"

    def getData(self, event, i):
        return eval(self.theStrCommand)

    def getList(self):
        return self.list

#Variable histo declaration  
dg = DataGetter()

print "PROCESSING TRAINING DATA"

trainingfile_ttbar = ROOT.TFile.Open("trainingTuple_division_0_TTbarSingleLep_training.root")
trainingfile_znunu = ROOT.TFile.Open("trainingTuple_division_0_ZJetsToNuNu_training.root")

hPtMatch   = ROOT.TH1D("hPtMatch", "hPtMatch", 50, 0.0, 2000.0)
hPtNoMatch = ROOT.TH1D("hPtNoMatch", "hPtNoMatch", 50, 0.0, 2000.0)
hPtZnunuMatch   = ROOT.TH1D("hPtZnunuMatch", "hPtZnunuMatch", 50, 0.0, 2000.0)
hPtZnunuNoMatch = ROOT.TH1D("hPtZnunuNoMatch", "hPtZnunuNoMatch", 50, 0.0, 2000.0)

NEVTS = 1e10
NEVTS_Z = 1e10

Nevts = 0
for event in trainingfile_ttbar.slimmedTuple:
    if Nevts >= NEVTS:
        break
    Nevts +=1
    for i in xrange(len(event.genConstiuentMatchesVec)):
        if event.genConstiuentMatchesVec[i] == 3:
            hPtMatch.Fill(event.cand_pt[i])
        else:
            hPtNoMatch.Fill(event.cand_pt[i])

Nevts = 0
for event in trainingfile_znunu.slimmedTuple:
    if Nevts >= NEVTS_Z:
        break
    Nevts +=1
    for i in xrange(len(event.genConstiuentMatchesVec)):
        if event.genConstiuentMatchesVec[i] == 3:
            hPtMatch.Fill(event.cand_pt[i])
        else:
            hPtNoMatch.Fill(event.cand_pt[i])

inputData = []
inputAnswer = []
inputWgts = []

Nevts = 0
for event in trainingfile_ttbar.slimmedTuple:
    if Nevts >= NEVTS:
        break
    Nevts +=1
    for i in xrange(len(event.cand_m)):
        #if(event.cand_pt[i] > 150):
            inputData.append(dg.getData(event, i))
            nmatch = event.genConstiuentMatchesVec[i]
            inputAnswer.append(int(nmatch == 3))
            if nmatch == 3:
                if hPtMatch.GetBinContent(hPtMatch.FindBin(event.cand_pt[i])) > 10:
                    inputWgts.append(1.0 / hPtMatch.GetBinContent(hPtMatch.FindBin(event.cand_pt[i])))
                else:
                    inputWgts.append(0.0)
            else:
                if hPtMatch.GetBinContent(hPtMatch.FindBin(event.cand_pt[i])) > 10:
                    inputWgts.append(1.0 / hPtNoMatch.GetBinContent(hPtNoMatch.FindBin(event.cand_pt[i])))
                else:
                    inputWgts.append(0.0)

Nevts = 0
for event in trainingfile_znunu.slimmedTuple:
    if Nevts >= NEVTS_Z:
        break
    Nevts +=1
    for i in xrange(len(event.cand_m)):
        #if(event.cand_pt[i] > 150):
            inputData.append(dg.getData(event, i))
            nmatch = event.genConstiuentMatchesVec[i]
            inputAnswer.append(int(nmatch == 3))
            if nmatch == 3:
                if hPtZnunuMatch.GetBinContent(hPtZnunuMatch.FindBin(event.cand_pt[i])) > 10:
                    inputWgts.append(1.0 / hPtZnunuMatch.GetBinContent(hPtZnunuMatch.FindBin(event.cand_pt[i])))
                else:
                    inputWgts.append(0.0)
            else:
                if hPtZnunuNoMatch.GetBinContent(hPtZnunuNoMatch.FindBin(event.cand_pt[i])) > 10:
                    inputWgts.append(1.0 / hPtZnunuNoMatch.GetBinContent(hPtZnunuNoMatch.FindBin(event.cand_pt[i])))
                else:
                    inputWgts.append(0.0)
    

npyInputData = numpy.array(inputData, numpy.float32)
npyInputAnswer = numpy.array(inputAnswer, numpy.float32)
npyInputWgts = numpy.array(inputWgts, numpy.float32)

print "TRAINING MVA"

#clf = RandomForestClassifier(n_estimators=100, n_jobs = 4)
#clf = RandomForestRegressor(n_estimators=100, n_jobs = 4)
#clf = AdaBoostRegressor(n_estimators=100)
clf = GradientBoostingClassifier(n_estimators=100, learning_rate=0.1, random_state=0)
#clf = GradientBoostingRegressor(n_estimators=100, learning_rate=0.1, random_state=0, loss='ls')
#clf = DecisionTreeRegressor()
#clf = DecisionTreeClassifier()
#clf = svm.SVC()

#randomize input data
perms = numpy.random.permutation(npyInputData.shape[0])
npyInputData = npyInputData[perms]
npyInputAnswer = npyInputAnswer[perms]
npyInputWgts = npyInputWgts[perms]

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
