My Main Page                         {#mainpage}
============

## Installing the tagger in CMS software release

### Standalone (edm free) install instructions within CMSSW

If you would rather not go through the hassle of installing ROOT/python/tensorflow the CMSSW environment can be used to provide the necessary libraries and python modules 

~~~~~~~~~~~~~{.sh}
cmsrel CMSSW_9_3_3
cd CMSSW_9_3_3/src
cmsenv
git clone git@github.com:susy2015/TopTagger.git
cd TopTagger/TopTagger/test
./configure
make -j8 
~~~~~~~~~~~~~

The test code can then be run identically to the completely standalone instructions found [here](../README.md#running-the-example)

### Install tagger integrated in the edm framework

For the time being it is suggested to produce flat NTuples containing the input variables which are necessary to run the resolved tagger offline.  This is because running tensorflow alongside CMSSW 8_X_Y requires some rather convoluted hacks (instructions are given below, but these are very involved).  It is much easier to produce flat tuples where all the necessary jet information is kept and use the stansalong instructions above to process the flat NTuples.  

#### Instructions for producing jet variables for resolved top tagger with CMSSW 8_0_28_patch1

Setting up a new CMSSW 8_0_28_patch1 release to produce the resolved top tagger variables.

~~~~~~~~~~~~~{.sh}
#get CMSSW release
cmsrel CMSSW_8_0_28_patch1
cd CMSSW_8_0_28_patch1/src/
#initialize cms and cms git commands 
cmsenv
git cms-init
#patch to qg producer to add axis1
git cms-merge-topic -u pastika:AddJetAxis1
#compile
scram b -j8
~~~~~~~~~~~~~

The following code should be added to your CMSSW config file to prepare the jet collection with all necessary variables.  

~~~~~~~~~~~~~{.py}
jetTag = cms.InputTag("slimmedJets")

process.load('RecoJets.JetProducers.QGTagger_cfi')
process.QGTagger.srcJets   = cms.InputTag("slimmedJets")

from PhysicsTools.PatAlgos.tools.jetTools import updateJetCollection

updateJetCollection(
   process,
   labelName = "DeepCSV",
   jetSource = cms.InputTag("slimmedJets"),
   jetCorrections = ('AK4PFchs', cms.vstring(['L1FastJet', 'L2Relative', 'L3Absolute']), 'None'),
   btagDiscriminators = [
      'pfDeepCSVJetTags:probudsg',
      'pfDeepCSVJetTags:probb',
      'pfDeepCSVJetTags:probc',
      'pfDeepCSVJetTags:probbb',
      'pfDeepCSVJetTags:probcc',
      ] ## to add discriminators                                                                                                                                                                                                              
)

process.updatedPatJetsDeepCSV.userData.userFloats.src += ['QGTagger:qgLikelihood', 'QGTagger:ptD', 'QGTagger:axis1', 'QGTagger:axis2',]
process.updatedPatJetsDeepCSV.userData.userInts.src += ['QGTagger:mult',]

~~~~~~~~~~~~~

This will produce a jet collection called 'selectedUpdatedPatJetsDeepCSV' which contains all necessary variables for the resolved top tagger.

The necessary variables can then be accessed in a edm producer as follows 

~~~~~~~~~~~~{c++}
//in constructor
JetTok_ = consumes<std::vector<pat::Jet> >(edm::InputTag("selectedUpdatedPatJetsDeepCSV"));
~~~~~~~~~~~~

~~~~~~~~~~~~{c++}
//in produce(...)
edm::Handle<std::vector<pat::Jet> > jets;
iEvent.getByToken(JetTok_, jets);

for(const pat::Jet& jet : *jets)
{
    //No need to keep jets below 20 GeV
    if(jet.pt() < 20) continue;

    TLorentzVector perJetLVec;
    perJetLVec.SetPtEtaPhiE( jet.pt(), jet.eta(), jet.phi(), jet.energy() );

    double qgPtD = jet.userFloat("QGTagger:ptD");
    double qgAxis1 = jet.userFloat("QGTagger:axis1");
    double qgAxis2 = jet.userFloat("QGTagger:axis2");
    double qgMult = static_cast<double>(jet.userInt("QGTagger:mult"));
    double deepCSVb = jet.bDiscriminator("pfDeepCSVJetTags:probb");
    double deepCSVc = jet.bDiscriminator("pfDeepCSVJetTags:probc");
    double deepCSVl = jet.bDiscriminator("pfDeepCSVJetTags:probudsg");
    double deepCSVbb = jet.bDiscriminator("pfDeepCSVJetTags:probbb");
    double deepCSVcc = jet.bDiscriminator("pfDeepCSVJetTags:probcc");
    double btag = jet.bDiscriminator("pfCombinedSecondaryVertexV2BJetTags");
    double chargedHadronEnergyFraction = jet.chargedHadronEnergyFraction();
    double neutralHadronEnergyFraction = jet.neutralHadronEnergyFraction();
    double chargedEmEnergyFraction = jet.chargedEmEnergyFraction();
    double neutralEmEnergyFraction = jet.neutralEmEnergyFraction();
    double muonEnergyFraction = jet.muonEnergyFraction();
    double photonEnergyFraction = jet.photonEnergyFraction();
    double electronEnergyFraction = jet.electronEnergyFraction();
    double recoJetsHFHadronEnergyFraction = jet.HFHadronEnergyFraction();
    double recoJetsHFEMEnergyFraction = jet.HFEMEnergyFraction();
    double chargedHadronMultiplicity = jet.chargedHadronMultiplicity();
    double neutralHadronMultiplicity = jet.neutralHadronMultiplicity();
    double photonMultiplicity = jet.photonMultiplicity();
    double electronMultiplicity = jet.electronMultiplicity();
    double muonMultiplicity = jet.muonMultiplicity();
}
~~~~~~~~~~~~

For convinenent use with the top tagger the 4-vector and each jet variable can be saved in a flat tuple in its own std::vector per event.  

#### Instructions for running tagger online with CMSSW 8_0_28_patch1

The instructions are currently for CMSSW8, but these will be updated when MC is avaliable for 9X series releases.  In addition to the top tagger itself these instructions include the steps to configure additional packages, uncluding the deepFlavor tagger (for the tagger itself along with the tensorflow configuration for 8X), a patch to the qg producer to produce the Axis1 variable, and a version of the jet toolbox with a minor bug fix, 

~~~~~~~~~~~~~{.sh}
#get CMSSW release
cmsrel CMSSW_8_0_28_patch1
cd CMSSW_8_0_28_patch1/src/
cmsenv
git cms-init
#configure deep Flavor https://twiki.cern.ch/twiki/bin/viewauth/CMS/DeepJet
git cms-merge-topic -u mverzett:Experimental_DeepFlavour_80X
cd RecoBTag/DeepFlavour/scripts/
wget -nv http://www-ekp.physik.uni-karlsruhe.de/~harrendorf/tensorflow-cmssw8-0-26.tar.gz
tar -zxf tensorflow-cmssw8-0-26.tar.gz
mv tensorflow-cmssw8-0-26-patch1/site-packages ../../Tensorflow/python
rm -rf tensorflow-cmssw8-0-26.tar.gz tensorflow-cmssw8-0-26-patch1/
cd "$CMSSW_BASE/src"
scram setup "RecoBTag/Tensorflow/py2-numpy-c-api.xml"
cmsenv
#patch to gq producer to add axis1
git cms-merge-topic -u pastika:AddJetAxis1
#patched version of jet toolbox
git clone git@github.com:susy2015/JetToolbox.git JMEAnalysis/JetToolbox -b fix_NoLep_jetToolbox_80X_V3
#download top tagger code 
git clone -b IntermediateRecipeV0 git@github.com:susy2015/TopTagger.git
#compile everything 
scram b -j12
cd TopTagger/TopTagger/test
#get qgl database file
wget https://raw.githubusercontent.com/cms-jet/QGLDatabase/master/SQLiteFiles/QGL_cmssw8020_v2.db
#get top tager cfg file and MVA model files 
../../Tools/getTaggerCfg.sh -t Intermediate_Example_v1.0.0
#run example code
voms-proxy-init
cmsRun run_topTagger.py
~~~~~~~~~~~~~

The default configuration of the example cfg file "run_topTagger.py" will run over a single-lepton ttbar sample and produce an edm formatted output file ("test.root") containing the vector of reconstructed top TLorentzVectors along with a second vector indicating the type of top (monojet, dijet, trijet).  

#### Running tagger online in CMSSW 9_4_X/10_0_X

Instructions coming soon 

## More about getting a configuration file

Before the top tagger can be used the top tagger configuration file must be checked out.  Configuration files for top tagger working points are stored in the Susy2015/TopTaggerCfg repository and are published through releases.  These should not be accessed directly, but instead you can use the script "getTaggerCfg.sh" to download the working points desired.  An example of a basic checkout of the standard working point for use with the example code (different working points are found here https://github.com/susy2015/TopTaggerCfg/releases) is as follows

~~~~~~~~~~~~~sh
#run the following once to set up the top tagger and add the "getTaggerCfg.sh" to the path
source TopTagger/TopTagger/test/taggerSetup.sh
getTaggerCfg.sh -t MVAAK8_Tight_noQGL_binaryCSV_v1.0.2
~~~~~~~~~~~~~

This will download the configuration file along with the MVA training file if appropriate into a directory.  It will then softlink the files into your current directory so this script is intended to be run from the same directory as you will run your code.  If you want the directory containing the configuration file and MVA file to be located elsewhere this can be specified with the "-d" option (Multiple run directories can point to the same directory, this can be helpful to save space as the random forest training files are quite large).  If you have multiple configuration files you can specify a different name for the configuration file with the "-f" option.  Finally, if you want to download the file without creating the softlinks use the "-n" option.


## Top tagger structure

The top tagger code is written to take modules which act on a central object to produce the final collection of top objects.  The tagger framework consists of the following classes (found in TopTagger/TopTagger/)

### Top Tagger Modules and the Configuration File.

The configuration file is central to the functioning of the top tagger code.  This file is a basic text file implemented to follow the structure of the HCAL configuration parser and the code can be found here "TopTagger/CfgParser."  This code upon which this is based can be found here (https://svnweb.cern.ch/cern/wsvn/cmshcos/trunk/hcalBase/include/hcal/cfg/?#aafaf15fcace155f9a3d702b52eb6d719).  The configuration script is used to tell the top tagger what modules to run and in which order, in addition to allowing any parameters necessary for the tagger and each module to be defined cleanly in one place.  An example configuration script can be found in TopTagger/TopTagger/test/Example_TopTagger.cfg

Modules are where all the real work of the top tagger is done.  Each module performs a particular task and stores its results in a TopTaggerResults object which will eventually be presented to the user as a summary of final results.  All modules inherit from TTModule and, as described in TTModule" implement the 2 functions "getParameter"s and "run."  Each module is called automatically from the TopTagger class.  They are also instantiated automatically based upon the configuration file.

For further documentation of the code and each module see the following webpage http://susy2015.github.io/TopTagger/html/index.html.

### TopTagger

The TopTagger module is the primary (and only mandatory) section in every top tagger configuration.  This section defines all the other top tagger modules which will be run and in which order.  This module has 2 variables (both arrays) Which are used to define the module run order and if necessary the module context name.

#### module[] (string):

This variable is an array and is used to define which other modules will be run and in which order.  This can be any module listed here in this section.

#### context[] (string):

This variable must be specified for any module being run more than once to specify what context name to read its configuration from.

