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

class TTMTMVA : public TTModule
{
#ifdef SHOTTOPTAGGER_DO_TMVA
private:
    double discriminator_;
    std::string modelFile_, modelName_;
    std::vector<std::string> vars_;
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
