import TopTaggerInterface as tti

class Top:
    def __init__(self, pt, eta, phi, mass, disc, type):
        self.pt = pt
        self.eta = eta
        self.phi = phi
        self.mass = mass
        self.disc = disc
        self.type = type

class TopTaggerResult:
    def __init__(self, results):
        self.floatVals = results[0]
        self.intVals = results[1]

    def __len__(self):
        return self.floatVals.shape[0]

    def __iter__(self):
        for variables in zip(self.ptCol(), self.etaCol(), self.phiCol(), self.massCol(), self.discCol(), self.typeCol()):
            yield Top(*variables)

    def ptCol(self):
        return self.floatVals[:, 0]

    def etaCol(self):
        return self.floatVals[:, 1]

    def phiCol(self):
        return self.floatVals[:, 2]

    def massCol(self):
        return self.floatVals[:, 3]

    def discCol(self):
        return self.floatVals[:, 4]

    def typeCol(self):
        return self.intVals[:, 0]

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
        results = tti.getResults(self.tt)
        return TopTaggerResult(results)



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
    
        print "\tN tops:", len(tops)
    
        for top in tops:
            print "\tTop properties: Type: %3d,   Pt: %6.1lf,   Eta: %7.3lf,   Phi: %7.3lf,   M: %7.3lf,   Disc: %7.3f"%(top.type, top.pt, top.eta, top.phi, top.mass, top.disc)
        print ""

