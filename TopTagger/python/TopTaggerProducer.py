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
    def __init__(self, cfgName="TopTagger.cfg", cfgWD="."):
        self.topTaggerCfg = cfgName
        self.topTaggerWD = cfgWD

        self.tt = TopTagger(self.topTaggerCfg, self.topTaggerWD)

    def beginJob(self):
        pass

    def endJob(self):
        pass

    def beginFile(self, inputFile, outputFile, inputTree, wrappedOutputTree):
        self.isFirstEventOfFile = True
        self.out = wrappedOutputTree
        self.out.branch("ResolvedTop_cpp_Stop0l_pt", "F",            lenVar="nResolvedTop_cpp_Stop0l")
        self.out.branch("ResolvedTop_cpp_Stop0l_eta", "F",           lenVar="nResolvedTop_cpp_Stop0l")
        self.out.branch("ResolvedTop_cpp_Stop0l_phi", "F",           lenVar="nResolvedTop_cpp_Stop0l")
        self.out.branch("ResolvedTop_cpp_Stop0l_mass", "F",          lenVar="nResolvedTop_cpp_Stop0l")
        self.out.branch("ResolvedTop_cpp_Stop0l_discriminator", "F", lenVar="nResolvedTop_cpp_Stop0l")
        self.out.branch("ResolvedTop_cpp_Stop0l_j1Idx", "I",         lenVar="nResolvedTop_cpp_Stop0l")
        self.out.branch("ResolvedTop_cpp_Stop0l_j2Idx", "I",         lenVar="nResolvedTop_cpp_Stop0l")
        self.out.branch("ResolvedTop_cpp_Stop0l_j3Idx", "I",         lenVar="nResolvedTop_cpp_Stop0l")

        self.out.branch("MergedTop_cpp_Stop0l_pt", "F",              lenVar="nMergedTop_cpp_Stop0l")
        self.out.branch("MergedTop_cpp_Stop0l_eta", "F",             lenVar="nMergedTop_cpp_Stop0l")
        self.out.branch("MergedTop_cpp_Stop0l_phi", "F",             lenVar="nMergedTop_cpp_Stop0l")
        self.out.branch("MergedTop_cpp_Stop0l_mass", "F",            lenVar="nMergedTop_cpp_Stop0l")
        self.out.branch("MergedTop_cpp_Stop0l_discriminator", "F",   lenVar="nMergedTop_cpp_Stop0l")

        self.out.branch("MergedW_cpp_Stop0l_pt", "F",            lenVar="nMergedW_cpp_Stop0l")
        self.out.branch("MergedW_cpp_Stop0l_eta", "F",           lenVar="nMergedW_cpp_Stop0l")
        self.out.branch("MergedW_cpp_Stop0l_phi", "F",           lenVar="nMergedW_cpp_Stop0l")
        self.out.branch("MergedW_cpp_Stop0l_mass", "F",          lenVar="nMergedW_cpp_Stop0l")
        self.out.branch("MergedW_cpp_Stop0l_discriminator", "F", lenVar="nMergedW_cpp_Stop0l")

    def endFile(self, inputFile, outputFile, inputTree, wrappedOutputTree):
        pass

    def analyze(self, event):
        """process event, return True (go to next module) or False (fail, go to next event)"""
        ## Process event

        ttr = self.tt.runFromNanoAOD(event, self.isFirstEventOfFile)
        self.isFirstEventOfFile = False

        resolvedFilter = ttr.typeCol() == 3
        mergedFilter = ttr.typeCol() == 1
        WFilter = ttr.typeCol() == 4

        ### Store output
        self.out.fillBranch("ResolvedTop_cpp_Stop0l_pt",            ttr.ptCol()[resolvedFilter])
        self.out.fillBranch("ResolvedTop_cpp_Stop0l_eta",           ttr.etaCol()[resolvedFilter])
        self.out.fillBranch("ResolvedTop_cpp_Stop0l_phi",           ttr.phiCol()[resolvedFilter])
        self.out.fillBranch("ResolvedTop_cpp_Stop0l_mass",          ttr.massCol()[resolvedFilter])
        self.out.fillBranch("ResolvedTop_cpp_Stop0l_discriminator", ttr.discCol()[resolvedFilter])
        self.out.fillBranch("ResolvedTop_cpp_Stop0l_j1Idx",         ttr.j1IdxCol()[resolvedFilter])
        self.out.fillBranch("ResolvedTop_cpp_Stop0l_j2Idx",         ttr.j2IdxCol()[resolvedFilter])
        self.out.fillBranch("ResolvedTop_cpp_Stop0l_j3Idx",         ttr.j3IdxCol()[resolvedFilter])

        self.out.fillBranch("MergedTop_cpp_Stop0l_pt",              ttr.ptCol()[mergedFilter])
        self.out.fillBranch("MergedTop_cpp_Stop0l_eta",             ttr.etaCol()[mergedFilter])
        self.out.fillBranch("MergedTop_cpp_Stop0l_phi",             ttr.phiCol()[mergedFilter])
        self.out.fillBranch("MergedTop_cpp_Stop0l_mass",            ttr.massCol()[mergedFilter])
        self.out.fillBranch("MergedTop_cpp_Stop0l_discriminator",   ttr.discCol()[mergedFilter])

        self.out.fillBranch("MergedW_cpp_Stop0l_pt",                ttr.ptCol()[WFilter])
        self.out.fillBranch("MergedW_cpp_Stop0l_eta",               ttr.etaCol()[WFilter])
        self.out.fillBranch("MergedW_cpp_Stop0l_phi",               ttr.phiCol()[WFilter])
        self.out.fillBranch("MergedW_cpp_Stop0l_mass",              ttr.massCol()[WFilter])
        self.out.fillBranch("MergedW_cpp_Stop0l_discriminator",     ttr.discCol()[WFilter])

        return True

# define modules using the syntax 'name = lambda : constructor' to avoid having them loaded when not needed
