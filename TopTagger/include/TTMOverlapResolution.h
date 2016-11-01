#ifndef TTMOVERLAPRESOLUTION_H
#define TTMOVERLAPRESOLUTION_H

#include "TopTagger/TopTagger/include/TTModule.h"

#include <string>

class TopTaggerResults;

class TTMOverlapResolution : public TTModule
{
private:
    double mt_, maxTopEta_;
    std::string sortMethod_;

public:
    void getParameters(const cfg::CfgDocument*);
    void run(TopTaggerResults&);
};
REGISTER_TTMODULE(TTMOverlapResolution);

#endif
