#ifndef TTMAK8TOPFILTER_H
#define TTMAK8TOPFILTER_H

#include "TopTagger/TopTagger/interface/TTModule.h"
#include "TopTagger/TopTagger/interface/TTMFilterBase.h"
#include "TopTagger/TopTagger/interface/TopObject.h"

class TopTaggerResults;
class Constituent;

class TTMAK8TopFilter : public TTModule
{
private:

    TopObject::Type type_;

public:
    void getParameters(const cfg::CfgDocument*, const std::string&);
    void run(TopTaggerResults&);
};
REGISTER_TTMODULE(TTMAK8TopFilter);

#endif
