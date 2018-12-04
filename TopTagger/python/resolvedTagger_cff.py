import FWCore.ParameterSet.Config as cms
from  PhysicsTools.NanoAOD.common_cff import *

def customizeResolvedTagger(process):

    #updatedJets have recieved all updates from nanoAOD except final pT cut
    inputJetCollection = cms.InputTag("updatedJets")

    #run QGTagger code again to calculate jet axis1 
    process.load('RecoJets.JetProducers.QGTagger_cfi')
    process.QGTagger.srcJets = inputJetCollection

    #update jets to include new user float/int
    from PhysicsTools.PatAlgos.producersLayer1.jetUpdater_cff import updatedPatJets
    patJetsAuxiliary = updatedPatJets.clone(
        jetSource = inputJetCollection,
        addJetCorrFactors = cms.bool(False),
        addBTagInfo = cms.bool(False)
    )
    patJetsAuxiliary.userData.userFloats.src += ['QGTagger:qgLikelihood','QGTagger:ptD', 'QGTagger:axis1', 'QGTagger:axis2']
    patJetsAuxiliary.userData.userInts.src += ['QGTagger:mult']

    process.resolvedTopTagJets = patJetsAuxiliary

    #top tagging producer 
    process.load("TopTagger.TopTagger.SHOTProducer_cfi")
    process.SHOTProducer.ak4JetSrc = cms.InputTag("resolvedTopTagJets")
    process.SHOTProducer.muonSrc = cms.InputTag("slimmedMuonsWithUserData")
    process.SHOTProducer.elecSrc = cms.InputTag("slimmedElectronsWithUserData")
    
    #save resolved tops to nanoAOD
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

    #Construct task list 
    process.resolvedTask = cms.Task(process.QGTagger, process.resolvedTopTagJets, process.SHOTProducer, process.resolvedTopTable)

    process.schedule.associate(process.resolvedTask)

    return process
