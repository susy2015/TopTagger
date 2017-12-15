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

options.register('inputScript','',VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.string,"input Script")
options.register('outputFile','output',VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.string,"output File (w/o .root)")
options.register('maxEvents',-1,VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.int,"maximum events")
options.register('skipEvents', 0, VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.int, "skip N events")
options.register('job', 0, VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.int, "job number")
options.register('nJobs', 1, VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.int, "total jobs")
options.register('release','8_0_1', VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.string,"release number (w/o CMSSW)")
options.register('isData', 0, VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.int,"isData flag (0 for MC, 1 for data)")

options.parseArguments()

print("Using release "+options.release)


#if hasattr(sys, "argv"):
#    options.parseArguments()


process = cms.Process("DNNFiller")

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
process.MessageLogger.cerr.FwkReport.reportEvery = 1000

process.options = cms.untracked.PSet(
   allowUnscheduled = cms.untracked.bool(True),  
   wantSummary=cms.untracked.bool(False)
)

from PhysicsTools.PatAlgos.patInputFiles_cff import filesRelValTTbarPileUpMINIAODSIM

###############################################################################################################################

process.source = cms.Source('PoolSource',
#    fileNames=cms.untracked.vstring (["/store/mc/RunIISummer16MiniAODv2/TTJets_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/110000/423685A0-BFE6-E611-B2B5-001E67DBE36D.root"]),
                            fileNames=cms.untracked.vstring (["file:/uscms_data/d3/pastika/zinv/dev/CMSSW_8_0_26_patch1/src/SusyAnaTools/SkimsAUX/workdir/prod/80X_crab_example/423685A0-BFE6-E611-B2B5-001E67DBE36D.root"]),
#            '/store/data/Run2016C/SingleMuon/MINIAOD/23Sep2016-v1/90000/109B7DBF-0C91-E611-A5EC-0CC47A4D7690.root',]),
)

#if options.inputScript != '' and options.inputScript != 'DeepNTuples.DeepNtuplizer.samples.TEST':
#    process.load(options.inputScript)

numberOfFiles = len(process.source.fileNames)
numberOfJobs = options.nJobs
jobNumber = options.job

process.source.fileNames = process.source.fileNames[jobNumber:numberOfFiles:numberOfJobs]
if options.nJobs > 1:
    print ("running over these files:")
    print (process.source.fileNames)
#process.source.fileNames = ['file:/uscms/home/verzetti/nobackup/CMSSW_8_0_25/src/DeepNTuples/copy_numEvent100.root']

process.source.skipEvents = cms.untracked.uint32(options.skipEvents)
process.maxEvents  = cms.untracked.PSet( 
    input = cms.untracked.int32 (-1) 
)


################################################################################################################################
#
#from RecoJets.JetProducers.ak4PFJets_cfi import ak4PFJets
#from RecoJets.JetProducers.ak4GenJets_cfi import ak4GenJets
#
### Filter out neutrinos from packed GenParticles
#process.packedGenParticlesForJetsNoNu = cms.EDFilter("CandPtrSelector", 
#                                                     src = cms.InputTag("prunedGenParticles"), 
#                                                     cut = cms.string("abs(pdgId) != 12 && abs(pdgId) != 14 && abs(pdgId) != 16"))
#
### Define GenJets
#process.ak4GenJetsNoNu = ak4GenJets.clone(src = 'packedGenParticlesForJetsNoNu')
#
### -- do projections --
#process.pfCHS = cms.EDFilter("CandPtrSelector", 
#                             src = cms.InputTag("packedPFCandidates"), 
#                             cut = cms.string("fromPV"))
#
#process.pfNoMuonCHSNoMu =  cms.EDProducer("CandPtrProjector", 
#                                          src = cms.InputTag("pfCHS"), 
#                                          veto = cms.InputTag("prodMuons", "mu2Clean"))
#process.pfNoElectronCHSNoEle = cms.EDProducer("CandPtrProjector", 
#                                              src = cms.InputTag("pfNoMuonCHSNoMu"), 
#                                              veto = cms.InputTag("prodElectrons", "ele2Clean"))
#process.ak4PFJetsCHSNoLep = ak4PFJets.clone(src = 'pfNoElectronCHSNoEle', doAreaFastjet = True) # no idea while doArea is false by default, but it's True in RECO so we have to set it
#
################################################################################################################################
#
#jetCorrectionsAK4 = ('AK4PFchs', ['L1FastJet', 'L2Relative', 'L3Absolute'], 'None')
#
#from PhysicsTools.PatAlgos.tools.jetTools import addJetCollection
#
#addJetCollection(
#    process,
#    postfix = "",
#    labelName = 'DeepFlavour',
#    jetSource = cms.InputTag('ak4PFJetsCHSNoLep'),
#    pvSource = cms.InputTag('offlineSlimmedPrimaryVertices'),
#    pfCandidates = cms.InputTag('packedPFCandidates'),
#    svSource = cms.InputTag('slimmedSecondaryVertices'),
#    elSource = cms.InputTag('slimmedElectrons'),
#    muSource = cms.InputTag('slimmedMuons'),
#    jetCorrections = jetCorrectionsAK4,
##    btagDiscriminators = bTagDiscriminators,
#    genJetCollection = cms.InputTag('ak4GenJetsNoNu'),
#    genParticles = cms.InputTag('prunedGenParticles'),
#    algo = 'AK', rParam = 0.4
#    )


###############################################################################################################################

# QGLikelihood

qgDatabaseVersion = 'cmssw8020_v2'

databasepath='QGL_cmssw8020_v2.db'

from CondCore.CondDB.CondDB_cfi import *
process.QGPoolDBESSource = cms.ESSource("PoolDBESSource",
      CondDB.DBParameters,
      toGet = cms.VPSet(),
      connect = cms.string('sqlite_file:'+databasepath),
)

for type in ['AK4PFchs','AK4PFchs_antib']:
    process.QGPoolDBESSource.toGet.extend(cms.VPSet(cms.PSet(
                record = cms.string('QGLikelihoodRcd'),
                tag    = cms.string('QGLikelihoodObject_'+qgDatabaseVersion+'_'+type),
                label  = cms.untracked.string('QGL_'+type)
                )))

process.es_prefer_jec = cms.ESPrefer("PoolDBESSource", "QGPoolDBESSource")


###############################################################################################################################

process.load('RecoJets.JetProducers.QGTagger_cfi')
#process.QGTagger.srcJets   = cms.InputTag("selectedPatJetsDeepFlavour")
process.QGTagger.srcJets   = cms.InputTag("slimmedJets")
process.QGTagger.jetsLabel = cms.string('QGL_AK4PFchs')

#process, jetTag = addJetInfo(process, cms.InputTag("selectedPatJetsDeepFlavour"), userFloats=['QGTagger:qgLikelihood','QGTagger:ptD', 'QGTagger:axis1', 'QGTagger:axis2'], userInts=['QGTagger:mult'], suff="")
process, jetTag = addJetInfo(process, cms.InputTag("slimmedJets"), userFloats=['QGTagger:qgLikelihood','QGTagger:ptD', 'QGTagger:axis1', 'QGTagger:axis2'], userInts=['QGTagger:mult'], suff="")

###############################################################################################################################

#Deep Flavor

from PhysicsTools.PatAlgos.tools.jetTools import updateJetCollection

updateJetCollection(
   process,
   labelName = "DeepFlavour",
   jetSource = jetTag,
   jetCorrections = ('AK4PFchs', cms.vstring(['L1FastJet', 'L2Relative', 'L3Absolute']), 'None'),
   btagDiscriminators = [
      'pfDeepFlavourJetTags:probb',
      'pfDeepFlavourJetTags:probbb',
      'pfDeepFlavourJetTags:problepb',
      'pfDeepFlavourJetTags:probc',
      'pfDeepFlavourJetTags:probuds',
      'pfDeepFlavourJetTags:probg',
      ] ## to add discriminators
)

jetTag = cms.InputTag('selectedUpdatedPatJetsDeepFlavour')

###############################################################################################################################

from JMEAnalysis.JetToolbox.jetToolbox_cff import jetToolbox

# To get the lepton cleaned collection
#process.pfCandidatesNoMu =  cms.EDProducer("CandPtrProjector", 
#                                          src = cms.InputTag("packedPFCandidates"), 
#                                          veto = cms.InputTag("prodMuons", "mu2Clean"))
#process.pfCandidatesNoEle = cms.EDProducer("CandPtrProjector", 
#                                          src = cms.InputTag("pfCandidatesNoMu"), 
#                                          veto = cms.InputTag("prodElectrons", "ele2Clean"))
#jetToolbox( process, 'ak8', 'ak8JetSubsNoLep', 'out', 
#            runOnMC = options.mcInfo, 
#            PUMethod='Puppi', 
#            newPFCollection=True,
#            nameNewPFCollection='pfCandidatesNoEle',
#            addSoftDropSubjets = True, 
#            addSoftDrop = True, 
#            addNsub = True, 
#            bTagDiscriminators = ['pfCombinedInclusiveSecondaryVertexV2BJetTags'], 
#            addCMSTopTagger = False,
#            postFix="NoLep")

# Keep this behind the cleaned version for now, otherwise everything will be lepton cleaned
jetToolbox( process, 'ak8', 'ak8JetSubs', 'out', 
            runOnMC = not options.isData, 
            PUMethod='Puppi', 
            addSoftDropSubjets = True, 
            addSoftDrop = True, 
            addNsub = True, 
            bTagDiscriminators = ['pfCombinedInclusiveSecondaryVertexV2BJetTags'], 
            addCMSTopTagger = False)

###############################################################################################################################

process.load("TopTagger.TopTagger.SHOTProducer_cfi")
process.SHOTProducer.ak4JetSrc = jetTag
process.SHOTProducer.ak8JetSrc = cms.InputTag('packedPatJetsAK8PFPuppiSoftDrop')

###############################################################################################################################

process.out = cms.OutputModule("PoolOutputModule",
                               fileName = cms.untracked.string ("test.root"),
                               outputCommands = cms.untracked.vstring('keep *_SHOTProducer_*_*')
)

###############################################################################################################################

process.p = cms.Path(process.SHOTProducer)
process.endP = cms.EndPath(process.out)

