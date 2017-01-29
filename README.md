# TopTagger

##Compiling the tagger

The code is provided with full support for usage in CMSSW as well as with standalone root.  Note that with the latest release openCV is required to be installed to build top tagger code.  Instructions for installing on the LPC are found in the readme in the Tools/ folder of the repository.  To build in CMSSW simply checkout the repository in the src folder of CMSSW and run "scram b" in the base TopTagger directory.

To compile the standalone library in a root aware terminal go to TopTagger/TopTagger/test and run "make".  You will also need to install openCV on your laptop.

##Top tagger structure

The top tagger code is written to take modules which act on a central object to produce the final collection of top objects.  The tagger framework consists of the following classes (found in TopTagger/TopTagger/)

#### Constituent:

Serves as a wrapper class for jet objects including the 4-vector for each jet along with supporting information such as the b-tag discriminator and jet type

#### TopObject:

This class holds all the necessary information to define a top candidate.  This includes a list of constituents which makes up the top candidate as well as supporting information, like maxDr and the combined jet candidate 4-vector.

#### TopTagger:

This class holds the primary structure of the tagger.  It is responsible for parsing the configuration file for the tagger, instantiating and running the requested modules, and storing the common results objects for the modules and user.

#### TopTaggerResults:

This is a holder class which holds the final collection of top objects, along with the constituents used to construct them and any intermediate information used by modules.  This serves both as the user interface for the results and a container to pass between modules.

#### TopTaggerUtilities:

This file contains the namespace ttUtilities in which simple helper functions can be placed.  At the moment this only contains a helper function to convert a vector of TLorentz vectors and a corresponding vector of b-tag discriminators into a vector of Constituents.

#### TTMFactory:

This class acts as a factory to allow dynamic creation of modules.  Its functioning should be invisible to the end user.

#### TTModule:

This is a pure virtual base class which defines the interface for a top tagger module.  This implements 2 pure virtual functions which must be overridden by all modules.  The first, "getParameters," is used to load any configuration parameters the module may need from the config file.  The second, "run," is used to run the particular algorithm implemented in the module.  None of these functions should even be called directly, but instead are called automatically by the TopTagger class.  It is also important that each module also include the line "REGESTER_TTMODULE(ModuleClassName)" after the class declaration.  This macro calls special code so that the TTMFactory class can dynamically implement the module by name.



## Top Tagger Modules and the Configuration File.

The configuration file is central to the functioning of the top tagger code.  This file is a basic text file implemented to follow the structure of the HCAL configuration parser and the code can be found here "TopTagger/CfgParser."  This code upon which this is based can be found here (https://svnweb.cern.ch/cern/wsvn/cmshcos/trunk/hcalBase/include/hcal/cfg/?#aafaf15fcace155f9a3d702b52eb6d719).  The configuration script is used to tell the top tagger what modules to run and in which order, in addition to allowing any parameters necessary for the tagger and each module to be defined cleanly in one place.  An example configuration script can be found in TopTagger/TopTagger/test/Example_TopTagger.cfg

Modules are where all the real work of the top tagger is done.  Each module performs a particular tast and stores its results in a TopTaggerResults object which will eventually be presented to the user as a summary of final results.  All modules inherit from TTModule and, as described in TTModule" implement the 2 functions "getParameter"s and "run."  Each module is called automatically from the TopTagger class.  They are also instantiated automatically based upon the configuration file.


### TopTagger

The TopTagger module is the primary (and only mandatory) section in every top tagger configuration.  This section defines all the other top tagger modules which will be run and in which order.  This module has 2 variables (both arrays) Which are used to define the module run order and if necessary the module context name.

#### module[] (string):

This variable is an array and is used to define which other modules will be run and in which order.  This can be any module listed here in this section.

#### context[] (string):

This variable must be specified for any module being run more than once to specify what context name to read its configuration from.


### Common

The common context holds any variables/constants which are generally needed by more than one module.

#### mW:

The mass of the W boson.

#### mt:

The mass of the top quark.

#### maxTopEta:

The maximum eta allowed for final selected tops.

#### dRMatch:

The dR matching cone used to match AK4 jets to AK8 subjets.


### TTMLazyClusterAlgo

This module is used to cluster top candidates using only AK4 jets.  This module is capable of clustering trijet (resolved tops), dijet (W+jet), and monojet (fully merged top) candidates.

#### dRMax (float):

This is the maximum cone size in which a trijet or dijet category top candidate is allowed to lie.

#### lowWJetMassCut, highWJetMassCut (float):

These parameters defines the mass window for a single AK4 jet to be considered as a W-jet.

#### lowtJetMassCut, hightJetMassCut (float):

These parameters defines the mass window for a single AK4 jet to be considered as a merged-top jet.

#### minTopCandMass, maxTopCandMass (float):

These parameters defines the mass window for a di or trijet combination to be considered as a top-candidate.

#### minJetPt (float):

The minimum jet pt which is considered in the clustering process.

#### doMonojet (boolean):

Enable the fully merged top category.

#### doDijet (boolean):

Enable the W+jet top category.

#### doTrijet (boolean):

Enable the fully resolved top category.


### TTMBasicClusterAlgo

This module is used to cluster top candidates using a combination of AK4 and AK8 jets.  This algorithm uses AK8 jets for the merged W and top candidates, and AK4 jets for the resolved category as well as to combine with W jets to for top candidates.  This module is capable of clustering trijet (resolved tops), dijet (W+jet), and monojet (fully merged top) candidates.

#### doTrijet (boolean):

Enable the resolved top category clustering.  

#### minTopCandMass, maxTopCandMass (float):

These parameters define the mass window for the trijet and dijet resolved top candidates.

#### minAK4ResolvedPt (float):

The minimum pt requirement for AK4 jets to be considered by the resolved clustering algorithm.  

#### dRMaxTrijet (float):

The maximum dR cone size for resolved AK4 jets to be considered as a top candidate.  

#### doDijet (boolean):

Enable the W+jet top category clustering.  

#### minAK8WMass, maxAK8WMass (float):

These parameters define the mass window for the AK8 merged W candidates.  

#### maxWTau21 (float):

The maximum allowed value of the tau21 nsubjettiness variable for AK8 merged W candidates.  

#### minAK8WPt (float):

The minimum pt allowed for the AK8 merged W candidate to be considered in a top candidate.

#### minAK4WPt (float):

The minimum pt allowed for the AK4 jet paired with an AK8 W to be considered in a top candidate.

#### dRMaxDijet (boolean):

The maximum cone size allowed for W+jet top candidates.  

#### doMonojet (boolean):

Enable the fully merged AK8 top category.  

#### minAK8TopMass, maxAK8TopMass (float):

These parameters define the mass window allowed for AK8 jets to be considered as fully merged tops.

#### maxTopTau32 (float):

The maximum allowed value of the tau32 nsubjettiness variable for merged AK8 top candidates.  

#### minAK8TopPt (float):

The minimum pt requirement for AK8 jets to be considered as top candidates.  


### TTMAK8TopFilter

This module applies any final selection requirements to the fully merged AK8 tops.  Candidates passing these requirements are passed directly into the final top list.  Currently this module simply passes all tops and had no parameters.


### TTMHEPRequirements

This module applies the HEP top tagging criterion to the candidates as well as applying basic requirements on the number of b-jets in the candidate.  For trijet candidates the full HEP requirements are applied.  For dijet candidates the module applies a simplified version of the HEP criterion designed for use with a W and an additional jet.  The dijet and trijet categories will function with either inputs from TTMLazyClusterAlgo or TTMBasicClusterAlgo.  If the monojet mode is activated it will simply pass all AK4 monojet top candidates, but not AK8 monojet tops (this is instead done by "TTMAK8TopFilter").  Candidates passed by this module are placed into the final top list.

#### Rmin, Rmax (float):  

These parameters define the valid range for the R parameter in the HEP top tagging requirements.  Under the basic assumption that the input jets are massless, this requires that ratio of dijet to trijet masses (inside a jet triplet) is consistent with the ratio of the w to top masses within the specified bounds.  This applies to the full trijet HEP requirements as well as the simplified requirements for the dijet case.  

#### csvThreshold (float):

The minimum cut value on the CSV discriminator for an AK4 jet to be considered a b-tagged jet.  

#### bEtaCut (float):

The maximum absolute psudorapidity requirement placed on jets to be considered b-tagged.  

#### maxNbInTop (integer):

The maximum number of b-jets to allow in a single top candidate.  

#### doMonojet (boolean):

Enable the processing of AK4 monojet top candidates by this module (they are currently all automatically passed by this module).

#### doDijet (boolean):

Enable the processing of AK4 dijet candidates or AK8 W + AK4 jet candidates (depending which clustering algorithm is used).

#### doTrijet (boolean):

Enable the selection criterion for resolved AK4 trijet top candidates.  


### TTMOpenCVMVA

This module implements the openCV Random Forest regressor to select resolved AK4 trijet candidates.  Those that pass the discriminator threshold are passed directly into the final top list.  

#### discCut (float):

This is the minimum value for the random forest regressor discriminator value to select a trijet candidate as a final top.

#### modelFile (string):

This variable specifies the model file containing the trained random forest trained to discriminate trijet candidates matched to tops form those that do not.  

#### mvaVar[] (string):

This array specifies the variable names used in by the specified model file.  The order of these variables matters (as specified by the array indicies) and the array indicies must be sequential from 0 with no skipped indicies.  

#### csvThreshold (float):

The minimum cut value on the CSV discriminator for an AK4 jet to be considered a b-tagged jet.  

#### bEtaCut (float):

The maximum absolute psudorapidity requirement placed on jets to be considered b-tagged.  

#### maxNbInTop (integer):

The maximum number of b-jets to allow in a single top candidate.  



### TTMFilterBase

This class forms the base class for "TTMOverlapResolution" and holds the code necessary to do AK4 jet matching to AK8 subjets.  


### TTMOverlapResolution

After top candidates are added to the final top list it is possible that some constituent jets can be included in more than one top in the list.  This module is used to remove the tops which share constituents.  It functions by first sorting the list of tops by a figure of merit, and then in the case that 2 tops share a constituent, the top with the better figure of merit is removed.  The module can by run on the entire list of tops, or it can be run category by category.  

#### NConstituents (int):

This parameter determines which tops to include in the overlap resolution process. The options include "-1" for all tops, or 1, 2, or 3, for monojet, dijet, or trijet categories respectively.  

#### sortMethod (string):

This parameter defines the initial sorting order used to prioritize one top over another when resolving overlapping.  The options include "topMass", "topPt", "mvaDisc", "mvaDiscWithb", and "none".  

### TTMRemainingSystem

This module calclates the remaining system.  The purpose of the remaining system is to partially reconstruct a top quark to give a second input into the MT2 calculation for the 1-top catagory.  The reconstruction algorithm used here starts by looking for a b-jet not included in a top.  If no b jet is found then the highest pt jet not in a top is selected.  The algorithm can then combine nearby jets with the first selected jet if certain requirements are satisified.  If no second jet satisifies these requirments, the fist chosen jet is used by itself.  This algorithm only finds one such object using this method.  If multiple jet pairings satisify the requirements, the pairing with the smallest dR is chosen.  

#### lowRsysMass, highRsysMass (float):

These parameters define the mass window in which the original jet plus a second jet are allowed to fall.  

#### dRMaxRsys (float):

This is the maximum allowed angle between the first chosen jet and the second jet.  Set this to a negative value to disable the dR requirement.  

#### useSecondJet (boolean):

This flag defines if the algorith should try to look for a second jet, or simply use the first chosen just for the remaining system.  


### TTMFinalSort

This module is used to sort the final list of tops after overlap resolution.  

#### sortMethod (string):

This parameter defines the sorting order.  The possible options are "topMass", "topPt", and "none".



##Getting a configuration file

Configuration files for top tagger working points are stored in the Susy2015/TopTaggerCfg repository and are published through releases.  These should not be accessed directly, but instead you can use the script "Tools/getTaggerCfg.sh" to download the working points desired.  An example of a basic checkout of a working point (different working points are found here https://github.com/susy2015/TopTaggerCfg/releases) is as follows

```
./getTaggerCfg.sh -t MVAAK8_Medium_v1.0.1
```

This will download the configuration file along with the MVA training file if appropriate into a directory.  It will then softlink the files into your current directory so this script is intended to be run from the same directory as you will run your code.  If you want the directory containing the configuration file and MVA file to be located elsewhere this can be specified with the "-d" option.  If you have multiple configuration files you can specify a different name for the configuration file with the "-f" option.  Finally, if you want to download the file without creating softlinks use the "-n" option.

##Psudo-code

```c++

//manditory includes to use top tagger
#include "TopTagger/TopTagger/include/TopTagger.h"
#include "TopTagger/TopTagger/include/TopTaggerResults.h"
//this include is useful to get the helper function to make the vector of constituents
#include "TopTagger/TopTagger/include/TopTaggerUtilities.h"

//this include is necessary to handle exceptions thrown by the top tagger code
//#include "TTException.h"


int main()
{
    //Open data file

    //configure top tagger
    TopTagger tt;
    tt.setCfgFile("path/TopTaggerConfig.cfg");

    for(auto& event : file)
    {
        //AK4 variables
        //get AK4 jet vector
        vector<TLorentzVector> jets_AK4; // = AK4 jet vector

        //get b-tag info
        vector<double> btaginfo; // = b-tag vector

	//get QGL info
        vector<double> QGLinfo; // = jet QGL vector

        //AK8 variables
        //get AK8 jet vector
        vector<TLorentzVector> jets_AK8; //  = AK8 jet vector

        //get subjet vector (these will be dR and index matched to AK8 jets)
        vector<TLorentzVector> subJetsLVec; // = AK8 subjets

        //get subjet multiplicity variables
        std::vector<double> tau1; // = tau1 variable
        std::vector<double> tau2; // = tau2 variable
        std::vector<double> tau3; // = tau3 variable

        //get softdrop mass for AK8 jets
        std::vector<double> softDropMass; // = soft drop mass vector

        //Correction file for W soft drop mass
        std::string wMassCorrectionfile; // = mass correction root file for AK8 W jets

        //prepare containers of input collections
        ttUtility::ConstAK4Inputs myConstAK4Inputs = ttUtility::ConstAK4Inputs(jets_AK4, btaginfo, QGLinfo);
        ttUtility::ConstAK8Inputs myConstAK8Inputs = ttUtility::ConstAK8Inputs(jets_AK8, tau1, tau2, tau3, softDropMass, subJetsLVec);
        myConstAK8Inputs.setWMassCorrHistos(wMassCorrectionfile);

	//construct vector of constituents
        vector<Constituent> constituents = ttUtility::packageConstituents(myConstAK4Inputs, myConstAK8Inputs);

        //run tagger
        tt.runTagger(constituents);

        //get output of tagger
        const TopTaggerResults& ttr = tt.getResults();

	//get reconstructed top
	vector<TopObject*> Ntop = ttr.getTops();

	//do something with the results
        for(const TopObject* top : ttr.getTops())
        {
            //print top pt
            printf("Top Pt: %lf\n", top->p().Pt());
        }
    }
}
```

##Training output file

TTMOpenCVMVA module needs a training out file (.model extension). This file is downloaded along with the cfg file as described in the "Configuration file" section.

## Load opencv library path

Now its done by dynamic path setting. So run the following command

```
source TopTagger/Tools/opencvSetup.csh
```
or
```
setenv LD_LIBRARY_PATH ${LD_LIBRARY_PATH}:${CMSSW_BASE}/src/opencv/lib/
```

## Example code

One is welcome to look TopTagger/Tools/TagTest1.cc


## Running MVA code

Please go to TopTagger/Tools directory and find instruction in README

 LocalWords:  doDijet boolean doTrijet AK4 trijet TTMOpenCVMVA openCV regressor
 LocalWords:  discCut modelFile TTMFilterBase TTMOverlapResolution AK8 subjets
 LocalWords:  mvaDiscWithb indicies
