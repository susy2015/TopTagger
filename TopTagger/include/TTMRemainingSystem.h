#ifndef TTMREMAININGSYSTEM_H
#define TTMREMAININGSYSTEM_H

#include "TopTagger/TopTagger/include/TTModule.h"

class TTMRemainingSystem : public TTModule
{
private:
    double CSVThresh_, lowRsysMass_, highRsysMass_, dRMax_;
    bool useSecondJet_;

public:
    void getParameters(const cfg::CfgDocument*, const std::string&);
    void run(TopTaggerResults&);
};
REGISTER_TTMODULE(TTMRemainingSystem);

#endif
