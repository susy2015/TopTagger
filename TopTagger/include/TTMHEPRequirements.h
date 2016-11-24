#ifndef TTMHEPREQUIREMENTS_H
#define TTMHEPREQUIREMENTS_H

#include "TopTagger/TopTagger/include/TTModule.h"

class TopTaggerResults;

class TTMHEPRequirements : public TTModule
{
private:
    double mW_, mt_, Rmin_, Rmax_, csvThresh_, bEtaCut_;
    int maxNbInTop_;
    bool doTrijet_, doDijet_, doMonojet_;

public:
    void getParameters(const cfg::CfgDocument*, const std::string&);
    void run(TopTaggerResults&);
};
REGISTER_TTMODULE(TTMHEPRequirements);

#endif
