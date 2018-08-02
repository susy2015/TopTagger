#ifndef TTMTMVA_H
#define TTMTMVA_H

#include "TopTagger/TopTagger/include/TTModule.h"

#include <string>
#include <vector>
#include <memory>

#ifdef SHOTTOPTAGGER_DO_TMVA
#include "TMVA/Tools.h"
#include "TMVA/Reader.h"
#endif

namespace ttUtility
{
    class MVAInputCalculator;
}

/**
 *This module implements an interface to TMVA for filtering top candidates.  This module can either pass entries directly into the final top list, or filter entries out of the final top list if they do not pass the selection criterion. 
 *
 *@param discCut (float) Minimum threshold for the TMVA discriminator for te candidate to pass the selection
 *@param modelFile (string) Path to the model file
 *@param modelName (string) Name of the model
 *@param NConstituents (int) What type of constituents to apply selection too (1 - monojet, 2 - dijet, 3 - trijet)
 *@param filter (bool) Filter failing candidates from the final top list instead of adding passing candidates to the final tops list
 *@param mvaVar[] (string - array) MVA variable input names
 */
class TTMTMVA : public TTModule
{
#ifdef SHOTTOPTAGGER_DO_TMVA
private:
    double discriminator_;
    std::string modelFile_, modelName_;
    std::vector<std::string> vars_;
    std::vector<std::string> varsTMVA_;
    int NConstituents_;
    bool filter_;

    //TMVA model variables 
    std::unique_ptr<TMVA::Reader> reader_;
    std::vector<float> varMap_;

    //variable calclator
    std::unique_ptr<ttUtility::MVAInputCalculator> varCalculator_; 

#endif

public:
    void getParameters(const cfg::CfgDocument*, const std::string&);
    void run(TopTaggerResults&);

};
REGISTER_TTMODULE(TTMTMVA);

#endif
