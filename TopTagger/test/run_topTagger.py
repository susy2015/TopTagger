import FWCore.ParameterSet.Config as cms

#Kevin's function of magic #1
def addJetInfo(process, JetTag, userFloats=[], userInts=[], btagDiscrs=cms.VInputTag(), suff=""):
    # add userfloats to jet collection
    from PhysicsTools.PatAlgos.producersLayer1.jetUpdater_cff import updatedPatJets as patJetsUpdated

    # default suffix
    if len(suff)==0: suff = "Auxiliary"
    
    JetTagOut = cms.InputTag(JetTag.value()+suff)
    patJetsAuxiliary = patJetsUpdated.clone(
        jetSource = JetTag,
        addJetCorrFactors = cms.bool(False),
        addBTagInfo = cms.bool(False)
    )
    patJetsAuxiliary.userData.userFloats.src += userFloats
    patJetsAuxiliary.userData.userInts.src += userInts
    if len(btagDiscrs)>0:
        patJetsAuxiliary.discriminatorSources = btagDiscrs
        patJetsAuxiliary.addBTagInfo = cms.bool(True)
    setattr(process,JetTagOut.value(),patJetsAuxiliary)
    
    return (process, JetTagOut)

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
                            fileNames=cms.untracked.vstring (["root://cmseos.fnal.gov//store/user/benwu/Stop18/NtupleSyncMiniAOD/00257B91-1808-E811-BD39-0242AC130002.root"]),
)

process.source.skipEvents = cms.untracked.uint32(options.skipEvents)
process.maxEvents  = cms.untracked.PSet( 
    input = cms.untracked.int32 (10000) 
)

###############################################################################################################################

process.load('RecoJets.JetProducers.QGTagger_cfi')
process.QGTagger.srcJets   = cms.InputTag("slimmedJets")
process.QGTagger.jetsLabel = cms.string('QGL_AK4PFchs')

process, jetTag = addJetInfo(process, cms.InputTag("slimmedJets"), userFloats=['QGTagger:qgLikelihood','QGTagger:ptD', 'QGTagger:axis1', 'QGTagger:axis2'], userInts=['QGTagger:mult'], suff="")

###############################################################################################################################

process.load("TopTagger.TopTagger.SHOTProducer_cfi")
process.SHOTProducer.ak4JetSrc = jetTag
#This is set to false because the 
process.SHOTProducer.doLeptonCleaning = cms.bool(False)

###############################################################################################################################

process.out = cms.OutputModule("PoolOutputModule",
                               fileName = cms.untracked.string ("test.root"),
                               outputCommands = cms.untracked.vstring('keep *_SHOTProducer_*_*')
)

###############################################################################################################################

process.p = cms.Path(process.QGTagger * process.slimmedJetsAuxiliary * process.SHOTProducer)
process.endP = cms.EndPath(process.out)

