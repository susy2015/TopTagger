import ROOT
ROOT.PyConfig.IgnoreCommandLineOptions = True
import math
from collections import defaultdict
from itertools import permutations
import numpy as np
import itertools

from PhysicsTools.NanoAODTools.postprocessing.framework.datamodel import Collection, Object
from PhysicsTools.NanoAODTools.postprocessing.framework.eventloop import Module

from TopTagger import TopTagger

class TopTaggerProducer(Module):
    def __init__(self, cfgName="TopTagger.cfg", cfgWD=".", suffix=None, saveAK8=False, AK4JetInputs = ("Jet_pt", "Jet_eta", "Jet_phi", "Jet_mass"), recalculateFromRawInputs = False, useAK8 = False, doLepCleaning = True, saveCandidates = True, topDiscCut=None):
        self.topTaggerCfg = cfgName
        self.topTaggerWD = cfgWD
        self.saveAK8 = saveAK8
        self.AK4JetInputs = AK4JetInputs
        self.recalculateFromRawInputs = recalculateFromRawInputs
        self.useAK8 = useAK8
        self.doLepCleaning = doLepCleaning
        self.saveCandidates = saveCandidates
        self.topDiscCut = topDiscCut

        self.tt = TopTagger(self.topTaggerCfg, self.topTaggerWD)

        if suffix:
            if len(suffix) > 0 and suffix[0] != "_":
                self.suffix = "_" + suffix
            else:
                self.suffix = suffix
        else:
            self.suffix = ""

        if self.saveCandidates:
            self.suffixResolved = "Candidate" + self.suffix

    def beginJob(self):
        pass

    def endJob(self):
        pass

    def beginFile(self, inputFile, outputFile, inputTree, wrappedOutputTree):
        self.isFirstEventOfFile = True
        self.out = wrappedOutputTree
        self.out.branch("ResolvedTop%s_pt"%self.suffixResolved, "F",            lenVar="nResolvedTop%s"%self.suffixResolved, limitedPrecision=12)
        self.out.branch("ResolvedTop%s_eta"%self.suffixResolved, "F",           lenVar="nResolvedTop%s"%self.suffixResolved, limitedPrecision=12)
        self.out.branch("ResolvedTop%s_phi"%self.suffixResolved, "F",           lenVar="nResolvedTop%s"%self.suffixResolved, limitedPrecision=12)
        self.out.branch("ResolvedTop%s_mass"%self.suffixResolved, "F",          lenVar="nResolvedTop%s"%self.suffixResolved, limitedPrecision=12)
        self.out.branch("ResolvedTop%s_discriminator"%self.suffixResolved, "F", lenVar="nResolvedTop%s"%self.suffixResolved, limitedPrecision=12)
        self.out.branch("ResolvedTop%s_j1Idx"%self.suffixResolved, "I",         lenVar="nResolvedTop%s"%self.suffixResolved)
        self.out.branch("ResolvedTop%s_j2Idx"%self.suffixResolved, "I",         lenVar="nResolvedTop%s"%self.suffixResolved)
        self.out.branch("ResolvedTop%s_j3Idx"%self.suffixResolved, "I",         lenVar="nResolvedTop%s"%self.suffixResolved)

        if self.saveAK8:
            self.out.branch("MergedTop%s_pt"%self.suffix, "F",              lenVar="nMergedTop%s"%self.suffix, limitedPrecision=12)
            self.out.branch("MergedTop%s_eta"%self.suffix, "F",             lenVar="nMergedTop%s"%self.suffix, limitedPrecision=12)
            self.out.branch("MergedTop%s_phi"%self.suffix, "F",             lenVar="nMergedTop%s"%self.suffix, limitedPrecision=12)
            self.out.branch("MergedTop%s_mass"%self.suffix, "F",            lenVar="nMergedTop%s"%self.suffix, limitedPrecision=12)
            self.out.branch("MergedTop%s_discriminator"%self.suffix, "F",   lenVar="nMergedTop%s"%self.suffix, limitedPrecision=12)
    
            self.out.branch("MergedW%s_pt"%self.suffix, "F",            lenVar="nMergedW%s"%self.suffix, limitedPrecision=12)
            self.out.branch("MergedW%s_eta"%self.suffix, "F",           lenVar="nMergedW%s"%self.suffix, limitedPrecision=12)
            self.out.branch("MergedW%s_phi"%self.suffix, "F",           lenVar="nMergedW%s"%self.suffix, limitedPrecision=12)
            self.out.branch("MergedW%s_mass"%self.suffix, "F",          lenVar="nMergedW%s"%self.suffix, limitedPrecision=12)
            self.out.branch("MergedW%s_discriminator"%self.suffix, "F", lenVar="nMergedW%s"%self.suffix, limitedPrecision=12)

    def endFile(self, inputFile, outputFile, inputTree, wrappedOutputTree):
        pass

    def runFromNanoAOD(self, event):
        #This is a hack for the nanoAOD postprocessor to force it to read all necessary variables before passing them to C because each new branch accessed causes all branches to be reallocated 
        nHackLoop = 1
        if self.isFirstEventOfFile:
            nHackLoop = 2

        for i in xrange(nHackLoop):

            if self.recalculateFromRawInputs:
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
            else:
                supplementaryFloatVariables = {}
                supplementaryIntVariables = {}

            nJets = event.nJet
            jetInputs = (getattr(event, self.AK4JetInputs[0]), getattr(event, self.AK4JetInputs[1]), getattr(event, self.AK4JetInputs[2]), getattr(event, self.AK4JetInputs[3]), nJets)

            if self.doLepCleaning:
                nElec = event.nElectron
                nMuon = event.nMuon
                ak4Inputs = (nJets, jetInputs, event.Jet_btagCSVV2, supplementaryFloatVariables, supplementaryIntVariables, event.Jet_electronIdx1, event.Jet_muonIdx1, nElec, (event.Electron_pt, event.Electron_eta, event.Electron_phi, event.Electron_mass, nElec), event.Electron_vidNestedWPBitmap, event.Electron_miniPFRelIso_all, nMuon, (event.Muon_pt, event.Muon_eta, event.Muon_phi, event.Muon_mass, nMuon), None, event.Muon_miniPFRelIso_all)
            else:
                ak4Inputs = (nJets, jetInputs, event.Jet_btagCSVV2, supplementaryFloatVariables, supplementaryIntVariables)
            
            if self.useAK8:
                nFatJet = event.nFatJet
                nSubJet = event.nSubJet
                ak8Inputs = (nFatJet, (event.FatJet_pt, event.FatJet_eta, event.FatJet_phi, event.FatJet_mass, nFatJet), event.FatJet_msoftdrop, event.FatJet_deepTag_TvsQCD, event.FatJet_deepTag_WvsQCD, nSubJet, (event.SubJet_pt, event.SubJet_eta, event.SubJet_phi, event.SubJet_mass, nSubJet), event.FatJet_subJetIdx1, event.FatJet_subJetIdx2)
            else:
                ak8Inputs = None

            if not self.recalculateFromRawInputs:
                nResCand = event.nResolvedTopCandidate
                resTopInputs = (nResCand, (event.ResolvedTopCandidate_pt, event.ResolvedTopCandidate_eta, event.ResolvedTopCandidate_phi, event.ResolvedTopCandidate_mass, nResCand), event.ResolvedTopCandidate_discriminator, event.ResolvedTopCandidate_j1Idx, event.ResolvedTopCandidate_j2Idx, event.ResolvedTopCandidate_j3Idx)
            else:
                resTopInputs = None

        if ak8Inputs and resTopInputs:
            return self.tt.run(ak4Inputs = ak4Inputs, resolvedTopInputs=resTopInputs, ak8Inputs=ak8Inputs, saveCandidates=self.saveCandidates)
        elif ak8Inputs and not resTopInputs:
            return self.tt.run(ak4Inputs = ak4Inputs, ak8Inputs=ak8Inputs, saveCandidates=self.saveCandidates)
        elif not ak8Inputs and resTopInputs:
            return self.tt.run(ak4Inputs = ak4Inputs, resolvedTopInputs=resTopInputs, saveCandidates=self.saveCandidates)
        else:
            return self.tt.run(ak4Inputs = ak4Inputs, saveCandidates=self.saveCandidates)

    def analyze(self, event):
        """process event, return True (go to next module) or False (fail, go to next event)"""
        ## Process event

        ttr = self.runFromNanoAOD(event)
        self.isFirstEventOfFile = False

        resolvedFilter = ttr.typeCol() == 3
        mergedFilter = ttr.typeCol() == 1
        WFilter = ttr.typeCol() == 4

        if self.topDiscCut:
            resolvedFilter = resolvedFilter & (ttr.discCol() > self.topDiscCut)

        ### Store output
        self.out.fillBranch("ResolvedTop%s_pt"%self.suffixResolved,            ttr.ptCol()[resolvedFilter])
        self.out.fillBranch("ResolvedTop%s_eta"%self.suffixResolved,           ttr.etaCol()[resolvedFilter])
        self.out.fillBranch("ResolvedTop%s_phi"%self.suffixResolved,           ttr.phiCol()[resolvedFilter])
        self.out.fillBranch("ResolvedTop%s_mass"%self.suffixResolved,          ttr.massCol()[resolvedFilter])
        self.out.fillBranch("ResolvedTop%s_discriminator"%self.suffixResolved, ttr.discCol()[resolvedFilter])
        self.out.fillBranch("ResolvedTop%s_j1Idx"%self.suffixResolved,         ttr.j1IdxCol()[resolvedFilter].astype(int))
        self.out.fillBranch("ResolvedTop%s_j2Idx"%self.suffixResolved,         ttr.j2IdxCol()[resolvedFilter].astype(int))
        self.out.fillBranch("ResolvedTop%s_j3Idx"%self.suffixResolved,         ttr.j3IdxCol()[resolvedFilter].astype(int))

        if self.saveAK8:
            self.out.fillBranch("MergedTop%s_pt"%self.suffix,              ttr.ptCol()[mergedFilter])
            self.out.fillBranch("MergedTop%s_eta"%self.suffix,             ttr.etaCol()[mergedFilter])
            self.out.fillBranch("MergedTop%s_phi"%self.suffix,             ttr.phiCol()[mergedFilter])
            self.out.fillBranch("MergedTop%s_mass"%self.suffix,            ttr.massCol()[mergedFilter])
            self.out.fillBranch("MergedTop%s_discriminator"%self.suffix,   ttr.discCol()[mergedFilter])
    
            self.out.fillBranch("MergedW%s_pt"%self.suffix,                ttr.ptCol()[WFilter])
            self.out.fillBranch("MergedW%s_eta"%self.suffix,               ttr.etaCol()[WFilter])
            self.out.fillBranch("MergedW%s_phi"%self.suffix,               ttr.phiCol()[WFilter])
            self.out.fillBranch("MergedW%s_mass"%self.suffix,              ttr.massCol()[WFilter])
            self.out.fillBranch("MergedW%s_discriminator"%self.suffix,     ttr.discCol()[WFilter])

        return True

# define modules using the syntax 'name = lambda : constructor' to avoid having them loaded when not needed
