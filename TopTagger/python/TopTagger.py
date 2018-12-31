import TopTaggerInterface as tti

class TopTagger:
    def __init__(self, cfgFile, workingDir = ""):
        self.tt = None
        self.cfgFile = cfgFile
        self.workingDir = workingDir
        self.initialize()

    def __enter__(self):
        self.initialize()
        return self

    def __del__(self):
        self.close()

    def __exit__(self, exc_type, exc_val, exc_tb):
        self.close()

    def initialize(self):
        if not self.tt:
            self.tt = tti.setup(self.cfgFile, self.workingDir)

    def close(self):
        if self.tt != None:
            del self.tt
            self.tt = None

    def run(self, jet_pt, jet_eta, jet_phi, jet_mass, jet_btag, floatVars, intVars):
        tti.run(self.tt, jet_pt, jet_eta, jet_phi, jet_mass, jet_btag, floatVars, intVars)
        return tti.getResults(self.tt)



if __name__ == "__main__":
    import ROOT

    f = ROOT.TFile.Open("/home/pastika/topTagger/prod2017MC_NANO.root")

    tree = f.Get("Events")

    tt = TopTagger("TopTagger.cfg", "../../..")

    for i, event in enumerate(tree):
        if i + 1 > 100: break

        print "Event #:", i + 1

        supplementaryFloatVariables = {
            "qgPtD":                                event.Jet_qgptD,
            "qgAxis1":                              event.Jet_qgAxis1,
            "qgAxis2":                              event.Jet_qgAxis2,
            "recoJetschargedHadronEnergyFraction":  event.Jet_chHEF,
            "recoJetschargedEmEnergyFraction":      event.Jet_chEmEF,
            "recoJetsneutralEmEnergyFraction":      event.Jet_neEmEF,
            "recoJetsmuonEnergyFraction":           event.Jet_muEF,
            "recoJetsHFHadronEnergyFraction":       event.Jet_hfHadEF,
            "recoJetsHFEMEnergyFraction":           event.Jet_hfEMEF,
            "recoJetsneutralEnergyFraction":        event.Jet_neHEF,
            "PhotonEnergyFraction":                 event.Jet_phEF,
            "ElectronEnergyFraction":               event.Jet_elEF,
            "ChargedHadronMultiplicity":            event.Jet_chHadMult,
            "NeutralHadronMultiplicity":            event.Jet_neHadMult,
            "PhotonMultiplicity":                   event.Jet_phMult,
            "ElectronMultiplicity":                 event.Jet_elMult,
            "MuonMultiplicity":                     event.Jet_muMult,
            "DeepCSVb":                             event.Jet_deepCSVb,
            "DeepCSVc":                             event.Jet_deepCSVc,
            "DeepCSVl":                             event.Jet_deepCSVudsg,
            "DeepCSVbb":                            event.Jet_deepCSVbb,
        }
    
        supplementaryIntVariables = {
            "qgMult":                               event.Jet_qgMult,
        }
        
        tops = tt.run(event.Jet_pt, event.Jet_eta, event.Jet_phi, event.Jet_mass, event.Jet_btagCSVV2, supplementaryFloatVariables, supplementaryIntVariables)
    
        print "\tN tops:", tops.shape[0]
    
        for top in tops:
            print "\tTop properties: Type: %3d,   Pt: %6.1lf,   Eta: %7.3lf,   Phi: %7.3lf,   M: %7.3lf,   Disc: %7.3f"%(3, top[0], top[1], top[2], top[3], top[4])
        print ""


