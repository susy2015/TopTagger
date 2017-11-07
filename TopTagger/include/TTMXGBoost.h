#ifndef TTMXGBOOST_H
#define TTMXGBOOST_H

#include "TopTagger/TopTagger/include/TTModule.h"

#include <string>
#include <vector>

#ifdef DOXGBOOST
#include "include/xgboost/c_api.h"
#endif


class TTMXGBoost : public TTModule
{
private:
#ifdef DOXGBOOST
    double discriminator_;
    std::string modelFile_, inputOp_, outputOp_;
    double csvThreshold_;
    double bEtaCut_;
    int maxNbInTop_;

    //XGBoost booster pointer
    BoosterHandle h_booster;

    //Input variable names 
    std::vector<std::string> vars_;
#endif

public:
    ~TTMXGBoost();

    void getParameters(const cfg::CfgDocument*, const std::string&);
    void run(TopTaggerResults&);
};
REGISTER_TTMODULE(TTMXGBoost);

#endif
