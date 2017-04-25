import sys
import ROOT
import numpy
import math
from MVAhistograms_tf import *
from MVAcommon_tf import *
import tensorflow as tf
import optparse

parser = optparse.OptionParser("usage: %prog [options]\n")

parser.add_option ('-o', "--opencv", dest='opencv', action='store_true', help="Run using opencv RTrees")
parser.add_option ('-n', "--noRoc", dest='noROC', action='store_true', help="Do not calculate ROC to save time")
parser.add_option ('-d', "--disc", dest='discCut', action='store', default=0.6, help="Discriminator cut")

options, args = parser.parse_args()

#disc cut
discCut = options.discCut

dg = DataGetter()
varsname = dg.getList()

print "PROCESSING TTBAR VALIDATION DATA"

#Get training output
saver = tf.train.import_meta_graph('models/model.ckpt.meta')
sess = tf.Session()
# To initialize values with saved data
saver.restore(sess, './models/model.ckpt')
# Restrieve useful variables
trainInfo = tf.get_collection('TrainInfo')
x = trainInfo[0]
y_train = trainInfo[1]

#Get data transformation
mins = []
ptps = []
with open("DataScales.txt", "r") as f:
    for line in f:
        if line[0:5] == "mins:":
            mins = [float(s) for s in line[5:].split()]
        if line[0:5] == "ptps:":
            ptps = [float(s) for s in line[5:].split()]

fileValidation = ROOT.TFile.Open("trainingTuple_division_1_TTbarSingleLep_validation.root")

#cut = numpy.concatenate((numpy.arange(0.01, 0.05, 0.01), numpy.arange(0.05, 1, 0.05)))
cut = numpy.arange(0.01, 1.0, 0.01)
cut = numpy.append(numpy.append([0.0, 0.002, 0.004, 0.006, 0.008], cut), [0.992, 0.994, 0.996, 0.998, 1.0])
cut_fake_eff = numpy.arange(-1.0, 1.0, 0.01)
EffNumroc = len(cut_fake_eff) * [0]
Effroc =    len(cut_fake_eff) * [0]
EffDenroc = 0
EffNumrocHEP = 0
FakeNumroc = len(cut_fake_eff) * [0]
Fakeroc =    len(cut_fake_eff) * [0]
FakeDenroc = 0
FakeNumrocHEP = 0

rocInput = []
rocScore = []
rocHEP = []
evtwgt = []
Nevts = 0

outputCount = 0
Nevts = 0
for event in fileValidation.slimmedTuple:
    if event.Njet<4 : continue
    if Nevts >= NEVTS:
        break
    Nevts += 1

    #Due to shortcommings in PyROOT this cashing of values should speed things up
    sampleWgt = event.sampleWgt
    cand_pt = event.cand_pt
    genConstiuentMatchesVec = event.genConstiuentMatchesVec
    genConstMatchGenPtVec = event.genConstMatchGenPtVec
    cand_m = event.cand_m
    genTopPt = event.genTopPt
    data = dg.getData(event)

    if(len(genTopPt)):
        EffDenroc += 1*sampleWgt
    for pt in genTopPt:
        hEffDen.Fill(pt, sampleWgt)
        hEffHEPDen.Fill(pt, sampleWgt)
    inputList = []
    passHEP = []
    nCands = len(cand_m)
    for i in xrange(len(cand_m)):
        inputList.append(data[i])
        rocInput.append(data[i])
        rocScore.append((genConstiuentMatchesVec[i]==3))
        rocHEP.append(HEPReqs(event, i))
        evtwgt.append(sampleWgt)
        passHEP.append(HEPReqs(event, i))

    #Batch calculate NN output to save python function calls
    tmp_output = sess.run(y_train, feed_dict={x: data})[:,0]

    tops = resolveOverlap(event, tmp_output, discCut)
    topsHEP = resolveOverlapHEP(event, passHEP)
    hnTops.Fill(len(tops))
    hnTopsHEP.Fill(len(topsHEP))
    MVAcand =0
    HEPcand =0
    MatchMVAcand =0
    for i in xrange(len(cand_m)):
        #if cand_pt[i] < 100: continue
        #prep output
        Varsval = data[i]
        hDisc.Fill(tmp_output[i], sampleWgt)
        hMVAdisc_pt.Fill(cand_pt[i], tmp_output[i], sampleWgt)
        if(tmp_output[i] > discCut):
            MVAcand +=1
            hPurityDen.Fill(cand_pt[i], sampleWgt)
            hPurity_discDen.Fill(tmp_output[i], sampleWgt)
            hNConstMatchTag.Fill(genConstiuentMatchesVec[i])
            for j, vname in enumerate(varsname):
                if(histranges.has_key(vname)):
                    hist_tag[vname].Fill(Varsval[j])
        else:
            hNConstMatchNoTag.Fill(genConstiuentMatchesVec[i])
            for j, vname in enumerate(varsname):
                if(histranges.has_key(vname)):
                    hist_notag[vname].Fill(Varsval[j])
        passHEP = HEPReqs(event, i)
        if(passHEP):
            HEPcand +=1
            hPurityHEPDen.Fill(cand_pt[i], sampleWgt)
            hNConstMatchTagHEP.Fill(genConstiuentMatchesVec[i])
        else:
            hNConstMatchNoTagHEP.Fill(genConstiuentMatchesVec[i])
        #Truth matched candidates
        if(genConstiuentMatchesVec[i] == 3):
            for k in xrange(len(cut_fake_eff)):
                discpass = (tmp_output[i]>cut_fake_eff[k] or tmp_output[i]==cut_fake_eff[k]) if(k==0) else (tmp_output[i]>cut_fake_eff[k])
                if(discpass):
                    EffNumroc[k] += 1*sampleWgt
            hmatchGenPt.Fill(genConstMatchGenPtVec[i])
            #pass reco discriminator threshold
            if(tmp_output[i] > discCut):
                MatchMVAcand +=1
                hEffNum.Fill(genConstMatchGenPtVec[i], sampleWgt)
                hPurityNum.Fill(cand_pt[i], sampleWgt)
                hPurity_discNum.Fill(tmp_output[i], sampleWgt)
            if(passHEP):
                hEffHEPNum.Fill(genConstMatchGenPtVec[i],sampleWgt)
                hPurityHEPNum.Fill(cand_pt[i],sampleWgt)
                EffNumrocHEP +=1*sampleWgt
            hDiscMatch.Fill(tmp_output[i], sampleWgt)
            if(cand_pt[i] > 250):
                hDiscMatchPt.Fill(tmp_output[i], sampleWgt)
        #not truth matched
        else:
            hDiscNoMatch.Fill(tmp_output[i], sampleWgt)
            if(cand_pt[i] > 250):
                hDiscNoMatchPt.Fill(tmp_output[i], sampleWgt)
    hnMVAcand.Fill(MVAcand, sampleWgt)
    hnHEPcand.Fill(HEPcand, sampleWgt)
    hnMatchMVAcand.Fill(MatchMVAcand, sampleWgt)

#print "evt: ", Nevts

print "PROCESS FAKERATE VALIDATION DATA"
#FakeRate
fileFakeRate = ROOT.TFile.Open("trainingTuple_division_1_ZJetsToNuNu_validation.root")

outputCount = 0
Nevts = 0

rocInputZ = []
rocHEPZ = []
evtwgtZ = []

for event in fileFakeRate.slimmedTuple:
    if event.Njet<4 : continue
    if Nevts >= NEVTS_Z:
        break
    Nevts +=1

    #again do this as PyROOT is lacking cashing
    sampleWgt = event.sampleWgt
    MET = event.MET
    Njet = event.Njet
    cand_m = event.cand_m
    cand_pt = event.cand_pt
    data = dg.getData(event)

    ZinvInput = []
    ZinvpassHEP = []
    for i in xrange(len(cand_m)):
        ZinvInput.append(data[i])
        ZinvpassHEP.append(HEPReqs(event, i))

    #batch calculate NN otuput
    zinvOutput = sess.run(y_train, feed_dict={x: data})[:,0]

    FakeDenroc += 1*sampleWgt
    hFakeDen.Fill(MET, sampleWgt)
    hFakeDen_njet.Fill(Njet, sampleWgt)
    hFakeDenHEP.Fill(MET, sampleWgt)
    hFakeDenHEP_njet.Fill(Njet, sampleWgt)
    numflag = False
    numflagHEP = False
    numflagroc = len(cut_fake_eff)*[False]
    tops = resolveOverlap(event, zinvOutput, discCut)
    topsHEP = resolveOverlapHEP(event, ZinvpassHEP)
    hnTopsZinv.Fill(len(tops), sampleWgt)
    hnTopsHEPZinv.Fill(len(topsHEP), sampleWgt)

    for j in xrange(len(cand_m)):
        hDiscNoMatch.Fill(zinvOutput[j], sampleWgt)
        if(cand_pt[j] > 250):
            hDiscNoMatchPt.Fill(zinvOutput[j], sampleWgt)
        if(zinvOutput[j] > discCut):
            numflag = True
        if(ZinvpassHEP[j]):
            numflagHEP = True
        for k in xrange(len(cut_fake_eff)):
            discpass = (zinvOutput[j]>cut_fake_eff[k] or zinvOutput[j]==cut_fake_eff[k]) if(k==0) else (zinvOutput[j]>cut_fake_eff[k])
            if(discpass):
                numflagroc[k] = True
        outputCount += 1
    if(numflag):
        hFakeNum.Fill(MET, sampleWgt)
        hFakeNum_njet.Fill(Njet, sampleWgt)
    if(numflagHEP):
        hFakeNumHEP.Fill(MET, sampleWgt)
        hFakeNumHEP_njet.Fill(Njet, sampleWgt)
        FakeNumrocHEP += 1*sampleWgt
    for k in xrange(len(cut_fake_eff)):
        if numflagroc[k]:
            FakeNumroc[k] += 1*sampleWgt


    #ROC info
    for i in xrange(len(cand_m)):
        rocInputZ.append(data[i])
        rocHEPZ.append(HEPReqs(event, i))
        evtwgtZ.append(sampleWgt)


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
    
    nprocInput = numpy.array(rocInput, dtype=numpy.float32)
    rocOutput = sess.run(y_train, feed_dict={x: nprocInput})[:,0]
    #rescale output to ensure we get the full ROC curve
    rocOutput = (rocOutput - rocOutput.min()) / rocOutput.ptp()

    for i in xrange(len(rocOutput)):
        if(rocScore[i]):
            Nmatch= Nmatch+1*evtwgt[i]
            if(rocHEP[i]):TPHEP = TPHEP+1*evtwgt[i]
        else:
            Nnomatch = Nnomatch+1*evtwgt[i]
            if(rocHEP[i]):FPHEP = FPHEP+1*evtwgt[i]

        for j in xrange(len(cut)):
            discpass = (rocOutput[i]>cut[j] or rocOutput[i]==cut[j]) if(j==0) else (rocOutput[i]>cut[j])
            if(discpass):
                if(rocScore[i]): TP[j] = TP[j]+1*evtwgt[i]
                else:            FP[j] = FP[j]+1*evtwgt[i]
    
    for j in xrange(len(cut)):
        TPR[j] = float(TP[j])/Nmatch
        FPR[j] = float(FP[j])/Nnomatch
        hroc.Fill(FPR[j],TPR[j])
    TPRHEP = float(TPHEP)/Nmatch
    FPRHEP = float(FPHEP)/Nnomatch
    hroc_HEP.Fill(FPRHEP,TPRHEP)
    
    #Zinv
    FPZ  = len(cut) * [0]
    FPRZ = len(cut) * [0]
    NnomatchZ = 0
    FPHEPZ =0
    FPRHEPZ =0
    nprocInputZ = numpy.array(rocInputZ, dtype=numpy.float32)
    rocOutputZ = sess.run(y_train, feed_dict={x: nprocInputZ})[:,0]
    #rescale output to ensure we get the full ROC curve
    rocOutputZ = (rocOutputZ - rocOutputZ.min()) / rocOutputZ.ptp()

    for i in xrange(len(rocOutputZ)):
        NnomatchZ = NnomatchZ+1*evtwgtZ[i]
        if(rocHEPZ[i]):FPHEPZ = FPHEPZ+1*evtwgtZ[i]
        for j in xrange(len(cut)):
            discpass = (rocOutputZ[i]>cut[j] or rocOutputZ[i]==cut[j]) if(j==0) else (rocOutputZ[i]>cut[j])
            if(discpass):
                FPZ[j] = FPZ[j]+1*evtwgtZ[i]
    for j in xrange(len(cut)):
        FPRZ[j] = float(FPZ[j])/NnomatchZ
        hrocZ.Fill(FPRZ[j],TPR[j])
    FPRHEPZ = float(FPHEPZ)/NnomatchZ
    hroc_HEPZ.Fill(FPRHEPZ,TPRHEP)
    
    
    for j in xrange(len(cut_fake_eff)):
        Effroc[j]  = float(EffNumroc[j])/EffDenroc
        Fakeroc[j] = float(FakeNumroc[j])/FakeDenroc
        hroc_alt.Fill(Fakeroc[j], Effroc[j])
        hEff_disc.Fill(cut_fake_eff[j],Effroc[j])
        hFake_disc.Fill(cut_fake_eff[j],Fakeroc[j])
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

hnMatchMVAcand.SetTitle("Const matched MVA disc passed cand")
hnMatchMVAcand.Draw()
c.Print("MatchedMVAcand.png")

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

#Writing histograms in a root file.
mf = ROOT.TFile('MVAOutput_tf.root','RECREATE')
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
