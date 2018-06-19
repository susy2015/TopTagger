#ifndef TTMREMAININGSYSTEM_H
#define TTMREMAININGSYSTEM_H

#include "TopTagger/TopTagger/include/TTModule.h"
#include "TopTagger/TopTagger/include/TTMConstituentReqs.h"

class TTMRemainingSystem : public TTModule, public TTMConstituentReqs
{
private:
    float CSVThresh_, lowRsysMass_, highRsysMass_, dRMax_;
    bool useSecondJet_, allowW_;

public:
    void getParameters(const cfg::CfgDocument*, const std::string&);
    void run(TopTaggerResults&);
};
REGISTER_TTMODULE(TTMRemainingSystem);

#endif
