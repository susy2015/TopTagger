import TopTaggerInterface as tti

class Top:
    def __init__(self, pt, eta, phi, mass, disc, type, j1Idx = -999, j2Idx = -999, j13dx = -999):
        self.pt = pt
        self.eta = eta
        self.phi = phi
        self.mass = mass
        self.disc = disc
        self.type = type
        self.j1Idx = j1Idx
        self.j2Idx = j2Idx
        self.j3Idx = j3Idx

    def __str__(self):
        return "Top properties:   pt:  %7.3f,    eta:  %7.3f,    phi:  %7.3f,    mass:  %7.3f,    disc:  %7.3f,    type:  %3i"%(self.pt, self.eta, self.phi, self.mass, self.disc, self.type)

    def __repr__(self):
        return "Top(%f, %f, %f, %f, %f, %i, %i, %i, %i)"%(self.pt, self.eta, self.phi, self.mass, self.disc, self.type, self.j1Idx, self.j2Idx, self.j3Idx)

class TopTaggerResult:
    def __init__(self, results):
        self.floatVals = results[0]
        self.intVals = results[1]

    def __len__(self):
        return self.floatVals.shape[0]

    def __iter__(self):
        for variables in zip(self.ptCol(), self.etaCol(), self.phiCol(), self.massCol(), self.discCol(), self.typeCol(), self.j1IdxCol(), self.j2IdxCol(), self.j3IdxCol()):
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

    def j1IdxCol(self):
        return self.intVals[:, 1]

    def j2IdxCol(self):
        return self.intVals[:, 2]

    def j3IdxCol(self):
        return self.intVals[:, 3]

class TopTagger:

    def __init__(self, cfgFile, workingDir = ""):
        self.cfgFile = cfgFile
        self.workingDir = workingDir
        self.initialize()
        self.firstEvent = True

    def __enter__(self):
        self.initialize()
        return self

    def __del__(self):
        self.close()

    def __exit__(self, exc_type, exc_val, exc_tb):
        self.close()

    def initialize(self):
        if not hasattr(self, "tt"):
            self.tt = tti.setup(self.cfgFile, self.workingDir)

    def close(self):
        try:
            del self.tt
        except AttributeError:
            pass

    def run(self, *args, **kwargs):
        tti.run(self.tt, *args, **kwargs)
        results = tti.getResults(self.tt)
        return TopTaggerResult(results)

    def runFromNanoAOD(self, event, isFirstEvent = False):
        #This is a hack for the nanoAOD postprocessor to force it to read all necessary variables before passing them to C because each new branch accessed causes all branches to be reallocated 
        nHackLoop = 1
        if isFirstEvent:
            nHackLoop = 2

        for i in xrange(nHackLoop):

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

            nJets = event.nJet
            nElec = event.nElectron
            nMuon = event.nMuon
            ak4Inputs = (nJets, (event.Jet_pt, event.Jet_eta, event.Jet_phi, event.Jet_mass, nJets), event.Jet_btagCSVV2, supplementaryFloatVariables, supplementaryIntVariables, event.Jet_electronIdx1, event.Jet_muonIdx1, nElec, (event.Electron_pt, event.Electron_eta, event.Electron_phi, event.Electron_mass, nElec), event.Electron_vidNestedWPBitmap, event.Electron_miniPFRelIso_all, nMuon, (event.Muon_pt, event.Muon_eta, event.Muon_phi, event.Muon_mass, nMuon), None, event.Muon_miniPFRelIso_all)
            
            nFatJet = event.nFatJet
            nSubJet = event.nSubJet
            ak8Inputs = (nFatJet, (event.FatJet_pt, event.FatJet_eta, event.FatJet_phi, event.FatJet_mass, nFatJet), event.FatJet_msoftdrop, event.FatJet_deepTag_TvsQCD, event.FatJet_deepTag_WvsQCD, nSubJet, (event.SubJet_pt, event.SubJet_eta, event.SubJet_phi, event.SubJet_mass, nSubJet), event.FatJet_subJetIdx1, event.FatJet_subJetIdx2)

            nResCand = event.nResolvedTopCandidate
            resTopInputs = (nResCand, (event.ResolvedTopCandidate_pt, event.ResolvedTopCandidate_eta, event.ResolvedTopCandidate_phi, event.ResolvedTopCandidate_mass, nResCand), event.ResolvedTopCandidate_discriminator, event.ResolvedTopCandidate_j1Idx, event.ResolvedTopCandidate_j2Idx, event.ResolvedTopCandidate_j3Idx)

        results = self.run(ak4Inputs = ak4Inputs, resolvedTopInputs=resTopInputs, ak8Inputs=ak8Inputs)

        return results


if __name__ == "__main__":
    import ROOT
    import optparse

    def getTopsFromExampleFile(tt, event):
        supplementaryFloatVariables = {
            "qgPtD":                                event.ak4qgPtD,
            "qgAxis1":                              event.ak4qgAxis1,
            "qgAxis2":                              event.ak4qgAxis2,
            "qgMult":                               event.ak4qgMult,
            "recoJetschargedHadronEnergyFraction":  event.ak4recoJetschargedHadronEnergyFraction,
            "recoJetschargedEmEnergyFraction":      event.ak4recoJetschargedEmEnergyFraction,
            "recoJetsneutralEmEnergyFraction":      event.ak4recoJetsneutralEmEnergyFraction,
            "recoJetsmuonEnergyFraction":           event.ak4recoJetsmuonEnergyFraction,
            "recoJetsHFHadronEnergyFraction":       event.ak4recoJetsHFHadronEnergyFraction,
            "recoJetsHFEMEnergyFraction":           event.ak4recoJetsHFEMEnergyFraction,
            "recoJetsneutralEnergyFraction":        event.ak4recoJetsneutralEnergyFraction,
            "PhotonEnergyFraction":                 event.ak4PhotonEnergyFraction,
            "ElectronEnergyFraction":               event.ak4ElectronEnergyFraction,
            "ChargedHadronMultiplicity":            event.ak4ChargedHadronMultiplicity,
            "NeutralHadronMultiplicity":            event.ak4NeutralHadronMultiplicity,
            "PhotonMultiplicity":                   event.ak4PhotonMultiplicity,
            "ElectronMultiplicity":                 event.ak4ElectronMultiplicity,
            "MuonMultiplicity":                     event.ak4MuonMultiplicity,
            "DeepCSVb":                             event.ak4DeepCSVb,
            "DeepCSVc":                             event.ak4DeepCSVc,
            "DeepCSVl":                             event.ak4DeepCSVl,
            "DeepCSVbb":                            event.ak4DeepCSVbb,
            }
        
        supplementaryIntVariables = {
            }
        
        ak4LorentzVector = event.ak4jetsLVec
        ak4Inputs = (len(ak4LorentzVector), ak4LorentzVector, event.ak4recoJetsBtag, supplementaryFloatVariables, supplementaryIntVariables)
    
        tops = tt.run(ak4Inputs = ak4Inputs)

        return tops

    #Option parsing 
    parser = optparse.OptionParser()

    #Add command line options 
    parser.add_option ('-f', "--file",      dest='inputFile',  action='store',                          type=str, help="Input file")
    parser.add_option ('-b', "--tree",      dest='treeName',   action='store', default="Events",                  help="Name of TTree (Default: Events)")
    parser.add_option ('-c', "--taggerCfg", dest='taggerCfg',  action='store', default="TopTagger.cfg",           help="Name of Tagger config file name (Default: TopTagger.cfg)")
    parser.add_option ('-w', "--workDir",   dest='workDir',    action='store', default="",                        help="orking directory for top tagger config (usually where the cfg is located) (Default: .)")
    parser.add_option ('-n', "--nEvts",     dest='nEvts',      action='store', default=-1,              type=int, help="Number of events to run over (Default: all events)")
    parser.add_option ('-e', "--example",   dest='example',    action='store_true',                               help="Switch to read from exmaple file using std::vectors (Default: False")


    options, args = parser.parse_args()

    f = ROOT.TFile.Open(options.inputFile)

    tree = f.Get(options.treeName)

    tt = TopTagger(options.taggerCfg, options.workDir)

    for iEvt, event in enumerate(tree):
        if (options.nEvts > 0) and (iEvt + 1 > options.nEvts): break

        print "Event #:", iEvt + 1
    
        if options.example:
            tops = getTopsFromExampleFile(tt, event)
        else:
            tops = tt.runFromNanoAOD(event)

        print "\tN tops:", len(tops)

        for top in tops:
            print "\t%s"%str(top)
        print ""
    
