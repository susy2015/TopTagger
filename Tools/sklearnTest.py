import ROOT
import numpy
from sklearn.ensemble import RandomForestClassifier
from sklearn.ensemble import RandomForestRegressor
import sklearn.tree as tree
from sklearn.tree import DecisionTreeClassifier
from sklearn.tree import DecisionTreeRegressor

def getData(event, i):
    return [event.cand_pt[i], event.cand_dRMax[i], event.j12_m[i]/event.cand_m[i], event.j13_m[i]/event.cand_m[i], event.j23_m[i]/event.cand_m[i], event.dR12[i], event.dR23[i], event.dR13[i], event.j1_pt[i], event.j2_pt[i], event.j3_pt[i], event.j1_CSV[i], event.j2_CSV[i], event.j3_CSV[i]]

file = ROOT.TFile.Open("trainingTuple_division_0.root")

# allowedVarsD_ = {"cand_pt", "cand_eta", "cand_phi", "cand_m", "cand_dRMax", "j1_pt", "j1_eta", "j1_phi", "j1_m", "j1_CSV", "j2_pt", "j2_eta", "j2_phi", "j2_m", "j2_CSV", "j3_pt", "j3_eta", "j3_phi", "j3_m",  "j3_CSV", "dR12", "d\
#Eta12", "dPhi12", "dR13", "dEta13", "dPhi13", "dR23", "dEta23", "dPhi23"};
#        allowedVarsI_ = {"genTopMatchesVec", "genConstiuentMatchesVec"};

print "PROCESSING TRAINING DATA"

NBg = 0.0
NSig = 0.0
for event in file.TTbarSingleLep:
    for i in xrange(len(event.genConstiuentMatchesVec)):
        if event.genConstiuentMatchesVec[i] == 3:
            NSig += 1.0
        else:
            NBg += 1.0

inputData = []
inputAnswer = []
inputWgts = []
for event in file.TTbarSingleLep:
    for i in xrange(len(event.cand_m)):
        inputData.append(getData(event, i))
        nmatch = event.genConstiuentMatchesVec[i]
        inputAnswer.append((nmatch == 3))
        if nmatch == 3:
            inputWgts.append((NSig+NBg)/NSig)
        else:
            inputWgts.append((NSig+NBg)/NBg)
    
npyInputData = numpy.array(inputData, numpy.float32)
npyInputAnswer = numpy.array(inputAnswer, numpy.float32)
npyInputWgts = numpy.array(inputWgts, numpy.float32)

print "TRAINING MVA"

#clf = RandomForestClassifier(n_estimators=10)
clf = RandomForestRegressor(n_estimators=10)
#clf = DecisionTreeRegressor()
#clf = DecisionTreeClassifier()
clf = clf.fit(npyInputData, npyInputAnswer, npyInputWgts)

fileValidation = ROOT.TFile.Open("trainingTuple_division_1.root")

print "PROCESSING VALIDATION DATA"

inputList = []
truth = []
passPt = []
for event in fileValidation.TTbarSingleLep:
    for i in xrange(len(event.cand_m)):
        inputList.append(getData(event, i))
        truth.append(event.genConstiuentMatchesVec[i] == 3)
        passPt.append(event.cand_pt[i] > 250)

print "CALCULATING DESCRIMINATORS"

output = clf.predict(inputList)

print "FILLING HISTOGRAMS"


hDisc = ROOT.TH1D("disc", "disc", 11, -0.05, 1.05)
hDiscMatch = ROOT.TH1D("discMatch", "discMatch", 11, -0.05, 1.05)
hDiscMatch.SetLineColor(ROOT.kRed)
hDiscNoMatch = ROOT.TH1D("discNoMatch", "discNoMatch", 11, -0.05, 1.05)
hDiscNoMatch.SetLineColor(ROOT.kBlue)
hDiscMatchPt = ROOT.TH1D("discMatchPt", "discMatchPt", 11, -0.05, 1.05)
hDiscMatchPt.SetLineColor(ROOT.kRed+2)
hDiscMatchPt.SetLineStyle(ROOT.kDotted)
hDiscNoMatchPt = ROOT.TH1D("discNoMatchPt", "discNoMatchPt", 11, -0.05, 1.05)
hDiscNoMatchPt.SetLineColor(ROOT.kBlue+2)
hDiscNoMatchPt.SetLineStyle(ROOT.kDotted)

for i in xrange(len(output)):
    hDisc.Fill(output[i])
    if(truth[i]):
        hDiscMatch.Fill(output[i])
        if(passPt[i]):
            hDiscMatchPt.Fill(output[i])
    else:
        hDiscNoMatch.Fill(output[i])
        if(passPt[i]):
            hDiscNoMatchPt.Fill(output[i])

print "PLOTTING"

c = ROOT.TCanvas("c1","c1",800,800)

leg = ROOT.TLegend(0.55, 0.75, 0.9, 0.9)
leg.AddEntry(hDiscMatch, "Constituent Matched")
leg.AddEntry(hDiscNoMatch, "Not Constituent Matched")

hDiscMatch.SetStats(0)

hDiscMatch.Scale(1/hDiscMatch.Integral())
hDiscNoMatch.Scale(1/hDiscNoMatch.Integral())
hDiscMatchPt.Scale(1/hDiscMatchPt.Integral())
hDiscNoMatchPt.Scale(1/hDiscNoMatchPt.Integral())

#hDisc.Draw()
hDiscMatch.Draw()
hDiscNoMatch.Draw("same")
hDiscMatchPt.Draw("same")
hDiscNoMatchPt.Draw("same")
leg.Draw("same")
hDiscMatch.GetYaxis().SetRangeUser(0, 0.6)
c.Print("discriminator.png")
hDiscMatch.GetYaxis().SetRangeUser(0.001, 5)
c.SetLogy()
c.Print("discriminator_log.png")

#with open("iris.dot", 'w') as f:
#    f = tree.export_graphviz(clf, out_file=f,
#                             feature_names = ["cand_m", "cand_pt", "cand_dRMax"],
#                             class_names = ["background", "top"],
#                             filled=True, rounded=True)

print "DONE!"
