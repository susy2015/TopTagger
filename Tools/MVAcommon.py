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
        self.list = ["cand_m", "j12_m", "j13_m", "j23_m", "dTheta12", "dTheta23", "dTheta13", "j1_p", "j2_p", "j3_p", "j1_theta", "j2_theta", "j3_theta", "j12_dTheta", "j13_dTheta", "j23_dTheta", "j1_CSV", "j2_CSV", "j3_CSV", "j1_QGL", "j2_QGL", "j3_QGL"]
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
              "j12_m":[100, 0, 250],
              "j13_m":[100, 0, 250],
              "dTheta12":[50,0,4],
              "dTheta23":[50,0,4],
              "dTheta13":[50,0,4],
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
              "j23_dTheta":[50, 0, 4],
              "j13_dTheta":[50, 0, 4],
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
        self.j1.SetPtEtaPhiM(event.j1_pt[i], event.j1_eta[i], event.j1_phi[i], event.j1_m[i])
        self.j2.SetPtEtaPhiM(event.j2_pt[i], event.j2_eta[i], event.j2_phi[i], event.j2_m[i])
        self.j3.SetPtEtaPhiM(event.j3_pt[i], event.j3_eta[i], event.j3_phi[i], event.j3_m[i])
        self.discriminator = discriminator

    def __lt__(self, other):
        return self.discriminator < other.discriminator

def jetInList(jet, jlist):
    for j in jlist:
        if(abs(jet.M() - j.M()) < 0.0001):
            return True
    return False

def resolveOverlap(event, discriminators, threshold):
    topCands = [simpleTopCand(event, i, discriminators[i]) for i in xrange(len(event.j1_pt))]
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
        self.j1.SetPtEtaPhiM(event.j1_pt[i], event.j1_eta[i], event.j1_phi[i], event.j1_m[i])
        self.j2.SetPtEtaPhiM(event.j2_pt[i], event.j2_eta[i], event.j2_phi[i], event.j2_m[i])
        self.j3.SetPtEtaPhiM(event.j3_pt[i], event.j3_eta[i], event.j3_phi[i], event.j3_m[i])
        self.cand_m = event.cand_m[i]
        self.passHEP = passFail

    def __lt__(self, other):
        return abs(self.cand_m - 173.4) < abs(other.cand_m - 173.4)

def resolveOverlapHEP(event, passFail):
    topCands = [simpleTopCandHEP(event, i, passFail[i]) for i in xrange(len(event.j1_pt))]
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

NEVTS = 1e10
NEVTS_Z = 1e10
#disc cut
discCut = 0.50

hPtMatch   = ROOT.TH1D("hPtMatch", "hPtMatch", 50, 0.0, 2000.0)
hPtNoMatch = ROOT.TH1D("hPtNoMatch", "hPtNoMatch", 50, 0.0, 2000.0)
hPtZnunuMatch   = ROOT.TH1D("hPtZnunuMatch", "hPtZnunuMatch", 50, 0.0, 2000.0)
hPtZnunuNoMatch = ROOT.TH1D("hPtZnunuNoMatch", "hPtZnunuNoMatch", 50, 0.0, 2000.0)


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

hnTops = ROOT.TH1D("hnTop", "hnTop", 6, 0, 6)
hnTopsHEP = ROOT.TH1D("hnTopHEP", "hnTopHEP", 6, 0, 6)
hnMVAcand = ROOT.TH1D("hnMVAcand", "hnMVAcand", 6, 0, 6)
hnHEPcand = ROOT.TH1D("hnHEPcand", "hnHEPcand", 6, 0, 6)
hnTopsZinv = ROOT.TH1D("hnTopZinv", "hnTopZinv", 6, 0, 6)
hnTopsHEPZinv = ROOT.TH1D("hnTopHEPZinv", "hnTopHEPZinv", 6, 0, 6)
hFakeNum = ROOT.TH1D("hFakeNum", "hFakeNum", 25, 0.0, 1000.0)
hFakeDen = ROOT.TH1D("hFakeDen", "hFakeDen", 25, 0.0, 1000.0)
hFakeNumHEP = ROOT.TH1D("hFakeNumHEP", "hFakeNum", 25, 0.0, 1000.0)
hFakeDenHEP = ROOT.TH1D("hFakeDenHEP", "hFakeDen", 25, 0.0, 1000.0)
hFakeNum_njet = ROOT.TH1D("hFakeNum_njet", "hFakeNum_njet", 20, 0, 20)
hFakeDen_njet = ROOT.TH1D("hFakeDen_njet", "hFakeDen_njet", 20, 0, 20)
hFakeNumHEP_njet = ROOT.TH1D("hFakeNumHEP_njet", "hFakeNumHEP_njet", 20, 0, 20)
hFakeDenHEP_njet = ROOT.TH1D("hFakeDenHEP_njet", "hFakeDenHEP_njet", 20, 0, 20)

#ROC
#ttbar
hroc = ROOT.TProfile("hroc", "hroc", 100, 0, 1, 0, 1)
hroc_HEP = ROOT.TProfile("hroc_HEP", "hroc_HEP", 100, 0, 1, 0, 1)
#Zinv
hrocZ = ROOT.TProfile("hrocZ", "hrocZ", 100, 0, 1, 0, 1)
hroc_HEPZ = ROOT.TProfile("hroc_HEPZ", "hroc_HEPZ", 100, 0, 1, 0, 1)

hroc_alt = ROOT.TProfile("hroc_alt", "hroc_alt", 100, 0, 1, 0, 1)
hroc_HEP_alt = ROOT.TProfile("hroc_HEP_alt", "hroc_HEP_alt", 100, 0, 1, 0, 1)

