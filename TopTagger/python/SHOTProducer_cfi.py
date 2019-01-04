import FWCore.ParameterSet.Config as cms

SHOTProducer = cms.EDProducer(
  "SHOTProducer",
  ak4JetSrc = cms.InputTag('slimmedJets'),
  ak4PtCut = cms.double(20.0),
  doLeptonCleaning = cms.bool(True),
  muonSrc = cms.InputTag('slimmedMuons'),
  elecSrc = cms.InputTag('slimmedElectrons'),
  muonPtCut = cms.double(10.0),
  elecPtCut = cms.double(10.0),
  muonIsoName = cms.string('miniIsoAll'),
  elecIsoName = cms.string('miniIsoAll'),
  muonIsoCut = cms.double(0.2),
  elecIsoCut = cms.double(0.1),
  muonIDFlag = cms.string("CutBasedIdLoose"),
  elecIDBitFieldName = cms.string("VIDNestedWPBitmap"),
  elecIDFlag = cms.string("CutBasedIdVeto"),
  leptonJetDr = cms.double(0.20),
  qgTaggerKey = cms.string('QGTagger'),
  deepCSVBJetTags = cms.string('pfDeepCSVJetTags'),
  CvsBCJetTags = cms.string('pfCombinedCvsBJetTags'),
  CvsLCJetTags = cms.string('pfCombinedCvsLJetTags'),
  bTagKeyString = cms.string('pfCombinedInclusiveSecondaryVertexV2BJetTags'),
  taggerCfgFile = cms.FileInPath("TopTagger/TopTagger/data/TopTaggerCfg-DeepResolved_DeepCSV_GR_noDisc_Release_v1.0.0/TopTagger.cfg"),
  discriminatorCut = cms.double(0.5),
  saveAllTopCandidates = cms.bool(False)
)

