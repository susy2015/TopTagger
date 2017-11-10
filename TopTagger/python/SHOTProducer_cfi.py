import FWCore.ParameterSet.Config as cms

taggerCfg = """
"""

prodJets = cms.EDFilter(
  "SHOTProducer",
  jetSrc = cms.InputTag('slimmedJets'),
  ak4ptCut = cms.double(20.0),
  qgTaggerKey = cms.string('QGTagger'),
  deepCSVBJetTags = cms.string('pfDeepCSVJetTags'),
  deepFlavorBJetTags = cms.string('pfDeepFlavourJetTags'),
  combinedSVBJetTags = cms.string('pfCombinedSecondaryVertexV2BJetTags'),
  cMVABJetTags = cms.string('pfCombinedMVABJetTags'),
  cMVAv2BJetTags = cms.string('pfCombinedMVAV2BJetTags'),
  CvsBCJetTags = cms.string('pfCombinedCvsBJetTags'),
  CvsLCJetTags = cms.string('pfCombinedCvsLJetTags'),
  bTagKeyString = cms.string('pfCombinedInclusiveSecondaryVertexV2BJetTags'),
  taggerCfg = cms.string(taggerCfg),
)

