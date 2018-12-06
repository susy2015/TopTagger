#ifndef TTMXGBOOST_H
#define TTMXGBOOST_H

#include "TopTagger/TopTagger/interface/TTModule.h"

#include <memory>
#include <string>
#include <vector>

#ifdef DOXGBOOST
#include "include/xgboost/c_api.h"
#endif

namespace ttUtility
{
    class MVAInputCalculator;
}

/**
 *This module implements an interface to the OpenCV randomforest package for filtering top candidates.  This module can either pass entries directly into the final top list, or filter entries out of the final top list if they do not pass the selection criteria. 
 *
 *@param discCut (float) Minimum threshold for the TMVA discriminator for the candidate to pass the selection
 *@param modelFile (string) Path to the model file
 *@param NConstituents (int) Category of top to apply selection too (1 - monojet, 2 - dijet, 3 - trijet)
 *@param NCores (int) Number of cpu to allow XGBoost to use (default 1)
 *@param csvThreshold (float) Threshold on b-tag discriminator to be considered a b-jet.  
 *@param bEtaCut (float) Requirment on |eta| for a constituent to be considered a b-jet
 *@param maxNbInTop (int) The maximum number of constituent jets which can be b-tagged for the candidate to be a final *@param mvaVar[] (string - array) MVA variable input names
 */
class TTMXGBoost : public TTModule
{
private:
#ifdef DOXGBOOST
    double discriminator_;
    std::string modelFile_;
    double csvThreshold_;
    double bEtaCut_;
    int NConstituents_;
    int maxNbInTop_;
    int nCores_;

    //XGBoost booster pointer
    BoosterHandle h_booster;

    //vector to translate from map to DMatrix
    std::vector<float> data_;

    //Input variable names 
    std::vector<std::string> vars_;

    //variable calclator
    std::unique_ptr<ttUtility::MVAInputCalculator> varCalculator_; 

#endif

public:
    ~TTMXGBoost();

    void getParameters(const cfg::CfgDocument*, const std::string&);
    void run(TopTaggerResults&);
};
REGISTER_TTMODULE(TTMXGBoost);

#endif
