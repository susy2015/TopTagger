import FWCore.ParameterSet.Config as cms

taggerCfg = """
#The TopTagger section holds configuration properties for the top tagger object
TopTagger
{
    #The module variable defines which modules will be run
    #and in which order
    #If a module is included more than once the context variable sets
    #the name to use below for its option set

    #Cluster constituents into candidates
    module[0] = "TTMBasicClusterAlgo"

    #Add initial candidates to top list
    module[1] = "TTMTFPyBind"
    module[2] = "TTMHEPRequirements"
    module[3] = "TTMTMVA"
    context[3] = "TTMTMVA_monojet"
    module[4] = "TTMTMVA"
    context[4] = "TTMTMVA_dijet"

    #Clean out tops with overlapping candiates
    module[5] = "TTMOverlapResolution"
    context[5] = "TTMOverlapResolution_monojet"

    module[6] = "TTMOverlapResolution"
    context[6] = "TTMOverlapResolution_dijet"

    module[7] = "TTMOverlapResolution"
    context[7] = "TTMOverlapResolution_Resolved"

    #Find the remaining system
    module[8] = "TTMRemainingSystem"

    #do the final sort of the top list
    module[9] = "TTMFinalSort"
}
#The Common section holds config info or constants which may be needed
#in multiple modules
Common
{
    #basic constants
    mW = 80.385
    mt = 173.5

    #eta cut applied to all final tops
    maxTopEta = 2.3
    #matching criterion between AK8 subjets and AK4 jets
    dRMatch = 0.4
}
#Below we have a section for each module specified above to define
#the module specific options
TTMBasicClusterAlgo
{
    #Resolved tagger Variables
    doTrijet = true
    minTopCandMass = 100
    maxTopCandMass = 250
    minTrijetAK4JetPt = 20
    midTrijetAK4JetPt = 30
    maxTrijetAK4JetPt = 40
    dRMaxTrijet = 3.1415
    nbSeed = -1

    #Boosted W tagger parameters
    doDijet = true
    minAK8WMass = 65
    maxAK8WMass = 100
    maxWTau21 = 999.999
    minAK8WPt = 200
    minAK4WPt = 30
    dRMaxDijet = 1.0

    #Boosted top tagger parameters
    doMonojet = true
    minAK8TopMass = 105
    maxAK8TopMass = 210
    maxTopTau32 = 999.999
    minAK8TopPt = 400
}
TTMHEPRequirements
{
    #Parameters used for tri & dijets
    Rmin = 0.85
    Rmax = 1.25

    #b-counting parameters used only for trijets
    csvThreshold = 0.8484
    bEtaCut = 2.4
    maxNbInTop = 1

    doMonojet = false
    doDijet = true
    doTrijet = false
}
TTMTFPyBind
{
    #DNN parameters
    discCut = 0.95
    discSlope = 0.000375
    discOffset = 0.80
    modelFile = "tfModel_frozen.pb"
    inputOp = "x:0"
    outputOp = "y_ph:0"
    mvaVar[0] = "cand_m"
    mvaVar[1] = "cand_p"
    mvaVar[2] = "j12_m"
    mvaVar[3] = "j13_m"
    mvaVar[4] = "j23_m"
    mvaVar[5] = "dTheta12"
    mvaVar[6] = "dTheta23"
    mvaVar[7] = "dTheta13"
    mvaVar[8] = "j1_m"
    mvaVar[9] = "j1_p"
    mvaVar[10] = "j1_QGL"
    mvaVar[11] = "j1_CSV"
    mvaVar[12] = "j2_m"
    mvaVar[13] = "j2_p"
    mvaVar[14] = "j2_QGL"
    mvaVar[15] = "j2_CSV"
    mvaVar[16] = "j3_m"
    mvaVar[17] = "j3_p"
    mvaVar[18] = "j3_QGL"
    mvaVar[19] = "j3_CSV"

    #b-counting parameters used to suppress fakerate for high Nb
    csvThreshold = 0.8484
    bEtaCut = 2.4
    maxNbInTop = 1
}
TTMTMVA_monojet
{
    discCut = 0.35
    modelFile = "weights-t2tt850-sm-baseline-nodphi-nomtb-hqu-08112016.xml"
    modelName = "BDTG"
    NConstituents = 1
    filter = false

    mvaVar[0] = "ak8_sdmass"
    mvaVar[1] = "ak8_ptDR"
    mvaVar[2] = "ak8_tau21"
    mvaVar[3] = "ak8_tau32"
    mvaVar[4] = "ak8_rel_ptdiff"
    mvaVar[5] = "ak8_csv1_mass"
    mvaVar[6] = "ak8_csv1_csv"
    mvaVar[7] = "ak8_csv1_ptD"
    mvaVar[8] = "ak8_csv1_axis1"
    mvaVar[9] = "ak8_csv2_mass"
    mvaVar[10] = "ak8_csv2_ptD"
    mvaVar[11] = "ak8_csv2_axis1"
    mvaVar[12] = "ak8_csv1_mult"
    mvaVar[13] = "ak8_csv2_mult"
}
TTMTMVA_dijet
{
    discCut = 0.05
    modelFile = "sdWTag_ttbarTraining_v0.xml"
    modelName = "BDTG"
    NConstituents = 2
    filter = true

    mvaVar[0] = "var_fj_sdmass"
    mvaVar[1] = "var_fj_ptDR"
    mvaVar[2] = "var_fj_tau21"
    mvaVar[3] = "var_fj_rel_ptdiff"
    mvaVar[4] = "var_sj1_ptD"
    mvaVar[5] = "var_sj1_axis1"
    mvaVar[6] = "var_sj2_ptD"
    mvaVar[7] = "var_sj2_axis1"
    mvaVar[8] = "var_sjmax_csv"
    mvaVar[9] = "var_sd_n2"
    mvaVar[10] = "var_sj1_mult"
    mvaVar[11] = "var_sj2_mult"
}
TTMOverlapResolution_Resolved
{
    NConstituents = 3
    sortMethod = "mvaDiscWithb"

    #used for sortMethod = "mvaDiscWithb" only
    csvThreshold = 0.8484
}
TTMOverlapResolution_dijet
{
    NConstituents = 2
    sortMethod = "topMass"
}
TTMOverlapResolution_monojet
{
    NConstituents = 1
    sortMethod = "none"
}
TTMRemainingSystem
{
    lowRsysMass = 50
    highRsysMass = 220
    csvThreshold = 0.8484
    dRMaxRsys = 1.5
    useSecondJet = true

    #Boosted W tagger parameters
    allowW = false
    minAK8WMass = 65
    maxAK8WMass = 100
    maxWTau21 = 0.60
    minAK8WPt = 200
}
TTMFinalSort
{
    sortMethod = "topPt"
}
"""

SHOTProducer = cms.EDProducer(
  "SHOTProducer",
  ak4JetSrc = cms.InputTag('slimmedJets'),
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

