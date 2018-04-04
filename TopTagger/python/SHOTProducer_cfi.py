import FWCore.ParameterSet.Config as cms

SHOTProducer = cms.EDProducer(
  "SHOTProducer",
  ak4JetSrc = cms.InputTag('slimmedJets'),
  ak4ptCut = cms.double(20.0),
  ak8JetSrc = cms.InputTag("selectedPatJetsAK8PFPuppi"),
  ak8ptCut = cms.double(200.0),
  qgTaggerKey = cms.string('QGTagger'),
  deepCSVBJetTags = cms.string('pfDeepCSVJetTags'),
  deepFlavorBJetTags = cms.string('pfDeepFlavourJetTags'),
  combinedSVBJetTags = cms.string('pfCombinedSecondaryVertexV2BJetTags'),
  cMVABJetTags = cms.string('pfCombinedMVABJetTags'),
  cMVAv2BJetTags = cms.string('pfCombinedMVAV2BJetTags'),
  CvsBCJetTags = cms.string('pfCombinedCvsBJetTags'),
  CvsLCJetTags = cms.string('pfCombinedCvsLJetTags'),
  bTagKeyString = cms.string('pfCombinedInclusiveSecondaryVertexV2BJetTags'),
  NjettinessAK8Puppi_label = cms.string('NjettinessAK8Puppi'),
  ak8PFJetsPuppi_label = cms.string('ak8PFJetsPuppi'),
  taggerCfgFile = cms.string("TopTagger.cfg"),
)

