#ifndef TTMLAZYCLUSTERALGO_H
#define TTMLAZYCLUSTERALGO_H

#include "TopTagger/TopTagger/include/TTModule.h"

class TopTaggerResults;

class TTMLazyClusterAlgo : public TTModule
{
private:
    double lowWMassCut_, highWMassCut_, lowtMassCut_, hightMassCut_, minTopCandMass_, maxTopCandMass_;
    bool doMonojet_, doDijet_, doTrijet_;

public:
    void getParameters(const cfg::CfgDocument*);
    void run(TopTaggerResults&);
};
REGISTER_TTMODULE(TTMLazyClusterAlgo);

#endif
