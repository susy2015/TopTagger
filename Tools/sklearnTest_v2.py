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
import cv2

class DataGetter:

    def __init__(self):
        #self.list = ["cand_m", "cand_dRMax", "cand_pt", "j12_m", "j13_m", "j23_m", "dPhi12", "dPhi23", "dPhi13", "j1_pt", "j2_pt", "j3_pt", "j1_CSV", "j2_CSV", "j3_CSV", "j1_QGL", "j2_QGL", "j3_QGL"]
        self.list = ["cand_m", "j12_m", "j13_m", "j23_m", "dPhi12", "dPhi23", "dPhi13", "j1_p", "j2_p", "j3_p", "j1_theta", "j2_theta", "j3_theta", "j1_CSV", "j2_CSV", "j3_CSV", "j1_QGL", "j2_QGL", "j3_QGL", "j12_dTheta", "j13_dTheta", "j23_dTheta"]
        self.list2 = ["event." + v + "[i]" for v in self.list]
        self.theStrCommand = "[" + ", ".join(self.list2) + "]"

    def getData(self, event, i):
        return eval(self.theStrCommand)

    def getList(self):
        return self.list

#Variable histo declaration  
dg = DataGetter()

histranges = {"cand_m":[20, 50, 300], 
              "cand_dRMax":[50,0,5],
              "cand_pt":[50,0,1000],
#              "j1_m":[100, 0, 100],
              "j23_m":[100, 0, 250],
#              "j23j1_dR":[50,0,5],
              "j12_m":[100, 0, 250],
              "j13_m":[100, 0, 250],
              "dPhi12":[50,0,3.2],
              "dPhi23":[50,0,3.2],
              "dPhi13":[50,0,3.2],
              "j1_p":[50,0,1000],
              "j2_p":[50,0,1000],
              "j3_p":[50,0,1000],
#              "j2_m":[100, 0, 250],
#              "j3_m":[100, 0, 250],
              "j1_CSV":[50, 0, 1],
              "j2_CSV":[50, 0, 1],
              "j3_CSV":[50, 0, 1],
              "j1_theta":[50, 0, 4],
              "j2_theta":[50, 0, 4],
              "j3_theta":[50, 0, 4],
              "j12_dTheta":[50, 0, 4],
              "j13_dTheta":[50, 0, 4],
              "j23_dTheta":[50, 0, 4],
#              "j12j3_dR":[50,0,5],
#              "j13j2_dR":[50,0,5],
              "j1_QGL":[50, 0, 1],
              "j2_QGL":[50, 0, 1],
              "j3_QGL":[50, 0, 1]}
hist_tag = {}
hist_notag = {}

for var in dg.getList():
    if(histranges.has_key(var)) : 
        hist_tag[var] = ROOT.TH1D(var+"_tag", var+"_tag", histranges[var][0], histranges[var][1], histranges[var][2])
        hist_notag[var] = ROOT.TH1D(var+"_notag", var+"_notag", histranges[var][0], histranges[var][1], histranges[var][2])

varsname = dg.getList()
varsmap = {k:[] for k in varsname}

def HEPReqs(event, i):
    Mw = 80.385
    Mt = 173.5
    Rmin_ = 0.85 *(Mw/Mt)
    Rmax_ = 1.25 *(Mw/Mt)
    CSV_ = 0.800

    #HEP tagger requirements
    passHEPRequirments = True

    #Get the total candidate mass
    m123 = event.cand_m[i]

    m12  = event.j12_m[i];
    m23  = event.j23_m[i];
    m13  = event.j13_m[i];

    #Implement HEP mass ratio requirements here
    criterionA = 0.2 < math.atan(m13/m12) and math.atan(m13/m12) < 1.3 and Rmin_ < m23/m123 and m23/m123 < Rmax_

    criterionB = ((Rmin_**2)*(1+(m13/m12)**2) < (1 - (m23/m123)**2)) and ((1 - (m23/m123)**2) < (Rmax_**2)*(1 + (m13/m12)**2))

    criterionC = ((Rmin_**2)*(1+(m12/m13)**2) < (1 - (m23/m123)**2)) and ((1 - (m23/m123)**2) < (Rmax_**2)*(1 + (m12/m13)**2))

    passHEPRequirments = criterionA or criterionB or criterionC;

    passBreq = (int(event.j1_CSV[i] > CSV_) + int(event.j2_CSV[i] > CSV_) + int(event.j3_CSV[i] > CSV_)) <= 1

    return passHEPRequirments and passBreq


class simpleTopCand:
    def __init__(self, event, i, discriminator):
        self.j1 = ROOT.TLorentzVector()
        self.j2 = ROOT.TLorentzVector()
        self.j3 = ROOT.TLorentzVector()
        self.j1.SetPtEtaPhiM(event.j1_p[i], 0, event.j1_theta[i], event.j1_m[i])
        self.j2.SetPtEtaPhiM(event.j2_p[i], 0, event.j2_theta[i], event.j2_m[i])
        self.j3.SetPtEtaPhiM(event.j3_p[i], 0, event.j3_theta[i], event.j3_m[i])
        self.discriminator = discriminator

    def __lt__(self, other):
        return self.discriminator < other.discriminator

def jetInList(jet, jlist):
    for j in jlist:
        if(abs(jet.M() - j.M()) < 0.0001):
            return True
    return False

def resolveOverlap(event, discriminators, threshold):
    topCands = [simpleTopCand(event, i, discriminators[i]) for i in xrange(len(event.j1_p))]
    topCands.sort(reverse=True)

    finalTops = []
    usedJets = []
    for cand in topCands:
        #if not cand.j1 in usedJets and not cand.j2 in usedJets and not cand.j3 in usedJets:
        if not jetInList(cand.j1, usedJets) and not jetInList(cand.j2, usedJets) and not jetInList(cand.j3, usedJets):
            if cand.discriminator > threshold:
                usedJets += [cand.j1, cand.j2, cand.j3]
                finalTops.append(cand)

    return finalTops

class simpleTopCandHEP:
    def __init__(self, event, i, passFail):
        self.j1 = ROOT.TLorentzVector()
        self.j2 = ROOT.TLorentzVector()
        self.j3 = ROOT.TLorentzVector()
        self.j1.SetPtEtaPhiM(event.j1_p[i], 0, event.j1_theta[i], event.j1_m[i])
        self.j2.SetPtEtaPhiM(event.j2_p[i], 0, event.j2_theta[i], event.j2_m[i])
        self.j3.SetPtEtaPhiM(event.j3_p[i], 0, event.j3_theta[i], event.j3_m[i])
        self.cand_m = event.cand_m[i]
        self.passHEP = passFail

    def __lt__(self, other):
        return abs(self.cand_m - 173.4) < abs(other.cand_m - 173.4)

def resolveOverlapHEP(event, passFail):
    topCands = [simpleTopCandHEP(event, i, passFail[i]) for i in xrange(len(event.j1_p))]
    topCands.sort(reverse=True)

    finalTops = []
    usedJets = []
    for cand in topCands:
        #if not cand.j1 in usedJets and not cand.j2 in usedJets and not cand.j3 in usedJets:
        if not jetInList(cand.j1, usedJets) and not jetInList(cand.j2, usedJets) and not jetInList(cand.j3, usedJets):
            if cand.passHEP:
                usedJets += [cand.j1, cand.j2, cand.j3]
                finalTops.append(cand)

    return finalTops


print "PROCESSING TRAINING DATA"

trainingfile_ttbar = ROOT.TFile.Open("trainingTuple_division_0_TTbarSingleLep_TRF2.root")
trainingfile_znunu = ROOT.TFile.Open("trainingTuple_division_0_ZJetsToNuNu_TRF2.root")

hPtMatch   = ROOT.TH1D("hPtMatch", "hPtMatch", 50, 0.0, 2000.0)
hPtNoMatch = ROOT.TH1D("hPtNoMatch", "hPtNoMatch", 50, 0.0, 2000.0)
hPtZnunuMatch   = ROOT.TH1D("hPtZnunuMatch", "hPtZnunuMatch", 50, 0.0, 2000.0)
hPtZnunuNoMatch = ROOT.TH1D("hPtZnunuNoMatch", "hPtZnunuNoMatch", 50, 0.0, 2000.0)

NEVTS = 1e4
NEVTS_Z = 1e4

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
            hPtZnunuMatch.Fill(event.cand_pt[i])
        else:
            hPtZnunuNoMatch.Fill(event.cand_pt[i])

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
#clf = GradientBoostingClassifier(n_estimators=100, learning_rate=0.1, random_state=0, max_depth=7, verbose=2)
#clf = GradientBoostingRegressor(n_estimators=100, learning_rate=0.1, random_state=0, loss='ls')
#clf = DecisionTreeRegressor()
#clf = DecisionTreeClassifier()
#clf = svm.SVC()
clf = cv2.ml.RTrees_create()

clf.setTermCriteria((cv2.TERM_CRITERIA_COUNT, 100, 0.3));
clf.setMaxCategories(2);
clf.setMaxDepth(8);
clf.setMinSampleCount(5);

#randomize input data
perms = numpy.random.permutation(npyInputData.shape[0])
npyInputData = npyInputData[perms]
npyInputAnswer = npyInputAnswer[perms]
npyInputWgts = npyInputWgts[perms]

#make opencv TrainData container
cvTrainData = cv2.ml.TrainData_create(npyInputData, cv2.ml.ROW_SAMPLE, npyInputAnswer, sampleWeights = npyInputWgts)

#clf = clf.fit(npyInputData, npyInputAnswer, npyInputWgts)
clf.train(cvTrainData)

print "PROCESSING VALIDATION DATA"

fileValidation = ROOT.TFile.Open("trainingTuple_division_1_TTbarSingleLep_TRF2.root")

hEffNum = ROOT.TH1D("hEffNum", "hEffNum", 25, 0.0, 1000.0)
hEffDen = ROOT.TH1D("hEffDen", "hEffDen", 25, 0.0, 1000.0)
hPurityNum = ROOT.TH1D("hPurityNum", "hPurityNum", 25, 0.0, 1000.0)
hPurityDen = ROOT.TH1D("hPurityDen", "hPurityDen", 25, 0.0, 1000.0)

hEffHEPNum = ROOT.TH1D("hEffHEPNum", "hEffHEPNum", 25, 0.0, 1000.0)
hEffHEPDen = ROOT.TH1D("hEffHEPDen", "hEffHEPDen", 25, 0.0, 1000.0)
hPurityHEPNum = ROOT.TH1D("hPurityHEPNum", "hPurityHEPNum", 25, 0.0, 1000.0)
hPurityHEPDen = ROOT.TH1D("hPurityHEPDen", "hPurityHEPDen", 25, 0.0, 1000.0)

hDisc = ROOT.TH1D("disc", "disc", 20, 0, 1.0)
hDiscMatch = ROOT.TH1D("discMatch", "discMatch", 20, 0, 1.0)
hDiscMatch.SetLineColor(ROOT.kRed)
hDiscNoMatch = ROOT.TH1D("discNoMatch", "discNoMatch", 20, 0, 1.0)
hDiscNoMatch.SetLineColor(ROOT.kBlue)
hDiscMatchPt = ROOT.TH1D("discMatchPt", "discMatchPt", 20, 0, 1.0)
hDiscMatchPt.SetLineColor(ROOT.kRed)
hDiscMatchPt.SetLineStyle(ROOT.kDashed)
hDiscNoMatchPt = ROOT.TH1D("discNoMatchPt", "discNoMatchPt", 20, 0, 1.0)
hDiscNoMatchPt.SetLineColor(ROOT.kBlue)
hDiscNoMatchPt.SetLineStyle(ROOT.kDashed)

hNConstMatchTag   = ROOT.TH1D("hNConstMatchTag",   "hNConstMatchTag",   6, -0.5, 5.5)
hNConstMatchTag.SetLineColor(ROOT.kRed)
hNConstMatchNoTag = ROOT.TH1D("hNConstMatchNoTag", "hNConstMatchNoTag", 6, -0.5, 5.5)
hNConstMatchNoTag.SetLineColor(ROOT.kBlue)
hNConstMatchTagHEP   = ROOT.TH1D("hNConstMatchTagHEP",   "hNConstMatchTagHEP",   6, -0.5, 5.5)
hNConstMatchTagHEP.SetLineColor(ROOT.kRed)
hNConstMatchTagHEP.SetLineStyle(ROOT.kDashed)
hNConstMatchNoTagHEP = ROOT.TH1D("hNConstMatchNoTagHEP", "hNConstMatchNoTagHEP", 6, -0.5, 5.5)
hNConstMatchNoTagHEP.SetLineColor(ROOT.kBlue)
hNConstMatchNoTagHEP.SetLineStyle(ROOT.kDashed)

hmatchGenPt = ROOT.TH1D("hmatchGenPt", "hmatchGenPt", 25, 0.0, 1000.0)

discCut = 0.50

cut = numpy.concatenate((numpy.arange(0.01, 0.05, 0.01), numpy.arange(0.05, 1, 0.05)))
EffNumroc = len(cut) * [0]
Effroc =    len(cut) * [0]
EffDenroc = 0
EffNumrocHEP = 0
FakeNumroc = len(cut) * [0]
Fakeroc =    len(cut) * [0]
FakeDenroc = 0
FakeNumrocHEP = 0
   
inputList = []

rocInput = []
rocScore = []
rocHEP = []

Nevts = 0
for event in fileValidation.slimmedTuple:
    if Nevts >= NEVTS:
        break
    Nevts += 1
    if(len(event.genTopPt)):
        EffDenroc += 1
    for pt in event.genTopPt:
        hEffDen.Fill(pt)
        hEffHEPDen.Fill(pt)
    for i in xrange(len(event.cand_m)):
        inputList.append(dg.getData(event, i))

        rocInput.append(dg.getData(event, i))
        rocScore.append((event.genConstiuentMatchesVec[i]==3))
        rocHEP.append(HEPReqs(event, i))


print "CALCULATING DISCRIMINATORS"
npInputList = numpy.array(inputList, numpy.float32)
#output = clf.predict_proba(npInputList)[:,1]
output = [clf.predict(inputs)[0] for inputs in npInputList]

print "FILLING HISTOGRAMS"

hnTops = ROOT.TH1D("hnTop", "hnTop", 6, 0, 6)
hnTopsHEP = ROOT.TH1D("hnTopHEP", "hnTopHEP", 6, 0, 6)
hnMVAcand = ROOT.TH1D("hnMVAcand", "hnMVAcand", 6, 0, 6)
hnHEPcand = ROOT.TH1D("hnHEPcand", "hnHEPcand", 6, 0, 6)
evtWidcand = 0
cand = 0
matchcand = 0
nomatchcand =0
outputCount = 0;
Nevts = 0
for event in fileValidation.slimmedTuple:
    if Nevts >= NEVTS:
        break
    Nevts += 1
    if(len(event.cand_m)):
        evtWidcand+=1
    cand +=len(event.cand_m)
    #nCands = len(event.genConstiuentMatchesVec)
    nCands = len(event.cand_m)
    tmp_output = []
    passHEP = []
    for j in xrange(nCands):
        tmp_output.append(output[outputCount])
        outputCount += 1
        passHEP.append(HEPReqs(event, j))

    tops = resolveOverlap(event, tmp_output, discCut)
    topsHEP = resolveOverlapHEP(event, passHEP)
    hnTops.Fill(len(tops))
    hnTopsHEP.Fill(len(topsHEP))
    MVAcand =0
    HEPcand =0
    for i in xrange(len(event.cand_m)):
        #prep output
        Varsval = dg.getData(event, i)

        hDisc.Fill(tmp_output[i])
        if(tmp_output[i] > discCut):
            MVAcand +=1
            hPurityDen.Fill(event.cand_pt[i])
            hNConstMatchTag.Fill(event.genConstiuentMatchesVec[i])
            for j, vname in enumerate(varsname):
                if(histranges.has_key(vname)):
                    hist_tag[vname].Fill(Varsval[j])
        else:
            hNConstMatchNoTag.Fill(event.genConstiuentMatchesVec[i])
            for j, vname in enumerate(varsname):
                if(histranges.has_key(vname)):
                    hist_notag[vname].Fill(Varsval[j])
        passHEP = HEPReqs(event, i)
        if(passHEP):
            HEPcand +=1
            hPurityHEPDen.Fill(event.cand_pt[i])
            hNConstMatchTagHEP.Fill(event.genConstiuentMatchesVec[i])
        else:
            hNConstMatchNoTagHEP.Fill(event.genConstiuentMatchesVec[i])
        #Truth matched candidates
        if(event.genConstiuentMatchesVec[i] == 3):
            matchcand += 1
            for k in xrange(len(cut)):
                if(tmp_output[i] > cut[k]):
                    EffNumroc[k] += 1
            hmatchGenPt.Fill(event.genConstMatchGenPtVec[i])
            #pass reco discriminator threshold 
            if(tmp_output[i] > discCut):
                hEffNum.Fill(event.genConstMatchGenPtVec[i])
                hPurityNum.Fill(event.cand_pt[i])
            if(passHEP):
                hEffHEPNum.Fill(event.genConstMatchGenPtVec[i])
                hPurityHEPNum.Fill(event.cand_pt[i])
                EffNumrocHEP +=1
            hDiscMatch.Fill(tmp_output[i])
            if(event.cand_pt[i] > 250):
                hDiscMatchPt.Fill(tmp_output[i])
        #not truth matched 
        else:
            nomatchcand += 1
            hDiscNoMatch.Fill(tmp_output[i])
            if(event.cand_pt[i] > 250):
                hDiscNoMatchPt.Fill(tmp_output[i])
    hnMVAcand.Fill(MVAcand)
    hnHEPcand.Fill(HEPcand)

print "evt: ", Nevts
print "evtWidcand: ", evtWidcand
print "cand: ", cand
print "matchcand: ", matchcand
print "nomatchcand: ", nomatchcand

print "FakeRate Calculation"                                        
#FakeRate
fileFakeRate = ROOT.TFile.Open("trainingTuple_division_1_ZJetsToNuNu_TRF2.root")
hFakeNum = ROOT.TH1D("hFakeNum", "hFakeNum", 25, 0.0, 1000.0)
hFakeDen = ROOT.TH1D("hFakeDen", "hFakeDen", 25, 0.0, 1000.0)
hFakeNumHEP = ROOT.TH1D("hFakeNumHEP", "hFakeNum", 25, 0.0, 1000.0)
hFakeDenHEP = ROOT.TH1D("hFakeDenHEP", "hFakeDen", 25, 0.0, 1000.0)
hFakeNum_njet = ROOT.TH1D("hFakeNum_njet", "hFakeNum_njet", 20, 0, 20)
hFakeDen_njet = ROOT.TH1D("hFakeDen_njet", "hFakeDen_njet", 20, 0, 20)
hFakeNumHEP_njet = ROOT.TH1D("hFakeNumHEP_njet", "hFakeNumHEP_njet", 20, 0, 20)
hFakeDenHEP_njet = ROOT.TH1D("hFakeDenHEP_njet", "hFakeDenHEP_njet", 20, 0, 20)

ZinvInput = []
ZinvpassHEP = []
Nevts = 0
for event in fileFakeRate.slimmedTuple:
    if Nevts >= NEVTS_Z:
        break
    Nevts +=1
    for i in xrange(len(event.cand_m)):
        ZinvInput.append(dg.getData(event, i))
        ZinvpassHEP.append(HEPReqs(event, i))
npyZinvInput = numpy.array(ZinvInput, dtype=numpy.float32)
zinvOutput = output = [clf.predict(inputs)[0] for inputs in npyZinvInput]

hnTopsZinv = ROOT.TH1D("hnTopZinv", "hnTopZinv", 6, 0, 6)
hnTopsHEPZinv = ROOT.TH1D("hnTopHEPZinv", "hnTopHEPZinv", 6, 0, 6)

outputCount = 0
Nevts = 0
for event in fileFakeRate.slimmedTuple:
    if Nevts >= NEVTS_Z:
        break
    Nevts +=1
    FakeDenroc += 1
    hFakeDen.Fill(event.MET)
    hFakeDen_njet.Fill(event.Njet)
    hFakeDenHEP.Fill(event.MET)
    hFakeDenHEP_njet.Fill(event.Njet)
    numflag = False
    numflagHEP = False
    numflagroc = [False for r in range(len(cut))]
    tops = resolveOverlap(event, zinvOutput, discCut)
    topsHEP = resolveOverlapHEP(event, ZinvpassHEP)
    hnTopsZinv.Fill(len(tops))
    hnTopsHEPZinv.Fill(len(topsHEP))

    for j in xrange(len(event.cand_m)):
        if(zinvOutput[outputCount] > discCut):
            numflag = True
        if(ZinvpassHEP[outputCount]):
            numflagHEP = True
        for k in xrange(len(cut)):
            if(zinvOutput[outputCount]>cut[k]):
                numflagroc[k] = True
        outputCount += 1
    if(numflag):
        hFakeNum.Fill(event.MET)
        hFakeNum_njet.Fill(event.Njet)
    if(numflagHEP):
        hFakeNumHEP.Fill(event.MET)
        hFakeNumHEP_njet.Fill(event.Njet)
        FakeNumrocHEP += 1
    for k in xrange(len(cut)):
        if(numflagroc[k]):FakeNumroc[k] += 1

#print "ROC Calculation"
##ttbar
#hroc = ROOT.TProfile("hroc", "hroc", 100, 0, 1, 0, 1)
#hroc_HEP = ROOT.TProfile("hroc_HEP", "hroc_HEP", 100, 0, 1, 0, 1)
#TP =  len(cut)*[0]
#FP =  len(cut)*[0]
#TPR = len(cut)*[0]
#FPR = len(cut)*[0]
#Nmatch = 0
#Nnomatch = 0
#TPHEP =0
#FPHEP =0
#
#print "cut:", cut
#
#rocOutput = clf.predict_proba(rocInput)[:,1]
#
#for i in xrange(len(rocOutput)):
#    if(rocScore[i]):
#        Nmatch= Nmatch+1
#        if(rocHEP[i]):TPHEP = TPHEP+1
#    else:
#        Nnomatch = Nnomatch+1
#        if(rocHEP[i]):FPHEP = FPHEP+1
#    for j in xrange(len(cut)):
#        if(rocOutput[i]>cut[j]):
#            if(rocScore[i]):TP[j] = TP[j]+1
#            else:FP[j] = FP[j]+1
#
#for j in xrange(len(cut)):
#    TPR[j] = float(TP[j])/Nmatch
#    FPR[j] = float(FP[j])/Nnomatch
#    hroc.Fill(FPR[j],TPR[j])
#TPRHEP = float(TPHEP)/Nmatch
#FPRHEP = float(FPHEP)/Nnomatch
#hroc_HEP.Fill(FPRHEP,TPRHEP)
#
#print "TPR: ", TPR
#print "FPR: ", FPR
#
##Zinv
#hrocZ = ROOT.TProfile("hrocZ", "hrocZ", 100, 0, 1, 0, 1)
#hroc_HEPZ = ROOT.TProfile("hroc_HEPZ", "hroc_HEPZ", 100, 0, 1, 0, 1)
#FPZ  = len(cut) * [0]
#FPRZ = len(cut) * [0]
#NnomatchZ = 0
#FPHEPZ =0
#FPRHEPZ =0
#rocInputZ = []
#rocHEPZ = []
#for event in fileFakeRate.slimmedTuple:
#    for i in xrange(len(event.cand_m)):
#        rocInputZ.append(dg.getData(event, i))
#        rocHEPZ.append(HEPReqs(event, i))
#rocOutputZ = clf.predict_proba(rocInputZ)[:,1]
#for i in xrange(len(rocOutputZ)):
#    NnomatchZ = NnomatchZ+1
#    if(rocHEPZ[i]):FPHEPZ = FPHEPZ+1
#    for j in xrange(len(cut)):
#        if(rocOutputZ[i]>cut[j]):
#            FPZ[j] = FPZ[j]+1
#for j in xrange(len(cut)):
#    FPRZ[j] = float(FPZ[j])/NnomatchZ
#    hrocZ.Fill(FPRZ[j],TPR[j])
#FPRHEPZ = float(FPHEPZ)/NnomatchZ
#hroc_HEPZ.Fill(FPRHEPZ,TPRHEP)
#
#hroc_alt = ROOT.TProfile("hroc_alt", "hroc_alt", 100, 0, 0.5, 0, 0.5)
#hroc_HEP_alt = ROOT.TProfile("hroc_HEP_alt", "hroc_HEP_alt", 100, 0, 1, 0, 1)
#for j in xrange(len(cut)):
#    Effroc[j] = float(EffNumroc[j])/EffDenroc
#    Fakeroc[j] = float(FakeNumroc[j])/FakeDenroc
#    hroc_alt.Fill(Fakeroc[j], Effroc[j])
#EffrocHEP = float(EffNumrocHEP)/EffDenroc
#FakerocHEP = float(FakeNumrocHEP)/FakeDenroc
#hroc_HEP_alt.Fill(FakerocHEP,EffrocHEP)
#
#print "EffDenroc: ", EffDenroc
#print "EffNumroc: ", EffNumroc

print "PLOTTING"
c = ROOT.TCanvas("c1","c1",800,800)

#Draw discriminator plot

leg = ROOT.TLegend(0.55, 0.75, 0.9, 0.9)
leg.AddEntry(hDiscMatch, "Constituent Matched")
leg.AddEntry(hDiscNoMatch, "Not Constituent Matched")
leg.AddEntry(hDiscMatchPt, "Constituent Matched (pt > 250 GeV)")
leg.AddEntry(hDiscNoMatchPt, "Not Constituent Matched (pt > 250 GeV)")

hDiscMatch.SetStats(0)
hDiscMatch.SetTitle("")
hDiscMatch.GetXaxis().SetTitle("MVA Discriminator")

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
hDiscMatch.GetYaxis().SetRangeUser(0, 1.3*max([hDiscMatch.GetMaximum(), hDiscNoMatch.GetMaximum(), hDiscMatchPt.GetMaximum(), hDiscNoMatchPt.GetMaximum()]))
c.Print("discriminator_v2.png")
hDiscMatch.GetYaxis().SetRangeUser(0.001, 5)
c.SetLogy()
c.Print("discriminator_log_v2.png")

c.SetLogy(False)

#draw nTops Plot
hnTops.SetLineColor(ROOT.kRed)
hnMVAcand.SetLineColor(ROOT.kRed)
hnMVAcand.SetLineStyle(ROOT.kDashed)
hnTopsHEP.SetLineColor(ROOT.kBlue)
hnHEPcand.SetLineColor(ROOT.kBlue)
hnHEPcand.SetLineStyle(ROOT.kDashed)
hnTops.GetYaxis().SetRangeUser(0, 1.3*max([hnTops.GetMaximum(), hnTopsHEP.GetMaximum(), hnMVAcand.GetMaximum(), hnHEPcand.GetMaximum()]))
leg = ROOT.TLegend(0.55, 0.75, 0.9, 0.9)
leg.AddEntry(hnTops,"Resolved tops (MVA)")
leg.AddEntry(hnTopsHEP,"Resolved tops (HEP)")
leg.AddEntry(hnMVAcand,"Disc. passed candidates (MVA)")
leg.AddEntry(hnHEPcand,"HEP passed candidates (HEP)")
hnTops.SetStats(0)
hnTops.SetTitle("")
hnTops.Draw()
hnTopsHEP.Draw("same")
hnMVAcand.Draw("same")
hnHEPcand.Draw("same")
leg.Draw("same")
c.Print("nTops_v2.png")

#draw nTops Plot for Zinv
hnTopsZinv.SetLineColor(ROOT.kRed)
hnTopsHEPZinv.SetLineColor(ROOT.kBlue)
hnTops.GetYaxis().SetRangeUser(0, 1.3*max([hnTopsZinv.GetMaximum(), hnTopsHEPZinv.GetMaximum()]))
leg = ROOT.TLegend(0.55, 0.75, 0.9, 0.9)
leg.AddEntry(hnTops,"Resolved tops (MVA)")
leg.AddEntry(hnTopsHEP,"Resolved tops (HEP)")
hnTopsZinv.SetStats(0)
hnTopsZinv.SetTitle("")
hnTopsZinv.Draw()
hnTopsHEPZinv.Draw("same")
leg.Draw("same")
c.SetLogy()
c.Print("nTops_Zinv_v2.png")
c.SetLogy(False)

#draw num constituent matched plot

hNConstMatchTag.SetStats(0)
hNConstMatchTag.SetTitle("")
hNConstMatchTag.GetXaxis().SetTitle("N gen matched constituents")

leg = ROOT.TLegend(0.55, 0.75, 0.9, 0.9)
leg.AddEntry(hNConstMatchTag, "Top Tagged (MVA)")
leg.AddEntry(hNConstMatchNoTag, "Not Top Tagged (MVA)")
leg.AddEntry(hNConstMatchTagHEP, "Top Tagged (HEP)")
leg.AddEntry(hNConstMatchNoTagHEP, "Not Top Tagged (HEP)")

hNConstMatchTag.Scale(1/hNConstMatchTag.Integral())
hNConstMatchNoTag.Scale(1/hNConstMatchNoTag.Integral())
hNConstMatchTagHEP.Scale(1/hNConstMatchTagHEP.Integral())
hNConstMatchNoTagHEP.Scale(1/hNConstMatchNoTagHEP.Integral())

hNConstMatchTag.Draw()
hNConstMatchNoTag.Draw("same")
hNConstMatchTagHEP.Draw("same")
hNConstMatchNoTagHEP.Draw("same")
leg.Draw("same")
c.Print("nConstMatched_v2.png")

#draw variables
for h in hist_tag:
    leg = ROOT.TLegend(0.55, 0.75, 0.9, 0.9)
    leg.AddEntry(hist_tag[h], h+" for top like object")
    leg.AddEntry(hist_notag[h], h+" for not top like object")
    hist_tag[h].SetStats(0)
    hist_tag[h].SetTitle(h)
    hist_tag[h].Scale(1/hist_tag[h].Integral())
    hist_notag[h].Scale(1/hist_notag[h].Integral())
    hist_tag[h].SetLineColor(ROOT.kRed)
    hist_notag[h].SetLineColor(ROOT.kBlue)
    hist_tag[h].GetYaxis().SetRangeUser(0, 1.5*max(hist_tag[h].GetMaximum(), hist_notag[h].GetMaximum()))
    hist_tag[h].Draw()
    hist_notag[h].Draw("same")
    leg.Draw("same")
    c.Print(h+"_v2.png")

#draw efficiency
hGenPtDisc = hEffNum.Clone("hGenPtDisc")
hGenPt = hEffDen.Clone("hGenPt")
hGenPt.SetLineColor(ROOT.kBlue)
hmatchGenPt.SetLineColor(ROOT.kBlack)
hGenPtDisc.SetLineColor(ROOT.kRed)
leg = ROOT.TLegend(0.55, 0.75, 0.9, 0.9)
leg.AddEntry(hGenPt, "p_{T} of all gen tops")
leg.AddEntry(hmatchGenPt, "p_{T} of gen tops mached to a cand")
leg.AddEntry(hGenPtDisc, "p_{T} of gen tops mached to a disc. passed cand")
hGenPt.SetTitle("")
hGenPt.SetStats(0)
hGenPt.Draw()
hmatchGenPt.Draw("same")
hGenPtDisc.Draw("same")
leg.Draw("same")
c.Print("GentopPt_v2.png")

hEff = hEffNum.Clone("hEff")
hEff.SetStats(0)
hEff.SetTitle("")
hEff.GetXaxis().SetTitle("gen top Pt [GeV]")
hEff.GetYaxis().SetTitle("Efficiency")
hEff.GetYaxis().SetRangeUser(0, 1)
hEff.Divide(hEffDen)
hEffHEPNum.Divide(hEffHEPDen)
hEff.SetLineColor(ROOT.kRed)
hEffHEPNum.SetLineColor(ROOT.kBlue)
leg = ROOT.TLegend(0.55, 0.75, 0.9, 0.9)
leg.AddEntry(hEff, "MVA")
leg.AddEntry(hEffHEPNum, "HEP")
hEff.Draw()
hEffHEPNum.Draw("same")
leg.Draw("same")
c.Print("efficiency_v2.png")

print "EffDen: ", hEffDen.Integral()
print "EffNum: ", hEffNum.Integral()

#draw purity
hPurity = hPurityNum.Clone("hPurity")
hPurity.SetStats(0)
hPurity.SetTitle("")
hPurity.GetXaxis().SetTitle("reco top Pt [GeV]")
hPurity.GetYaxis().SetTitle("Purity")
hPurity.GetXaxis().SetTitle("reco top Pt [GeV]")
hPurity.GetYaxis().SetRangeUser(0, 1.3)
hPurity.Divide(hPurityDen)
hPurityHEPNum.Divide(hPurityHEPDen)
hPurity.SetLineColor(ROOT.kRed)
hPurityHEPNum.SetLineColor(ROOT.kBlue)

leg = ROOT.TLegend(0.55, 0.75, 0.9, 0.9)
leg.AddEntry(hPurity, "MVA")
leg.AddEntry(hPurityHEPNum, "HEP")

hPurity.Draw()
hPurityHEPNum.Draw("same")
leg.Draw("same")
c.Print("purity_v2.png")

#FakeRate
hFakeRate = hFakeNum.Clone("hFakeRate")
hFakeRate.SetStats(0)
hFakeRate.SetTitle("")
hFakeRate.GetXaxis().SetTitle("met [GeV]")
hFakeRate.GetYaxis().SetTitle("FakeRate")
leg = ROOT.TLegend(0.55, 0.75, 0.9, 0.9)
leg.AddEntry(hFakeRate, "MVA")
leg.AddEntry(hFakeNumHEP, "HEP")
hFakeRate.Divide(hFakeDen)
hFakeRate.GetYaxis().SetRangeUser(0, 1.3)
hFakeNumHEP.Divide(hFakeDenHEP)
hFakeNumHEP.GetYaxis().SetRangeUser(0, 1.3)
hFakeRate.SetLineColor(ROOT.kRed)
hFakeNumHEP.SetLineColor(ROOT.kBlue)
hFakeRate.Draw()
hFakeNumHEP.Draw("same")
leg.Draw("same")
c.Print("FakeRate_v2.png")

hFakeRate_njet = hFakeNum_njet.Clone("hFakeRate_njet")
hFakeRate_njet.SetStats(0)
hFakeRate_njet.SetTitle("")
hFakeRate_njet.GetXaxis().SetTitle("N_{jet}")
hFakeRate_njet.GetYaxis().SetTitle("FakeRate")
leg = ROOT.TLegend(0.55, 0.75, 0.9, 0.9)
leg.AddEntry(hFakeRate_njet, "MVA")
leg.AddEntry(hFakeNumHEP_njet, "HEP")
hFakeRate_njet.Divide(hFakeDen_njet)
hFakeRate_njet.GetYaxis().SetRangeUser(0, 1.3)
hFakeNumHEP_njet.Divide(hFakeDenHEP_njet)
hFakeNumHEP_njet.GetYaxis().SetRangeUser(0, 1.3)
hFakeRate_njet.SetLineColor(ROOT.kRed)
hFakeNumHEP_njet.SetLineColor(ROOT.kBlue)
hFakeRate_njet.Draw()
hFakeNumHEP_njet.Draw("same")
leg.Draw("same")
c.Print("FakeRate_njet_v2.png")

#print roc
#hroc.SetStats(0)
#hroc.SetTitle("ROC:Objectwise (t#bart)")
#hroc.SetXTitle("FPR")
#hroc.SetYTitle("TPR")
#hroc.GetYaxis().SetRangeUser(0, 1)
#hroc_HEP.SetStats(0)
#hroc_HEP.SetLineColor(ROOT.kRed)
#hroc_HEP.SetMarkerColor(ROOT.kRed)
#hroc_HEP.SetMarkerStyle(20)
#hroc_HEP.SetMarkerSize(1)
#hroc.SetMarkerStyle(20)
#hroc.SetMarkerSize(1)
#hroc.Draw("pe")
#hroc_HEP.Draw("samePE")
#c.Print("roc_v2.png")
#
#hrocZ.SetTitle("ROC:Objectwise (t#bar{t} and Z_{inv})")
#hrocZ.SetStats(0)
#hrocZ.SetXTitle("FPR")
#hrocZ.SetYTitle("TPR")
#hroc_HEPZ.SetStats(0)
#hroc_HEPZ.SetLineColor(ROOT.kRed)
#hroc_HEPZ.SetMarkerColor(ROOT.kRed)
#hroc_HEPZ.SetMarkerStyle(20)
#hroc_HEPZ.SetMarkerSize(1)
#hrocZ.SetMarkerStyle(20)
#hrocZ.SetMarkerSize(1)
#hrocZ.GetYaxis().SetRangeUser(0, 1)
#hrocZ.Draw()
#hroc_HEPZ.Draw("samePE")
#c.Print("rocZ_v2.png")
#
#hroc_alt.SetTitle("ROC:Eff.(t#bar{t}) and Fakerate(Z_{inv})")
#hroc_alt.SetStats(0)
#hroc_alt.SetXTitle("FakeRate")
#hroc_alt.SetYTitle("Efficiency")
#hroc_alt.GetYaxis().SetRangeUser(0, 0.5)
#hroc_HEP_alt.SetStats(0)
#hroc_HEP_alt.SetLineColor(ROOT.kRed)
#hroc_HEP_alt.SetMarkerColor(ROOT.kRed)
#hroc_HEP_alt.SetMarkerStyle(20)
#hroc_HEP_alt.SetMarkerSize(1)
#hroc_alt.SetMarkerStyle(20)
#hroc_alt.SetMarkerSize(1)
#hroc_alt.Draw("pe")
#hroc_HEP_alt.Draw("samePE")
#c.Print("roc_alt_v2.png")

# Plot feature importance
#feature_importance = clf.feature_importances_
#feature_names = numpy.array(dg.getList())
#feature_importance = 100.0 * (feature_importance / feature_importance.max())
#sorted_idx = numpy.argsort(feature_importance)

#try to plot it with matplotlib
#try:
#    import matplotlib.pyplot as plt
#
#    # make importances relative to max importance
#    pos = numpy.arange(sorted_idx.shape[0]) + .5
#    #plt.subplot(1, 2, 2)
#    plt.barh(pos, feature_importance[sorted_idx], align='center')
#    plt.yticks(pos, feature_names[sorted_idx])
#    plt.xlabel('Relative Importance')
#    plt.title('Variable Importance')
#    #plt.show()
#    plt.savefig("feature_importance.png")
#except ImportError:
#    #I guess no matplotlib is installed, just print to screen?
#    featureImportanceandNames = zip(feature_names, feature_importance)
#    print [featureImportanceandNames[a] for a in sorted_idx].reverse()

print "DONE!"

#Writing histograms in a root file.
mf = ROOT.TFile('MVAOutput.root','RECREATE')
hDiscMatch.Write()
hDiscNoMatch.Write()
hDiscMatchPt.Write()
hDiscNoMatchPt.Write()
hNConstMatchTag.Write()
hNConstMatchNoTag.Write()
hNConstMatchTagHEP.Write()
hNConstMatchNoTagHEP.Write()
for h in hist_tag:
    hist_tag[h].Write()
    hist_notag[h].Write()
hEff.Write()
#hEffHEP.Write()
hPurity.Write()
#hPurityHEP.Write()
hFakeRate.Write()
#hroc.Write()
#hroc_HEP.Write()
#hrocZ.Write()
#hroc_HEPZ.Write()
#hroc_alt.Write()
#hroc_HEP_alt.Wite()
mf.Write()
mf.Close()
