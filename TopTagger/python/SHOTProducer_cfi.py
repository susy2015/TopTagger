import FWCore.ParameterSet.Config as cms

SHOTProducer = cms.EDProducer(
  "SHOTProducer",
  ak4JetSrc = cms.InputTag('slimmedJets'),
  ak4ptCut = cms.double(20.0),
  qgTaggerKey = cms.string('QGTagger'),
  deepCSVBJetTags = cms.string('pfDeepCSVJetTags'),
  CvsBCJetTags = cms.string('pfCombinedCvsBJetTags'),
  CvsLCJetTags = cms.string('pfCombinedCvsLJetTags'),
  bTagKeyString = cms.string('pfCombinedInclusiveSecondaryVertexV2BJetTags'),
  taggerCfgFile = cms.string("TopTagger.cfg"),
)

