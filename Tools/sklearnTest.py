import ROOT
import numpy
import math
#import matplotlib.pyplot as plt
from sklearn.ensemble import RandomForestClassifier
from sklearn.ensemble import RandomForestRegressor
import sklearn.tree as tree
from sklearn.tree import DecisionTreeClassifier
from sklearn.tree import DecisionTreeRegressor
from sklearn import svm
from sklearn.ensemble import AdaBoostRegressor
from sklearn.ensemble import GradientBoostingRegressor



class DataGetter:

    def __init__(self):
        self.list = ["cand_m", "cand_dRMax", "cand_pt", "j12_m", "j13_m", "j23_m", "dR12", "dR23", "dR13", "j1_pt", "j2_pt", "j3_pt", "j1_CSV", "j2_CSV", "j3_CSV", "j1_QGL", "j2_QGL", "j3_QGL", "j12j3_dR", "j13j2_dR", "j23j1_dR"]
        #["cand_m", "cand_dRMax", "cand_pt", "j12_m", "j13_m", "j23_m", "dR12", "dR23", "dR13", "j1_pt", "j2_pt", "j3_pt", "j1_m", "j2_m", "j3_m", "j1_CSV", "j2_CSV", "j3_CSV", "j1_QGL", "j2_QGL", "j3_QGL", "j12j3_dR", "j13j2_dR", "j23j1_dR"]
        self.list2 = ["event." + v + "[i]" for v in self.list]
        self.theStrCommand = "[" + ", ".join(self.list2) + "]"

    def getData(self, event, i):
        return eval(self.theStrCommand)

    def getList(self):
        return self.list

#def getData(event, i):
#    return [event.cand_m[i], event.cand_dRMax[i], event.cand_pt[i], event.j12_m[i], event.j13_m[i], event.j23_m[i], event.dR12[i], event.dR23[i], event.dR13[i], event.j1_pt[i], event.j2_pt[i], event.j3_pt[i], event.j1_m[i], event.j2_m[i], event.j3_m[i], event.j1_CSV[i], event.j2_CSV[i], event.j3_CSV[i], event.j12j3_dR[i], event.j13j2_dR[i], event.j23j1_dR[i]]


#Variable histo declaration  
dg = DataGetter()

histranges = {"cand_m":[20, 50, 300], 
              "cand_dRMax":[50,0,5],
              "cand_pt":[50,0,1000],
              "j1_m":[100, 0, 100],
              "j23_m":[100, 0, 250],
              "j23j1_dR":[50,0,5],
              "j12_m":[100, 0, 250],
              "j13_m":[100, 0, 250],
              "dR12":[50,0,5],
              "dR23":[50,0,5],
              "dR13":[50,0,5],
              "j1_pt":[50,0,1000],
              "j2_pt":[50,0,1000],
              "j3_pt":[50,0,1000],
              "j2_m":[100, 0, 250],
              "j3_m":[100, 0, 250],
              "j1_CSV":[50, 0, 1],
              "j2_CSV":[50, 0, 1],
              "j3_CSV":[50, 0, 1],
              "j12j3_dR":[50,0,5],
              "j13j2_dR":[50,0,5],
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
    Rmin_ = 0.85
    Rmax_ = 1.15
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

    return passHEPRequirments + passBreq


print "PROCESSING TRAINING DATA"

file = ROOT.TFile.Open("trainingTuple_division_0_TTbarSingleLep.root")

hPtMatch   = ROOT.TH1D("hPtMatch", "hPtMatch", 50, 0.0, 2000.0)
hPtNoMatch = ROOT.TH1D("hPtNoMatch", "hPtNoMatch", 50, 0.0, 2000.0)

for event in file.slimmedTuple:
    for i in xrange(len(event.genConstiuentMatchesVec)):
        if event.genConstiuentMatchesVec[i] == 3:
            hPtMatch.Fill(event.cand_pt[i])
        else:
            hPtNoMatch.Fill(event.cand_pt[i])

NSig = 0.0#hPtMatch.Integral()
NBg = 0.0#hPtNoMatch.Integral()

#hPtMatch.Scale(1/hPtMatch.Integral())
#hPtNoMatch.Scale(1/hPtNoMatch.Integral())

hwMatch   = ROOT.TH1D("hwMatch",   "hwMatch",   1000, 0.0, 1.0)
hwNoMatch = ROOT.TH1D("hwNoMatch", "hwNoMatch", 1000, 0.0, 1.0)

inputData = []
inputAnswer = []
inputWgts = []
for event in file.slimmedTuple:
    for i in xrange(len(event.cand_m)):
        #if(event.cand_pt[i] > 150):
            inputData.append(dg.getData(event, i))
            nmatch = event.genConstiuentMatchesVec[i]
            inputAnswer.append(int(nmatch == 3))
            if nmatch == 3:
                inputWgts.append(1.0 / hPtMatch.GetBinContent(hPtMatch.FindBin(event.cand_pt[i])))
                NSig += inputWgts[-1]
                hwMatch.Fill(inputWgts[-1])
            else:
                inputWgts.append(1.0 / hPtNoMatch.GetBinContent(hPtNoMatch.FindBin(event.cand_pt[i])))
                NBg += inputWgts[-1]
                hwNoMatch.Fill(inputWgts[-1])
    

print NSig, NBg

npyInputData = numpy.array(inputData, numpy.float32)
npyInputAnswer = numpy.array(inputAnswer, numpy.float32)
npyInputWgts = numpy.array(inputWgts, numpy.float32)

print "TRAINING MVA"

#clf = RandomForestClassifier(n_estimators=10)
clf = RandomForestRegressor(n_estimators=100, n_jobs=4)
#clf = AdaBoostRegressor(n_estimators=100)
#clf = GradientBoostingRegressor(n_estimators=100, learning_rate=0.1, random_state=0, loss='ls')
#clf = DecisionTreeRegressor()
#clf = DecisionTreeClassifier()
#clf = svm.SVC()

clf = clf.fit(npyInputData, npyInputAnswer, npyInputWgts)




fileValidation = ROOT.TFile.Open("trainingTuple_division_1_TTbarSingleLep.root")

print "PROCESSING VALIDATION DATA"

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

discCut = 0.15

inputList = []
truth = []
genPt = []
recoPt = []
passHEP = []
rocScore = []

for event in fileValidation.slimmedTuple:
    for pt in event.genTopPt:
        hEffDen.Fill(pt)
        hEffHEPDen.Fill(pt)
    for i in xrange(len(event.cand_m)):
        recoPt.append(event.cand_pt[i])
        inputList.append(dg.getData(event, i))
        truth.append(event.genConstiuentMatchesVec[i])
        rocScore.append((event.genConstiuentMatchesVec[i]==3))
        genPt.append(event.genConstMatchGenPtVec[i])
        passHEP.append(HEPReqs(event, i))
        Varsval = dg.getData(event, i)
        for j in xrange(len(Varsval)):
            varsmap[varsname[j]].append(Varsval[j])

print "CALCULATING DISCRIMINATORS"

output = clf.predict(inputList)

print "FILLING HISTOGRAMS"

for i in xrange(len(output)):
    hDisc.Fill(output[i])
    if(output[i] > discCut):
        hPurityDen.Fill(recoPt[i])
        hNConstMatchTag.Fill(truth[i])
        for vname in varsname:
            if(histranges.has_key(vname)) : hist_tag[vname].Fill(varsmap[vname][i])
    else:
        hNConstMatchNoTag.Fill(truth[i])
        for vname in varsname:
            if(histranges.has_key(vname)) : hist_notag[vname].Fill(varsmap[vname][i])
    if(passHEP[i]):
        hPurityHEPDen.Fill(recoPt[i])
        hNConstMatchTagHEP.Fill(truth[i])
    else:
        hNConstMatchNoTagHEP.Fill(truth[i])
    #Truth matched candidates
    if(truth[i] == 3):
        #pass reco discriminator threshold 
        if(output[i] > discCut):
            hEffNum.Fill(genPt[i])
            hPurityNum.Fill(recoPt[i])
        if(passHEP[i]):
            hEffHEPNum.Fill(genPt[i])
            hPurityHEPNum.Fill(recoPt[i])
        hDiscMatch.Fill(output[i])
        if(recoPt[i] > 250):
            hDiscMatchPt.Fill(output[i])
    #not truth matched 
    else:
        hDiscNoMatch.Fill(output[i])
        if(recoPt[i] > 250):
            hDiscNoMatchPt.Fill(output[i])


print "PLOTTING"

c = ROOT.TCanvas("c1","c1",800,800)

#Random plots

hwMatch.SetLineColor(ROOT.kRed)
hwMatch.Draw()
hwNoMatch.Draw("same")
c.SetLogx()
c.SetLogy()
c.Print("weights.png")
c.SetLogx(False)
c.SetLogy(False)

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
hEffNum.SetStats(0)
hEffNum.SetTitle("")
hEffNum.GetXaxis().SetTitle("gen top Pt [GeV]")
hEffNum.GetYaxis().SetTitle("Efficiency")
hEffNum.GetYaxis().SetRangeUser(0, 1)

hEffNum.Divide(hEffDen)
hEffHEPNum.Divide(hEffHEPDen)

hEffNum.SetLineColor(ROOT.kRed)
hEffHEPNum.SetLineColor(ROOT.kBlue)

leg = ROOT.TLegend(0.55, 0.75, 0.9, 0.9)
leg.AddEntry(hEffNum, "MVA")
leg.AddEntry(hEffHEPNum, "HEP")

hEffNum.Draw()
hEffHEPNum.Draw("same")
leg.Draw("same")
c.Print("efficiency.png")

#draw purity

leg = ROOT.TLegend(0.55, 0.75, 0.9, 0.9)
leg.AddEntry(hNConstMatchTag, "Top Tagged (MVA)")
leg.AddEntry(hNConstMatchNoTag, "Not Top Tagged (MVA)")
leg.AddEntry(hNConstMatchTagHEP, "Top Tagged (HEP)")
leg.AddEntry(hNConstMatchNoTagHEP, "Not Top Tagged (HEP)")

hPurityNum.SetStats(0)
hPurityNum.SetTitle("")
hPurityNum.GetXaxis().SetTitle("reco top Pt [GeV]")
hPurityNum.GetYaxis().SetTitle("Purity")
hPurityNum.GetXaxis().SetTitle("reco top Pt [GeV]")
hPurityNum.GetYaxis().SetRangeUser(0, 1)

hPurityNum.SetLineColor(ROOT.kRed)
hPurityHEPNum.SetLineColor(ROOT.kBlue)

leg = ROOT.TLegend(0.55, 0.75, 0.9, 0.9)
leg.AddEntry(hPurityNum, "MVA")
leg.AddEntry(hPurityHEPNum, "HEP")


hPurityNum.Divide(hPurityDen)
hPurityHEPNum.Divide(hPurityHEPDen)
hPurityNum.Draw()
hPurityHEPNum.Draw("same")
leg.Draw("same")
c.Print("purity.png")

#with open("iris.dot", 'w') as f:
#    f = tree.export_graphviz(clf, out_file=f,
#                             feature_names = ["cand_m", "cand_pt", "cand_dRMax"],
#                             class_names = ["background", "top"],
#                             filled=True, rounded=True)

# Plot feature importance
if isinstance(clf, GradientBoostingRegressor):
    feature_importance = clf.feature_importances_
    # make importances relative to max importance
    feature_importance = 100.0 * (feature_importance / feature_importance.max())
    sorted_idx = numpy.argsort(feature_importance)
    pos = numpy.arange(sorted_idx.shape[0]) + .5
    #plt.subplot(1, 2, 2)
    #plt.barh(pos, feature_importance[sorted_idx], align='center')
    #plt.yticks(pos, boston.feature_names[sorted_idx])
    #plt.xlabel('Relative Importance')
    #plt.title('Variable Importance')
    #plt.show()
    
    print zip(dg.getList(), feature_importance)
    print sorted_idx

print "DONE!"

#Writing histograms in a root file.
hEff = hEffNum.Clone("hEff")
hEffHEP = hEffHEPNum.Clone("hEffHEP")
hPurity = hPurityNum.Clone("hPurity")
hPurityHEP = hPurityHEPNum.Clone("hPurityHEP")

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
mf.Write()
mf.Close()
