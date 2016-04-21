#ifndef TTMHEPREQUIREMENTS_H
#define TTMHEPREQUIREMENTS_H

#include "TopTagger/TopTagger/include/TTModule.h"

class TopTaggerResults;

class TTMHEPRequirements : public TTModule
{
private:
    double mW_, mt_, Rmin_, Rmax_, csvThresh_, bEtaCut_;
    int maxNbInTop_;

public:
    void getParameters(const cfg::CfgDocument*);
    void run(TopTaggerResults&);
};
REGESTER_TTMODULE(TTMHEPRequirements);

#endif
