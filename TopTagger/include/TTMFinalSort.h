#ifndef TTMFINALSORT_H
#define TTMFINALSORT_H

#include "TopTagger/TopTagger/include/TTModule.h"
#include "TopTagger/TopTagger/include/TTMFilterBase.h"

#include <string>

class TopTaggerResults;

class TTMFinalSort : public TTModule
{
private:
    double mt_;
    std::string sortMethod_;

public:
    void getParameters(const cfg::CfgDocument*, const std::string&);
    void run(TopTaggerResults&);
};
REGISTER_TTMODULE(TTMFinalSort);

#endif
