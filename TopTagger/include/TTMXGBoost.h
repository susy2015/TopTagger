#ifndef TTMXGBOOST_H
#define TTMXGBOOST_H

#include "TopTagger/TopTagger/include/TTModule.h"

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

class TTMXGBoost : public TTModule
{
private:
#ifdef DOXGBOOST
    double discriminator_;
    std::string modelFile_, inputOp_, outputOp_;
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
