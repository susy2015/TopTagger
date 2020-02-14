import TopTaggerInterface as tti

class Top:
    def __init__(self, pt, eta, phi, mass, disc, type, j1Idx = -999, j2Idx = -999, j3Idx = -999, genMatch = -999, sf = -999.9, syst = None):
        self.pt = pt
        self.eta = eta
        self.phi = phi
        self.mass = mass
        self.disc = disc
        self.type = type
        self.j1Idx = j1Idx
        self.j2Idx = j2Idx
        self.j3Idx = j3Idx
        self.genMatch = genMatch
        self.sf = sf
        if syst:
            self.systs = syst
        else:
            self.systs = {}

    def __str__(self):
        return "Top properties:   pt:  %7.3f,    eta:  %7.3f,    phi:  %7.3f,    mass:  %7.3f,    disc:  %7.3f,    type:  %3i,    genMatch:  %3i"%(self.pt, self.eta, self.phi, self.mass, self.disc, self.type, self.genMatch)

    def __repr__(self):
        try:
            return "Top(%f, %f, %f, %f, %f, %i, %i, %i, %i)"%(self.pt, self.eta, self.phi, self.mass, self.disc, self.type, self.j1Idx, self.j2Idx, self.j3Idx, self.genMatch, self.systs, self.systs)
        except AttributeError:
            return "Top(%f, %f, %f, %f, %f, %i, %i, %i, %i)"%(self.pt, self.eta, self.phi, self.mass, self.disc, self.type, self.j1Idx, self.j2Idx, self.j3Idx, self.genMatch)

class TopTaggerResult:
    def __init__(self, results, sfAndSyst=None):
        self.floatVals = results[0]
        self.intVals = results[1]

        if sfAndSyst:
            self.sfVals = sfAndSyst[0]
            self.systVals = sfAndSyst[1]

    def __len__(self):
        return self.floatVals.shape[0]

    def __iter__(self):
        try:
            for variables in zip(self.ptCol(), self.etaCol(), self.phiCol(), self.massCol(), self.discCol(), self.typeCol(), self.j1IdxCol(), self.j2IdxCol(), self.j3IdxCol(), self.genMatchCol(), self.sfCol(), self.systCol()):
                yield Top(*variables)
        except AttributeError:
            for variables in zip(self.ptCol(), self.etaCol(), self.phiCol(), self.massCol(), self.discCol(), self.typeCol(), self.j1IdxCol(), self.j2IdxCol(), self.j3IdxCol(), self.genMatchCol()):
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

    def genMatchCol(self):
        return self.intVals[:, 4]

    def sfCol(self):
        try:
            return self.sfVals
        except AttributeError:
            raise AttributeError("No SF values defined")

    def systCol(self):
        try:
            return self.systVals
        except AttributeError:
            raise AttributeError("No systematic values defined")


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

    def run(self, saveCandidates = False, saveSFAndSyst = False, *args, **kwargs):
        tti.run(self.tt, *args, **kwargs)
        if saveCandidates:
            results = tti.getCandidates(self.tt)
            if saveSFAndSyst:
                sfAndSyst = tti.getCandidateSFSyst(self.tt)
                return TopTaggerResult(results, sfAndSyst)
        else:
            results = tti.getResults(self.tt)
            if saveSFAndSyst:
                sfAndSyst = tti.getSFSyst(self.tt)
                return TopTaggerResult(results, sfAndSyst)
        return TopTaggerResult(results)

    def runFromNanoAOD(self, event, isFirstEvent = False, saveSFAndSyst = False, saveCandidates = False):
        #This is a hack for the nanoAOD postprocessor to force it to read all necessary variables before passing them to C because each new branch accessed causes all branches to be reallocated 
        nHackLoop = 1
        if isFirstEvent:
            nHackLoop = 2

        isMC = hasattr(event, "nGenPart")

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
            ak4Inputs = (nJets, (event.Jet_pt, event.Jet_eta, event.Jet_phi, event.Jet_mass, nJets), event.Jet_btagDeepB, supplementaryFloatVariables, supplementaryIntVariables, event.Jet_electronIdx1, event.Jet_muonIdx1, nElec, (event.Electron_pt, event.Electron_eta, event.Electron_phi, event.Electron_mass, nElec), event.Electron_vidNestedWPBitmap, event.Electron_miniPFRelIso_all, nMuon, (event.Muon_pt, event.Muon_eta, event.Muon_phi, event.Muon_mass, nMuon), None, event.Muon_miniPFRelIso_all)
            
            nFatJet = event.nFatJet
            nSubJet = event.nSubJet
            ak8Inputs = (nFatJet, (event.FatJet_pt, event.FatJet_eta, event.FatJet_phi, event.FatJet_mass, nFatJet), event.FatJet_msoftdrop, event.FatJet_deepTag_TvsQCD, event.FatJet_deepTag_WvsQCD, nSubJet, (event.SubJet_pt, event.SubJet_eta, event.SubJet_phi, event.SubJet_mass, nSubJet), event.FatJet_subJetIdx1, event.FatJet_subJetIdx2)

            nResCand = event.nResolvedTopCandidate
            resTopInputs = (nResCand, (event.ResolvedTopCandidate_pt, event.ResolvedTopCandidate_eta, event.ResolvedTopCandidate_phi, event.ResolvedTopCandidate_mass, nResCand), event.ResolvedTopCandidate_discriminator, event.ResolvedTopCandidate_j1Idx, event.ResolvedTopCandidate_j2Idx, event.ResolvedTopCandidate_j3Idx)

            if isMC:
                nGenPart = event.nGenPart
                genInputs = (nGenPart, (event.GenPart_pt, event.GenPart_eta, event.GenPart_phi, event.GenPart_mass, nGenPart), event.GenPart_pdgId, event.GenPart_statusFlags, event.GenPart_genPartIdxMother)

        
        if isMC:
            results = self.run(ak4Inputs = ak4Inputs, saveCandidates=saveCandidates, resolvedTopInputs=resTopInputs, ak8Inputs=ak8Inputs, genInputs=genInputs, saveSFAndSyst = saveSFAndSyst)
        else:
            results = self.run(ak4Inputs = ak4Inputs, saveCandidates=saveCandidates, resolvedTopInputs=resTopInputs, ak8Inputs=ak8Inputs)

        return results


if __name__ == "__main__":
    import ROOT
    import optparse
    import uproot as up
    import numpy as np

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
    parser.add_option ('-f', "--file",           dest='inputFile',  action='store',                          type=str, help="Input file")
    parser.add_option ('-b', "--tree",           dest='treeName',   action='store', default="Events",                  help="Name of TTree (Default: Events)")
    parser.add_option ('-c', "--taggerCfg",      dest='taggerCfg',  action='store', default="TopTagger.cfg",           help="Name of Tagger config file name (Default: TopTagger.cfg)")
    parser.add_option ('-w', "--workDir",        dest='workDir',    action='store', default="",                        help="orking directory for top tagger config (usually where the cfg is located) (Default: .)")
    parser.add_option ('-n', "--nEvts",          dest='nEvts',      action='store', default=-1,              type=int, help="Number of events to run over (Default: all events)")
    parser.add_option ('-e', "--example",        dest='example',    action='store_true',                               help="Switch to read from exmaple file using std::vectors (Default: False")
    parser.add_option ('-s', "--selectEvents",   dest='selEvents',  action='store', default=None,            type=str, help="Run only over the selected event numbers (comma seperated list)")
    parser.add_option ('-v', "--eventBranch",    dest='evtBranch',  action='store', default="event",         type=str, help="Name of event number branch (Default: event")
    parser.add_option ('-d', "--saveCandidates", dest='saveCands',  action='store_true',                               help="Save Top/W candidates instead of final tops")


    options, args = parser.parse_args()

    f = ROOT.TFile.Open(options.inputFile)

    event = f.Get(options.treeName)

    tt = TopTagger(options.taggerCfg, options.workDir)

    eventList = []
    if options.selEvents:
        with up.open(options.inputFile) as f_up:
            eventNums = np.array([int(s) for s in options.selEvents.split(",")])
            events = f_up[options.treeName][options.evtBranch].array()
            for evtNum in eventNums:
                eventList.extend(np.where(events == evtNum))

    for iEvt in eventList if len(eventList) else xrange(event.GetEntries()):
        if (options.nEvts > 0) and (iEvt + 1 > options.nEvts): break

        event.GetEntry(iEvt)

        try:
            evtNum = getattr(event, options.evtBranch)
            print "Event #:", evtNum
        except AttributeError:
            print "Event #:", iEvt

        if options.example:
            tops = getTopsFromExampleFile(tt, event)
        else:
            tops = tt.runFromNanoAOD(event, saveCandidates=options.saveCands)

        print "\tN tops:", len(tops)

        for top in tops:
            print "\t%s"%str(top)
        print ""
    
