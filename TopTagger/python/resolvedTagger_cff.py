import FWCore.ParameterSet.Config as cms
from PhysicsTools.NanoAOD.common_cff import *

def prepareJets(process):

    #Get jet source used as input to nanoAOD
    inputJetCollection = process.slimmedJetsWithUserData.src

    #run QGTagger code again to calculate jet axis1 
    process.load('RecoJets.JetProducers.QGTagger_cfi')
    process.QGTagger.srcJets = inputJetCollection

    #update jets to include new user float/int    
    jetUserFloat = process.slimmedJetsWithUserData.userFloats.clone(
            qgptD   = cms.InputTag("QGTagger:ptD"),
            qgAxis1 = cms.InputTag("QGTagger:axis1"),
            qgAxis2 = cms.InputTag("QGTagger:axis2"),
        )

    process.slimmedJetsWithUserData.userFloats = jetUserFloat

    jetUserInt = process.slimmedJetsWithUserData.userInts.clone(
            qgMult = cms.InputTag("QGTagger:mult")
        )

    process.slimmedJetsWithUserData.userInts = jetUserInt

    #Construct task list 
    process.resolvedJetTask = cms.Task(process.QGTagger)

    process.schedule.associate(process.resolvedJetTask)

    return process


def setupResolvedTaggerVariables(process):

    topTaggerJetVars = process.jetTable.variables.clone(
            deepCSVb    = Var("bDiscriminator('pfDeepCSVJetTags:probb')",    float,doc="DeepCSV b discriminator"     ,precision=10),
            deepCSVbb   = Var("bDiscriminator('pfDeepCSVJetTags:probbb')",   float,doc="DeepCSV bb discriminator"    ,precision=10),
            deepCSVudsg = Var("bDiscriminator('pfDeepCSVJetTags:probudsg')", float,doc="DeepCSV light discriminator" ,precision=10),
            deepCSVc    = Var("bDiscriminator('pfDeepCSVJetTags:probc')",    float,doc="DeepCSV charm discriminator" ,precision=10),

            deepFlavourb    = Var("bDiscriminator('pfDeepFlavourJetTags:probb')",    float,doc="DeepFlavour b discriminator"           ,precision=10),
            deepFlavourlepb = Var("bDiscriminator('pfDeepFlavourJetTags:problepb')", float,doc="DeepFlavour lep b discriminator"       ,precision=10),
            deepFlavourbb   = Var("bDiscriminator('pfDeepFlavourJetTags:probbb')",   float,doc="DeepFlavour bb discriminator"          ,precision=10),
            deepFlavouruds  = Var("bDiscriminator('pfDeepFlavourJetTags:probuds')",  float,doc="DeepFlavour light quark discriminator" ,precision=10),
            deepFlavourg    = Var("bDiscriminator('pfDeepFlavourJetTags:probg')",    float,doc="DeepFlavour gluon discriminator"       ,precision=10),
            deepFlavourc    = Var("bDiscriminator('pfDeepFlavourJetTags:probc')",    float,doc="DeepFlavour charm discriminator"       ,precision=10),

            qgptD = Var("userFloat('qgptD')",float,doc="QG Jet ptD",precision=10),
            qgAxis1 = Var("userFloat('qgAxis1')",float,doc="QG Jet semi major axis",precision=10),
            qgAxis2 = Var("userFloat('qgAxis2')",float,doc="QG Jet semi minor axis",precision=10),
            qgMult = Var("userInt('qgMult')",int ,doc="QG constituent multiplicity"),

            muEF      = Var("muonEnergyFraction()",        float, doc="muon energy fraction"        ,precision=10),
            phEF      = Var("photonEnergyFraction()",      float, doc="photon energy fraction"      ,precision=10),
            elEF      = Var("electronEnergyFraction()",    float, doc="electron energy fraction"    ,precision=10),
            hfHadEF   = Var("HFHadronEnergyFraction()",    float, doc="HF hadron energy fraction"   ,precision=10),
            hfEMEF    = Var("HFEMEnergyFraction()",        float, doc="HF EM energy fraction"       ,precision=10),
            chHadMult = Var("chargedHadronMultiplicity()", float, doc="charged hadron multiplicity" ,precision=10),
            neHadMult = Var("neutralHadronMultiplicity()", float, doc="neutral hadron multiplicity" ,precision=10),
            phMult    = Var("photonMultiplicity()",        float, doc="photon multiplicity"         ,precision=10),
            elMult    = Var("electronMultiplicity()",      float, doc="electron multiplicity"       ,precision=10),
            muMult    = Var("muonMultiplicity()",          float, doc="muon multiplicity"           ,precision=10),

            CvsL = Var("bDiscriminator('pfCombinedCvsLJetTags')",    float,doc="Charm vs Light discriminator" ,precision=10),
            CvsB = Var("bDiscriminator('pfCombinedCvsBJetTags')",    float,doc="Charm vs b discriminator"     ,precision=10),
        )

    process.jetTable.variables = topTaggerJetVars

    return process


def setupResolvedTagger(process, saveAllTopCandidates=False):
    
    #top tagging producer 
    process.load("TopTagger.TopTagger.SHOTProducer_cfi")
    #updatedJets have recieved all updates from nanoAOD except final pT cut
    process.SHOTProducer.ak4JetSrc = cms.InputTag("updatedJets")
    process.SHOTProducer.muonSrc = cms.InputTag("slimmedMuonsWithUserData")
    process.SHOTProducer.elecSrc = cms.InputTag("slimmedElectronsWithUserData")
    process.SHOTProducer.elecIDFlag = cms.string("cutbasedID_Fall17_V2_medium")
    process.SHOTProducer.saveAllTopCandidates = cms.bool(saveAllTopCandidates)

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
    process.resolvedTask = cms.Task(process.SHOTProducer, process.resolvedTopTable)

    process.schedule.associate(process.resolvedTask)

    return process


def customizeResolvedTagger(process):
    process = prepareJets(process)
    process = setupResolvedTagger(process)
    return process

def customizeResolvedTaggerAllCanidiates(process):
    process = prepareJets(process)
    process = setupResolvedTagger(process, True)
    return process

def customizeResolvedTaggerVariables(process):
    process = prepareJets(process)
    process = setupResolvedTaggerVariables(process)
    return process

def customizeResolvedTaggerAndVariables(process):
    process = prepareJets(process)
    process = setupResolvedTagger(process)
    process = setupResolvedTaggerVariables(process)
    return process


def customizeResolvedTaggerAllCanidiatesAndVariables(process):
    process = prepareJets(process)
    process = setupResolvedTagger(process, True)
    process = setupResolvedTaggerVariables(process)
    return process

