#ifndef TTMAK8WFILTER_H
#define TTMAK8WFILTER_H

#include "TopTagger/TopTagger/include/TTModule.h"
#include "TopTagger/TopTagger/include/TTMFilterBase.h"

class TopTaggerResults;
class Constituent;

class TTMAK8WFilter : public TTModule, public TTMFilterBase
{
private:
    double dRMatch_;

public:
    void getParameters(const cfg::CfgDocument*);
    void run(TopTaggerResults&);
};
REGISTER_TTMODULE(TTMAK8WFilter);

#endif
