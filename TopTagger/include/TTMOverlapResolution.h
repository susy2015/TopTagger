#ifndef TTMOVERLAPRESOLUTION_H
#define TTMOVERLAPRESOLUTION_H

#include "TopTagger/TopTagger/include/TTModule.h"
#include "TopTagger/TopTagger/include/TTMFilterBase.h"

#include <string>

class TopTaggerResults;

class TTMOverlapResolution : public TTModule, public TTMFilterBase
{
private:
    float mt_, maxTopEta_, dRMatch_, cvsThreshold_;
    int NConstituents_;
    std::string sortMethod_;

public:
    void getParameters(const cfg::CfgDocument*, const std::string&);
    void run(TopTaggerResults&);
};
REGISTER_TTMODULE(TTMOverlapResolution);

#endif
