# TopTagger

##Compiling the tagger

The code is provided with full support for usage in CMSSW as well as with standalone root.  To build in CMSSW simply checkout the repository in the src folder of CMSSW and run "scram b" in the base TopTagger directory.  To compile the standaline library in a root aware terminal go to TopTagger/TopTagger/test and run "make".

##Top tagger structure 

The top tagger code is written to take modules which act on a central object to produce the final collection of top objects.  The tagger framework consists of the following classes (found in TopTagger/TopTagger/)

Constituent:
Serves as a wrapper class for jet objects including the 4-vector for each jet along with supporting information such as the b-tag discriminator and jet type

TopObject:
This class holds all the necessary information to define a top candidate.  This includes a list of constituents which makes up the top candidate as well as supporting information, like maxDr and the combined jet candidate 4-vedtor.

TopTagger:
This class holds the primary structure of the tagger.  It is responcible for parsing the configuration file for the tagger, instantiating and running the requested modules, and storing the common results objects for the modules and user.  

TopTaggerResults:
This is a holder class which holds the final collection of top objects, along with the constituents used to construct them and any intermediate information used by modules.  This serves both as the user interfact for the results and a container to pass between modules.  

TopTaggerUtilities:
This file containst the namespace ttUtilities in which simple helper functions can be placed.  At the moment this only contains a helper function to convert a vector of TLorentz vectorss and a corrosponding vector of b-tag discriminators into a vector of Constituents.  

TTMFactory:
This class acts as a factory to allow dynamic creation of modules.  Its functioning should be invisible to the end user.  

TTModule:
This is a pure virtual base class which defines the interface for a top tagger module.  This implements 2 pure virtual functions which must be overriden by all modules.  The first, "getParameters," is used to load any configuration parameters the module may need from the config file.  The second, "run," is used to run the particular algorithm implemented in the module.  None of these functions should even be called directly, but instead are called automatically by the TopTagger class.  It is also important that each module also include the line "REGESTER_TTMODULE(ModuleClassName)" after the class declaration.  This macro calls special code so that the TTMFactory class can dynamically implement the module by name.  

TTMLazyClusterAlgo, TTMHEPRequirements, TTMOverlapResolution:
These three classes are module implementations.  They inherit, as all modules must, from TTModule and as described there implement the 2 functions "getParameter"s and "run."  Each module is called automatically from the TopTagger class.  They are also instantiated automatically based upon the configuration file.  

##Configuration file 

The configuration file is central to the functioning of the top tagger code.  This file is a basic text file implemented to follow the structure of the HCAL configuration parser and the code can be found here "TopTagger/CfgParser."  This code upon which this is based can be found here (https://svnweb.cern.ch/cern/wsvn/cmshcos/trunk/hcalBase/include/hcal/cfg/?#aafaf15fcace155f9a3d702b52eb6d719).  The configuration script is used to tell the top tagger what modules to run and in which order, in addition to allowing any parameters necessary for the tagger and each module to be defined cleanly in one place.  An example configuration script can be found in TopTagger/TopTagger/test

##Psudo-code

```c++

//manditory includes to use top tagger
#include "TopTagger.h"
#include "TopTaggerResults.h"

//this include is necessary to handle exceptions thrown by the top tagger code
//#include "TTException.h"

//this include is useful to get the helper function to make the vector of constituents
#include "TopTaggerUtilities.h"


int main()
{
    //Open data file

    //configure top tagger
    TopTagger tt;
    tt.setCfgFile("path/to/cfg/file.cfg");

    for(auto& event : file)
    {
        //get jet vector
        vector<TLorentzVector> jets; // = jet vector

        //get b-tag info
        vector<double> btaginfo; // = b-tag vector

        //construct vector of constituents 
        vector<Constituent> constituents = ttUtility::packageCandidates(jets, btaginfo);

        //run tagger
        tt.runTagger(constituents);

        //get output of tagger
        const TopTaggerResults& ttr = tt.getResults();

        //do something with the results
        for(const TopObject* top : ttr.getTops())
        {
            //print top pt
            printf("Top Pt: %lf\n", top->p().Pt());
        }
    }
}

```

## Example code

comming soon


## Running MVA code

Please go to TopTagger/Tools directory and find instruction in README





