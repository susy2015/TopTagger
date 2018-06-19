#ifndef TTMBASICCLUSTERALGO_H
#define TTMBASICCLUSTERALGO_H

#include "TopTagger/TopTagger/include/TTModule.h"
#include "TopTagger/TopTagger/include/TTMConstituentReqs.h"

class TopTaggerResults;
class Constituent;

class TTMBasicClusterAlgo : public TTModule, public TTMConstituentReqs
{
private:
    //mino-jet variables
    bool doMonojet_;

    //dijet variables
    float dRMaxDiJet_;
    bool doDijet_;

    //trijet variables
    float minTopCandMass_, maxTopCandMass_, dRMaxTrijet_;
    bool doTrijet_;

public:
    void getParameters(const cfg::CfgDocument*, const std::string&);
    void run(TopTaggerResults&);
};
REGISTER_TTMODULE(TTMBasicClusterAlgo);

#endif
