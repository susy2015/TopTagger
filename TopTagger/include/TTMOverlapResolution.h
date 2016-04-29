#ifndef TTMOVERLAPRESOLUTION_H
#define TTMOVERLAPRESOLUTION_H

#include "TopTagger/TopTagger/include/TTModule.h"

class TopTaggerResults;

class TTMOverlapResolution : public TTModule
{
private:
    double mt_, minTopCandMass_, maxTopCandMass_, minTopCandMassLoose_, maxTopCandMassLoose_, maxTopEta_;

public:
    void getParameters(const cfg::CfgDocument*);
    void run(TopTaggerResults&);
};
REGISTER_TTMODULE(TTMOverlapResolution);

#endif
