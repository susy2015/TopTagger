#ifndef TTMBASICCLUSTERALGO_H
#define TTMBASICCLUSTERALGO_H

#include "TopTagger/TopTagger/include/TTModule.h"

class TopTaggerResults;
class Constituent;

class TTMBasicClusterAlgo : public TTModule
{
private:
    double lowWMassCut_, highWMassCut_, lowtMassCut_, hightMassCut_, minTopCandMass_, maxTopCandMass_, dRMax_;
    bool doMonojet_, doDijet_, doTrijet_;


    bool passAK8TopReqs(const Constituent& constituent) const;
    bool passAK8WReqs(const Constituent& constituent) const;

public:
    void getParameters(const cfg::CfgDocument*);
    void run(TopTaggerResults&);
};
REGISTER_TTMODULE(TTMBasicClusterAlgo);

#endif
