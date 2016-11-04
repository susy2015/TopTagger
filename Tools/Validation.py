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
varsname = dg.getList()

print "PROCESSING VALIDATION DATA"

#Get training output
if options.opencv:
    #This is absolutely terrible, but I have to recreate the openCV training here because it cannot be loaded in python yet
    import Training
    clf1 = Training.clf
else:
    fileTraining = open("TrainingOutput.pkl",'r')
    clf1 = pickle.load(fileTraining)
    fileTraining.close()

fileValidation = ROOT.TFile.Open("trainingTuple_division_1_TTbarSingleLep_validation.root")

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
    if event.Njet<4 : continue
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
if options.opencv:
    output = [clf1.predict(inputs)[0] for inputs in npInputList]
else:
    output = clf1.predict_proba(npInputList)[:,1]

print "FILLING HISTOGRAMS"

evtWidcand = 0
cand = 0
matchcand = 0
nomatchcand =0
outputCount = 0;
Nevts = 0
for event in fileValidation.slimmedTuple:
    if event.Njet<4 : continue
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
        #if event.cand_pt[i] < 100: continue
        #prep output
        Varsval = dg.getData(event, i)
        hDisc.Fill(tmp_output[i])
        hMVAdisc_pt.Fill(event.cand_pt[i], tmp_output[i])
        if(tmp_output[i] > discCut):
            MVAcand +=1
            hPurityDen.Fill(event.cand_pt[i])
            hPurity_discDen.Fill(tmp_output[i])
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
            hj1Theta.Fill(math.cos(event.j1_theta[i]))
            hj2Theta.Fill(math.cos(event.j2_theta[i]))
            hj3Theta.Fill(math.cos(event.j3_theta[i]))
            for k in xrange(len(cut)):
                if(tmp_output[i] > cut[k]):
                    EffNumroc[k] += 1
            hmatchGenPt.Fill(event.genConstMatchGenPtVec[i])
            #pass reco discriminator threshold
            if(tmp_output[i] > discCut):
                hEffNum.Fill(event.genConstMatchGenPtVec[i])
                hPurityNum.Fill(event.cand_pt[i])
                hPurity_discNum.Fill(tmp_output[i])
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
fileFakeRate = ROOT.TFile.Open("trainingTuple_division_1_ZJetsToNuNu_validation.root")

ZinvInput = []
ZinvpassHEP = []
Nevts = 0
for event in fileFakeRate.slimmedTuple:
    if event.Njet<4 : continue
    if Nevts >= NEVTS_Z:
        break
    Nevts +=1
    for i in xrange(len(event.cand_m)):
        ZinvInput.append(dg.getData(event, i))
        ZinvpassHEP.append(HEPReqs(event, i))
if options.opencv:
    npZinvInput = numpy.array(ZinvInput, dtype=numpy.float32)
    zinvOutput = [clf1.predict(inputs)[0] for inputs in npZinvInput]
else:
    zinvOutput = clf1.predict_proba(ZinvInput)[:,1]

outputCount = 0
Nevts = 0
for event in fileFakeRate.slimmedTuple:
    if event.Njet<4 : continue
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

if not options.noROC:
    print "ROC Calculation"
    #ttbar
    TP =  len(cut)*[0]
    FP =  len(cut)*[0]
    TPR = len(cut)*[0]
    FPR = len(cut)*[0]
    Nmatch = 0
    Nnomatch = 0
    TPHEP =0
    FPHEP =0
    
    print "cut:", cut
    
    if options.opencv:
        nprocInput = numpy.array(rocInput, dtype=numpy.float32)
        rocOutput = [clf1.predict(inputs)[0] for inputs in nprocInput]
    else:
        rocOutput = clf1.predict_proba(rocInput)[:,1]
    
    for i in xrange(len(rocOutput)):
        if(rocScore[i]):
            Nmatch= Nmatch+1
            if(rocHEP[i]):TPHEP = TPHEP+1
        else:
            Nnomatch = Nnomatch+1
            if(rocHEP[i]):FPHEP = FPHEP+1
        for j in xrange(len(cut)):
            if(rocOutput[i]>cut[j]):
                if(rocScore[i]):TP[j] = TP[j]+1
                else:FP[j] = FP[j]+1
    
    for j in xrange(len(cut)):
        TPR[j] = float(TP[j])/Nmatch
        FPR[j] = float(FP[j])/Nnomatch
        hroc.Fill(FPR[j],TPR[j])
    TPRHEP = float(TPHEP)/Nmatch
    FPRHEP = float(FPHEP)/Nnomatch
    hroc_HEP.Fill(FPRHEP,TPRHEP)
    
    print "TPR: ", TPR
    print "FPR: ", FPR
    
    #Zinv
    
    FPZ  = len(cut) * [0]
    FPRZ = len(cut) * [0]
    NnomatchZ = 0
    FPHEPZ =0
    FPRHEPZ =0
    rocInputZ = []
    rocHEPZ = []
    for event in fileFakeRate.slimmedTuple:
        if event.Njet<4 : continue
        for i in xrange(len(event.cand_m)):
            rocInputZ.append(dg.getData(event, i))
            rocHEPZ.append(HEPReqs(event, i))
    
    nprocInputZ = numpy.array(rocInputZ, dtype=numpy.float32)
    if options.opencv:
        rocOutputZ = [clf1.predict(inputs)[0] for inputs in nprocInputZ]
    else:
        rocOutputZ = clf1.predict_proba(rocInputZ)[:,1]
    
    for i in xrange(len(rocOutputZ)):
        NnomatchZ = NnomatchZ+1
        if(rocHEPZ[i]):FPHEPZ = FPHEPZ+1
        for j in xrange(len(cut)):
            if(rocOutputZ[i]>cut[j]):
                FPZ[j] = FPZ[j]+1
    for j in xrange(len(cut)):
        FPRZ[j] = float(FPZ[j])/NnomatchZ
        hrocZ.Fill(FPRZ[j],TPR[j])
    FPRHEPZ = float(FPHEPZ)/NnomatchZ
    hroc_HEPZ.Fill(FPRHEPZ,TPRHEP)
    
    
    for j in xrange(len(cut)):
        Effroc[j] = float(EffNumroc[j])/EffDenroc
        Fakeroc[j] = float(FakeNumroc[j])/FakeDenroc
        hroc_alt.Fill(Fakeroc[j], Effroc[j])
        hEff_disc.Fill(cut[j],Effroc[j])
        hFake_disc.Fill(cut[j],Fakeroc[j])
    EffrocHEP = float(EffNumrocHEP)/EffDenroc
    FakerocHEP = float(FakeNumrocHEP)/FakeDenroc
    hroc_HEP_alt.Fill(FakerocHEP,EffrocHEP)
    
    

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
c.Print("discriminator.png")
hDiscMatch.GetYaxis().SetRangeUser(0.001, 5)
c.SetLogy()
c.Print("discriminator_log.png")

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
c.Print("nTops.png")

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
c.Print("nTops_Zinv.png")
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
c.Print("nConstMatched.png")

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
    c.Print(h+".png")

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
c.Print("GentopPt.png")

hEff = hEffNum.Clone("hEff")
hEff.SetStats(0)
hEff.SetTitle("")
hEff.GetXaxis().SetTitle("gen top Pt [GeV]")
hEff.GetYaxis().SetTitle("Efficiency")
hEff.GetYaxis().SetRangeUser(0, 1)
hEff.Divide(hEffDen)
hEffHEP = hEffHEPNum.Clone("hEffHEP")
hEffHEP.Divide(hEffHEPDen)
hEff.SetLineColor(ROOT.kRed)
hEffHEP.SetLineColor(ROOT.kBlue)
leg = ROOT.TLegend(0.55, 0.75, 0.9, 0.9)
leg.AddEntry(hEff, "MVA")
leg.AddEntry(hEffHEP, "HEP")
hEff.Draw()
hEffHEP.Draw("same")
leg.Draw("same")
c.Print("efficiency.png")

print "EffDen: ", hEffDen.Integral()
print "EffNum: ", hEffNum.Integral()

#draw purity
hPurity = hPurityNum.Clone("hPurity")
hPurity.SetStats(0)
hPurity.SetTitle("")
hPurity.GetXaxis().SetTitle("reco top Pt [GeV]")
hPurity.GetYaxis().SetTitle("Purity")
hPurity.GetYaxis().SetRangeUser(0, 1.3)
hPurity.Divide(hPurityDen)
hPurityHEP = hPurityHEPNum.Clone("hPurityHEP")
hPurityHEP.Divide(hPurityHEPDen)
hPurity.SetLineColor(ROOT.kRed)
hPurityHEP.SetLineColor(ROOT.kBlue)

leg = ROOT.TLegend(0.55, 0.75, 0.9, 0.9)
leg.AddEntry(hPurity, "MVA")
leg.AddEntry(hPurityHEP, "HEP")

hPurity.Draw()
hPurityHEP.Draw("same")
leg.Draw("same")
c.Print("purity.png")


#FakeRate
hFakeRate_met = hFakeNum.Clone("hFakeRate_met")
hFakeRateHEP_met = hFakeNumHEP.Clone("hFakeRateHEP_met")
hFakeRate_met.SetStats(0)
hFakeRate_met.SetTitle("")
hFakeRate_met.GetXaxis().SetTitle("met [GeV]")
hFakeRate_met.GetYaxis().SetTitle("FakeRate")
leg = ROOT.TLegend(0.55, 0.75, 0.9, 0.9)
leg.AddEntry(hFakeRate_met, "MVA")
leg.AddEntry(hFakeRateHEP_met, "HEP")
hFakeRate_met.Divide(hFakeDen)
hFakeRate_met.GetYaxis().SetRangeUser(0, 1.3)
hFakeRateHEP_met.Divide(hFakeDenHEP)
hFakeRateHEP_met.GetYaxis().SetRangeUser(0, 1.3)
hFakeRate_met.SetLineColor(ROOT.kRed)
hFakeRateHEP_met.SetLineColor(ROOT.kBlue)
hFakeRate_met.Draw()
hFakeRateHEP_met.Draw("same")
leg.Draw("same")
c.Print("FakeRate_met.png")

hFakeRate_njet = hFakeNum_njet.Clone("hFakeRate_njet")
hFakeRateHEP_njet = hFakeNumHEP_njet.Clone("hFakeRateHEP_njet")
hFakeRate_njet.SetStats(0)
hFakeRate_njet.SetTitle("")
hFakeRate_njet.GetXaxis().SetTitle("N_{jet}")
hFakeRate_njet.GetYaxis().SetTitle("FakeRate")
leg = ROOT.TLegend(0.55, 0.75, 0.9, 0.9)
leg.AddEntry(hFakeRate_njet, "MVA")
leg.AddEntry(hFakeRateHEP_njet, "HEP")
hFakeRate_njet.Divide(hFakeDen_njet)
hFakeRate_njet.GetYaxis().SetRangeUser(0, 1.3)
hFakeRateHEP_njet.Divide(hFakeDenHEP_njet)
hFakeRateHEP_njet.GetYaxis().SetRangeUser(0, 1.3)
hFakeRate_njet.SetLineColor(ROOT.kRed)
hFakeRateHEP_njet.SetLineColor(ROOT.kBlue)
hFakeRate_njet.Draw()
hFakeRateHEP_njet.Draw("same")
leg.Draw("same")
c.Print("FakeRate_njet.png")

if not options.noROC:
    #print roc
    hroc.SetStats(0)
    hroc.SetTitle("ROC:Objectwise (t#bar{t})")
    hroc.SetXTitle("FPR")
    hroc.SetYTitle("TPR")
    hroc.GetYaxis().SetRangeUser(0, 1)
    hroc_HEP.SetStats(0)
    hroc_HEP.SetLineColor(ROOT.kRed)
    hroc_HEP.SetMarkerColor(ROOT.kRed)
    hroc_HEP.SetMarkerStyle(20)
    hroc_HEP.SetMarkerSize(1)
    hroc.SetMarkerStyle(20)
    hroc.SetMarkerSize(1)
    hroc.Draw("pe")
    hroc_HEP.Draw("samePE")
    c.Print("roc.png")
    
    hrocZ.SetTitle("ROC:Objectwise (t#bar{t} and Z_{inv})")
    hrocZ.SetStats(0)
    hrocZ.SetXTitle("FPR")
    hrocZ.SetYTitle("TPR")
    hroc_HEPZ.SetStats(0)
    hroc_HEPZ.SetLineColor(ROOT.kRed)
    hroc_HEPZ.SetMarkerColor(ROOT.kRed)
    hroc_HEPZ.SetMarkerStyle(20)
    hroc_HEPZ.SetMarkerSize(1)
    hrocZ.SetMarkerStyle(20)
    hrocZ.SetMarkerSize(1)
    hrocZ.GetYaxis().SetRangeUser(0, 1)
    hrocZ.Draw("pe")
    hroc_HEPZ.Draw("samePE")
    c.Print("rocZ.png")
    
    hroc_alt.SetTitle("ROC:Eff.(t#bar{t}) and Fakerate(Z_{inv})")
    hroc_alt.SetStats(0)
    hroc_alt.SetXTitle("FakeRate")
    hroc_alt.SetYTitle("Efficiency")
    hroc_alt.GetYaxis().SetRangeUser(0, 1)
    hroc_alt.GetXaxis().SetRangeUser(0, 1)
    hroc_HEP_alt.SetStats(0)
    hroc_HEP_alt.SetLineColor(ROOT.kRed)
    hroc_HEP_alt.SetMarkerColor(ROOT.kRed)
    hroc_HEP_alt.SetMarkerStyle(20)
    hroc_HEP_alt.SetMarkerSize(1)
    hroc_alt.SetMarkerStyle(20)
    hroc_alt.SetMarkerSize(1)
    hroc_alt.Draw("pe")
    hroc_HEP_alt.Draw("samePE")
    c.Print("roc_alt.png")

#few test plots
hj1Theta.SetTitle("cos#theta(topboost, j_{i})")
hj1Theta.SetStats(0)
hj1Theta.SetLineColor(ROOT.kRed)
hj2Theta.SetLineColor(ROOT.kBlack)
hj3Theta.SetLineColor(ROOT.kBlue)
leg = ROOT.TLegend(0.55, 0.75, 0.9, 0.9)
leg.AddEntry(hj1Theta, "j1")
leg.AddEntry(hj2Theta, "j2")
leg.AddEntry(hj3Theta, "j3")
hj1Theta.Draw()
hj2Theta.Draw("same")
hj3Theta.Draw("same")
leg.Draw("same")
c.Print("Matched_theta.png")

hPurity_disc = hPurity_discNum.Clone("hPurity_disc")
hPurity_disc.SetStats(0)
hPurity_disc.SetTitle("")
hPurity_disc.GetXaxis().SetTitle("MVA disc.")
hPurity_disc.GetYaxis().SetTitle("Purity")
hPurity_disc.GetYaxis().SetRangeUser(0, 1.3)
hPurity_disc.Divide(hPurity_discDen)
hPurity_disc.Draw()
c.Print("purity_disc.png")

hMVAdisc_pt.Draw("colz")
c.Print("discriminator_vs_candPt.png")

from sklearn.covariance import EmpiricalCovariance

npRocInput = numpy.array(rocInput)
npRocAnswers = numpy.array(rocScore)
slimNpData0 = npRocInput[npRocAnswers==0]
slimNpData1 = npRocInput[npRocAnswers==1]

ecv = EmpiricalCovariance()
ecv.fit(slimNpData0)

from scipy.linalg import fractional_matrix_power
def diagElements(m):
    size = m.shape[0]
    return numpy.matrix(numpy.diag([m[i, i] for i in xrange(size)]))

def corrMat(m):
    sqrt_diag = fractional_matrix_power(diagElements(m), -0.5)
    return numpy.array(sqrt_diag * m  * sqrt_diag)

corr0 = corrMat(numpy.matrix(ecv.covariance_))

ecv.fit(slimNpData0)

corr1 = corrMat(numpy.matrix(ecv.covariance_))

ecv.fit(nprocInputZ)

corrZ = corrMat(numpy.matrix(ecv.covariance_))

#try to plot it with matplotlib
try:
    import matplotlib.pyplot as plt

    plt.matshow(corr0, cmap=plt.cm.seismic, vmin = -1, vmax = 1)
    varlist = dg.getList()
    plt.xticks(range(len(varlist)), varlist, rotation='vertical')
    plt.yticks(range(len(varlist)), varlist)
    plt.colorbar(orientation='vertical')
    plt.savefig("feature_corrolation_ttbar_nomatch.png")

    plt.matshow(corr1, cmap=plt.cm.seismic, vmin = -1, vmax = 1)
    varlist = dg.getList()
    plt.xticks(range(len(varlist)), varlist, rotation='vertical')
    plt.yticks(range(len(varlist)), varlist)
    plt.colorbar(orientation='vertical')
    plt.savefig("feature_corrolation_ttbar_match.png")

    plt.matshow(corrZ, cmap=plt.cm.seismic, vmin = -1, vmax = 1)
    varlist = dg.getList()
    plt.xticks(range(len(varlist)), varlist, rotation='vertical')
    plt.yticks(range(len(varlist)), varlist)
    plt.colorbar(orientation='vertical')
    plt.savefig("feature_corrolation_Znunu.png")
except ImportError:
    pass #I guess we could puta root based backup here?

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
hEffHEP.Write()
hPurity.Write()
hPurityHEP.Write()
hFakeRate_met.Write()
hFakeRateHEP_met.Write()
hFakeRate_njet.Write()
hFakeRateHEP_njet.Write()
if not options.noROC:
    hroc.Write()
    hroc_HEP.Write()
    hrocZ.Write()
    hroc_HEPZ.Write()
    hroc_alt.Write()
    hroc_HEP_alt.Write()
    hEff_disc.Write()
    hFake_disc.Write()
mf.Write()
mf.Close()

print "VALIDATION DONE!"
