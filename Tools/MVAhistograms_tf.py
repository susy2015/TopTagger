import ROOT
from MVAcommon_tf import DataGetter

histranges = {"cand_pt":[50, 0, 1000], 
              "cand_m":[20, 50, 300], 
#              "cand_dRMax":[50,0,5],
              "cand_pt":[50,0,1000],
              "j1_m":[100, 0, 100],
              "j23_m":[100, 0, 250],
              "j12_m":[100, 0, 250],
              "j13_m":[100, 0, 250],
              "dTheta12":[50,0,4],
              "dTheta23":[50,0,4],
              "dTheta13":[50,0,4],
              "j1_p":[50,0,1000],
              "j2_p":[50,0,1000],
              "j3_p":[50,0,1000],
              "j2_m":[100, 0, 250],
              "j3_m":[100, 0, 250],
              "j1_CSV":[50, 0, 1],
              "j2_CSV":[50, 0, 1],
              "j3_CSV":[50, 0, 1],
#              "j1_theta":[50, 0, 4],
#              "j2_theta":[50, 0, 4],
#              "j3_theta":[50, 0, 4],
#              "j12_dTheta":[50, 0, 4],
#              "j23_dTheta":[50, 0, 4],
#              "j13_dTheta":[50, 0, 4],
              "j1_QGL":[50, 0, 1],
              "j2_QGL":[50, 0, 1],
              "j3_QGL":[50, 0, 1],
              "j1_Chrg":[6, -1, 1],
              "j2_Chrg":[6, -1, 1],
              "j3_Chrg":[6, -1, 1]}
hist_tag = {}
hist_notag = {}

dg = DataGetter()

varsname = dg.getList()

for var in dg.getList():
    if(histranges.has_key(var)) : 
        hist_tag[var] = ROOT.TH1D(var+"_tag", var+"_tag", histranges[var][0], histranges[var][1], histranges[var][2])
        hist_notag[var] = ROOT.TH1D(var+"_notag", var+"_notag", histranges[var][0], histranges[var][1], histranges[var][2])

hEffNum = ROOT.TH1D("hEffNum", "hEffNum", 25, 0.0, 1000.0)
hEffDen = ROOT.TH1D("hEffDen", "hEffDen", 25, 0.0, 1000.0)
hPurityNum = ROOT.TH1D("hPurityNum", "hPurityNum", 25, 0.0, 1000.0)
hPurityDen = ROOT.TH1D("hPurityDen", "hPurityDen", 25, 0.0, 1000.0)
hPurity_discNum = ROOT.TH1D("hPurity_discNum", "hPurity_disc", 20, 0.0, 1.0)
hPurity_discDen = ROOT.TH1D("hPurity_discDen", "hPurity_disc", 20, 0.0, 1.0)

hEffHEPNum = ROOT.TH1D("hEffHEPNum", "hEffHEPNum", 25, 0.0, 1000.0)
hEffHEPDen = ROOT.TH1D("hEffHEPDen", "hEffHEPDen", 25, 0.0, 1000.0)
hPurityHEPNum = ROOT.TH1D("hPurityHEPNum", "hPurityHEPNum", 25, 0.0, 1000.0)
hPurityHEPDen = ROOT.TH1D("hPurityHEPDen", "hPurityHEPDen", 25, 0.0, 1000.0)

hDisc = ROOT.TH1D("disc", "disc", 100, 0, 1.0)
hDiscMatch = ROOT.TH1D("discMatch", "discMatch", 100, 0, 1.0)
hDiscMatch.SetLineColor(ROOT.kRed)
hDiscNoMatch = ROOT.TH1D("discNoMatch", "discNoMatch", 100, 0, 1.0)
hDiscNoMatch.SetLineColor(ROOT.kBlue)
hDiscMatchPt = ROOT.TH1D("discMatchPt", "discMatchPt", 100, 0, 1.0)
hDiscMatchPt.SetLineColor(ROOT.kRed)
hDiscMatchPt.SetLineStyle(ROOT.kDashed)
hDiscNoMatchPt = ROOT.TH1D("discNoMatchPt", "discNoMatchPt", 100, 0, 1.0)
hDiscNoMatchPt.SetLineColor(ROOT.kBlue)
hDiscNoMatchPt.SetLineStyle(ROOT.kDashed)
hMVAdisc_pt = ROOT.TH2D("hMVAdisc_pt", "disc vs cand pt", 25, 0.0, 1000.0, 100, 0, 1.0)

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
hj1Theta = ROOT.TH1D("hj1Theta", "cos#theta(topboost, j1)", 50, -1 , 1)
hj2Theta = ROOT.TH1D("hj2Theta", "cos#theta(topboost, j2)", 50, -1 , 1)
hj3Theta = ROOT.TH1D("hj3Theta", "cos#theta(topboost, j3)", 50, -1 , 1)

hnTops = ROOT.TH1D("hnTop", "hnTop", 10, 0, 10)
hnTopsHEP = ROOT.TH1D("hnTopHEP", "hnTopHEP", 10, 0, 10)
hnMVAcand = ROOT.TH1D("hnMVAcand", "hnMVAcand", 10, 0, 10)
hnMatchMVAcand = ROOT.TH1D("hnMatchMVAcand", "hnMatchMVAcand", 10, 0, 10)
hnHEPcand = ROOT.TH1D("hnHEPcand", "hnHEPcand", 10, 0, 10)
hnTopsZinv = ROOT.TH1D("hnTopZinv", "hnTopZinv", 10, 0, 10)
hnTopsHEPZinv = ROOT.TH1D("hnTopHEPZinv", "hnTopHEPZinv", 10, 0, 10)
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
hroc = ROOT.TProfile("hroc", "hroc", 1000, 0, 1, 0, 1)
hroc_HEP = ROOT.TProfile("hroc_HEP", "hroc_HEP", 100, 0, 1, 0, 1)
#Zinv
hrocZ = ROOT.TProfile("hrocZ", "hrocZ", 1000, 0, 1, 0, 1)
hroc_HEPZ = ROOT.TProfile("hroc_HEPZ", "hroc_HEPZ", 1000, 0, 1, 0, 1)

hroc_alt = ROOT.TProfile("hroc_alt", "hroc_alt", 1000, 0, 1, 0, 1)
hroc_HEP_alt = ROOT.TProfile("hroc_HEP_alt", "hroc_HEP_alt", 1000, 0, 1, 0, 1)

#Eff vs disc value
hEff_disc = ROOT.TProfile("hEff_disc", "hEff_disc", 100, 0, 1, 0, 1)

#Fakerate vs disc value
hFake_disc = ROOT.TProfile("hFake_disc", "hFake_disc", 100, 0, 1, 0, 1)
