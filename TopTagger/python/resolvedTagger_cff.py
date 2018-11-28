import FWCore.ParameterSet.Config as cms

from  PhysicsTools.NanoAOD.common_cff import *

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


def customizeResolvedTagger(process):

    jetTag = cms.InputTag("updatedJets")
    process.load('RecoJets.JetProducers.QGTagger_cfi')
    process.QGTagger.srcJets   = jetTag
    
    process, jetTag = addJetInfo(process, jetTag, userFloats=['QGTagger:qgLikelihood','QGTagger:ptD', 'QGTagger:axis1', 'QGTagger:axis2'], userInts=['QGTagger:mult'], suff="")
    
    process.load("TopTagger.TopTagger.SHOTProducer_cfi")
    process.SHOTProducer.ak4JetSrc = jetTag
    process.SHOTProducer.muonSrc = cms.InputTag("slimmedMuonsWithUserData")
    process.SHOTProducer.elecSrc = cms.InputTag("slimmedElectronsWithUserData")
    
    process.resolvedTopTable = cms.EDProducer("SimpleCandidateFlatTableProducer",
        src=cms.InputTag("SHOTProducer"),
        cut=cms.string(""),
        name=cms.string("ResolvedTop"),
        doc=cms.string("Neural network top tagger results (http://susy2015.github.io/TopTagger)."),
        singleton=cms.bool(False),
        extension=cms.bool(False),
        variables=cms.PSet(P4Vars,
                           discriminator = Var("getDiscriminator()", float, doc="top discriminator", precision=10),
                           type = Var("getType()", int, doc="top type (http://susy2015.github.io/TopTagger/html/classTopObject.html, \"enum Type\")"),
                           j1Idx = Var("getJ1Idx()", int, doc="index to the first jet in the top"),
                           j2Idx = Var("getJ2Idx()", int, doc="index to the second jet in the top"),
                           j3Idx = Var("getJ3Idx()", int, doc="index to the third jet in the top"),
        )
    )

    process.resolvedTask = cms.Task(process.QGTagger, getattr(process, jetTag.getModuleLabel()), process.SHOTProducer, process.resolvedTopTable)

    process.schedule.associate(process.resolvedTask)

    return process
