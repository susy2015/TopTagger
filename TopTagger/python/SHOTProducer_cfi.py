import FWCore.ParameterSet.Config as cms

SHOTProducer = cms.EDProducer(
  "SHOTProducer",
  ak4JetSrc = cms.InputTag('slimmedJets'),
  ak4ptCut = cms.double(20.0),
  doLeptonCleaning = cms.bool(True),
  muonSrc = cms.InputTag('slimmedMuons'),
  elecSrc = cms.InputTag('slimmedElectrons'),
  muonIDFlag = cms.string("CutBasedIdMedium"),
  elecIDFlag = cms.string("cutbasedID_medium"),
  leptonJetDr = cms.double(0.10),
  qgTaggerKey = cms.string('QGTagger'),
  deepCSVBJetTags = cms.string('pfDeepCSVJetTags'),
  CvsBCJetTags = cms.string('pfCombinedCvsBJetTags'),
  CvsLCJetTags = cms.string('pfCombinedCvsLJetTags'),
  bTagKeyString = cms.string('pfCombinedInclusiveSecondaryVertexV2BJetTags'),
  taggerCfgFile = cms.FileInPath("TopTagger/TopTagger/data/TopTaggerCfg-DeepResolved_DeepCSV_GR_Medium_v1.0.0/TopTagger.cfg"),
)

