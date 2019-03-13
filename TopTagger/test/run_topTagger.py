import FWCore.ParameterSet.Config as cms

import FWCore.ParameterSet.VarParsing as VarParsing
### parsing job options 
import sys

options = VarParsing.VarParsing()

options.register('maxEvents',-1,VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.int,"maximum events")
options.register('skipEvents', 0, VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.int, "skip N events")
options.register('isData', 0, VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.int,"isData flag (0 for MC, 1 for data)")

options.parseArguments()

process = cms.Process("SHOTTagger")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.load("Configuration.EventContent.EventContent_cff")
process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
from Configuration.AlCa.GlobalTag import GlobalTag

print "isData: ", options.isData
if options.isData:
    print "Running on Data"
    process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:run2_data', '')
else:
    print "Running on MC"
    process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:run2_mc', '')    

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(options.maxEvents) )

process.load('FWCore.MessageService.MessageLogger_cfi')
process.MessageLogger.cerr.FwkReport.reportEvery = 100

process.options = cms.untracked.PSet(
   allowUnscheduled = cms.untracked.bool(True),  
   wantSummary=cms.untracked.bool(True)
)

from PhysicsTools.PatAlgos.patInputFiles_cff import filesRelValTTbarPileUpMINIAODSIM

###############################################################################################################################

process.source = cms.Source('PoolSource',
                            fileNames=cms.untracked.vstring (["/store/mc/RunIISummer16MiniAODv3/TTJets_SingleLeptFromT_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PUMoriond17_94X_mcRun2_asymptotic_v3_ext1-v2/120000/1E4DDC30-2AEB-E811-BBB1-0CC47A5FA3BD.root"]),
)

process.source.skipEvents = cms.untracked.uint32(options.skipEvents)
process.maxEvents  = cms.untracked.PSet( 
    input = cms.untracked.int32 (1000) 
)

###############################################################################################################################

from PhysicsTools.PatAlgos.producersLayer1.electronProducer_cfi import patElectrons

process.slimmedElectronsUpdated = cms.EDProducer("PATElectronUpdater",
    src = cms.InputTag("slimmedElectrons"),
    vertices = cms.InputTag("offlineSlimmedPrimaryVertices"),
    computeMiniIso = cms.bool(True),
    pfCandsForMiniIso = cms.InputTag("packedPFCandidates"),
    miniIsoParamsB = patElectrons.miniIsoParamsB, # so they're in sync
    miniIsoParamsE = patElectrons.miniIsoParamsE, # so they're in sync
)

from PhysicsTools.SelectorUtils.tools.vid_id_tools import setupVIDSelection
from RecoEgamma.ElectronIdentification.egmGsfElectronIDs_cff import egmGsfElectronIDs

process.egmGsfElectronIDs = egmGsfElectronIDs
process.egmGsfElectronIDs.physicsObjectIDs = cms.VPSet()
process.egmGsfElectronIDs.physicsObjectSrc = cms.InputTag("slimmedElectronsUpdated")

_electron_id_modules_WorkingPoints = cms.PSet(
    modules = cms.vstring(
        'RecoEgamma.ElectronIdentification.Identification.cutBasedElectronID_Summer16_80X_V1_cff',
        'RecoEgamma.ElectronIdentification.Identification.cutBasedElectronHLTPreselecition_Summer16_V1_cff',
        'RecoEgamma.ElectronIdentification.Identification.cutBasedElectronID_Spring15_25ns_V1_cff',
    ),
    WorkingPoints = cms.vstring(
        "egmGsfElectronIDs:cutBasedElectronID-Summer16-80X-V1-veto",
        "egmGsfElectronIDs:cutBasedElectronID-Summer16-80X-V1-loose",
        "egmGsfElectronIDs:cutBasedElectronID-Summer16-80X-V1-medium",
        "egmGsfElectronIDs:cutBasedElectronID-Summer16-80X-V1-tight",
    )
)

process.bitmapVIDForEle = cms.EDProducer("EleVIDNestedWPBitmapProducer",
    src = cms.InputTag("slimmedElectronsUpdated"),
    WorkingPoints = _electron_id_modules_WorkingPoints.WorkingPoints
)

from math import ceil, log

#this is magic ...
for modname in _electron_id_modules_WorkingPoints.modules:
    ids= __import__(modname, globals(), locals(), ['idName','cutFlow'])
    for name in dir(ids):
        _id = getattr(ids,name)
        if hasattr(_id,'idName') and hasattr(_id,'cutFlow'):
            setupVIDSelection(egmGsfElectronIDs,_id)

process.isoForEle = cms.EDProducer("EleIsoValueMapProducer",
    src = cms.InputTag("slimmedElectronsUpdated"),
    relative = cms.bool(False),
    rho_MiniIso = cms.InputTag("fixedGridRhoFastjetAll"),
    rho_PFIso = cms.InputTag("fixedGridRhoFastjetAll"),
    EAFile_MiniIso = cms.FileInPath("RecoEgamma/ElectronIdentification/data/Spring15/effAreaElectrons_cone03_pfNeuHadronsAndPhotons_25ns.txt"),
    EAFile_PFIso = cms.FileInPath("RecoEgamma/ElectronIdentification/data/Summer16/effAreaElectrons_cone03_pfNeuHadronsAndPhotons_80X.txt"),
)

process.ptRatioRelForEle = cms.EDProducer("ElectronJetVarProducer",
    srcJet = cms.InputTag("slimmedJets"),
    srcLep = cms.InputTag("slimmedElectronsUpdated"),
    srcVtx = cms.InputTag("offlineSlimmedPrimaryVertices"),
)

process.slimmedElectronsWithUserData = cms.EDProducer("PATElectronUserDataEmbedder",
    src = cms.InputTag("slimmedElectronsUpdated"),
    userFloats = cms.PSet(
        miniIsoAll = cms.InputTag("isoForEle:miniIsoAll"),
    ),
    userInts = cms.PSet(
        VIDNestedWPBitmap = cms.InputTag("bitmapVIDForEle"),
    ),
    userCands = cms.PSet(
        jetForLepJetVar = cms.InputTag("ptRatioRelForEle:jetForLepJetVar") # warning: Ptr is null if no match is found
    ),
)


###############################################################################################################################

from PhysicsTools.PatAlgos.producersLayer1.muonProducer_cfi import patMuons

# this below is used only in some eras
process.slimmedMuonsUpdated = cms.EDProducer("PATMuonUpdater",
    src = cms.InputTag("slimmedMuons"),
    vertices = cms.InputTag("offlineSlimmedPrimaryVertices"),
    computeMiniIso = cms.bool(True),
    pfCandsForMiniIso = cms.InputTag("packedPFCandidates"),
    miniIsoParams = patMuons.miniIsoParams, # so they're in sync
    recomputeMuonBasicSelectors = cms.bool(True),
)

process.isoForMu = cms.EDProducer("MuonIsoValueMapProducer",
    src = cms.InputTag("slimmedMuonsUpdated"),
    relative = cms.bool(False),
    rho_MiniIso = cms.InputTag("fixedGridRhoFastjetAll"),
    EAFile_MiniIso = cms.FileInPath("PhysicsTools/NanoAOD/data/effAreaMuons_cone03_pfNeuHadronsAndPhotons_80X.txt"),
)

process.ptRatioRelForMu = cms.EDProducer("MuonJetVarProducer",
    srcJet = cms.InputTag("slimmedJets"),
    srcLep = cms.InputTag("slimmedMuonsUpdated"),
    srcVtx = cms.InputTag("offlineSlimmedPrimaryVertices"),
)

process.slimmedMuonsWithUserData = cms.EDProducer("PATMuonUserDataEmbedder",
     src = cms.InputTag("slimmedMuonsUpdated"),
     userFloats = cms.PSet(
        miniIsoAll = cms.InputTag("isoForMu:miniIsoAll"),
     ),
     userCands = cms.PSet(
        jetForLepJetVar = cms.InputTag("ptRatioRelForMu:jetForLepJetVar") # warning: Ptr is null if no match is found
     ),
)

###############################################################################################################################

process.load('RecoJets.JetProducers.QGTagger_cfi')
process.QGTagger.srcJets   = cms.InputTag("slimmedJets")
process.QGTagger.jetsLabel = cms.string('QGL_AK4PFchs')

process.slimmedJetsWithUserData = cms.EDProducer("PATJetUserDataEmbedder",
    src = cms.InputTag("slimmedJets"),
    userFloats = cms.PSet(
        qgptD   = cms.InputTag("QGTagger:ptD"),
        qgAxis1 = cms.InputTag("QGTagger:axis1"),
        qgAxis2 = cms.InputTag("QGTagger:axis2"),
        ),
    userInts = cms.PSet(
        qgMult = cms.InputTag("QGTagger:mult")
        ),
)

###############################################################################################################################

process.load("TopTagger.TopTagger.SHOTProducer_cfi")
process.SHOTProducer.ak4JetSrc = cms.InputTag("slimmedJetsWithUserData")
process.SHOTProducer.muonSrc = cms.InputTag('slimmedMuonsWithUserData')
process.SHOTProducer.elecSrc = cms.InputTag('slimmedElectronsWithUserData')
process.SHOTProducer.doLeptonCleaning = cms.bool(True)

###############################################################################################################################

process.out = cms.OutputModule("PoolOutputModule",
                               fileName = cms.untracked.string ("test.root"),
                               outputCommands = cms.untracked.vstring('keep *_SHOTProducer_*_*')
)

###############################################################################################################################

process.p = cms.Path(process.slimmedElectronsUpdated * process.isoForEle * process.egmGsfElectronIDs * process.bitmapVIDForEle * process.ptRatioRelForEle * process.slimmedElectronsWithUserData * process.slimmedMuonsUpdated * process.isoForMu * process.ptRatioRelForMu * process.slimmedMuonsWithUserData * process.QGTagger * process.slimmedJetsWithUserData * process.SHOTProducer)
process.endP = cms.EndPath(process.out)

