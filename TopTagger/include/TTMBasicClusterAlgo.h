#ifndef TTMBASICCLUSTERALGO_H
#define TTMBASICCLUSTERALGO_H

#include "TopTagger/TopTagger/include/TTModule.h"

class TopTaggerResults;
class Constituent;

class TTMBasicClusterAlgo : public TTModule
{
private:
    //mino-jet variables
    double minAK8TopMass_, maxAK8TopMass_, maxTopTau32_, minAK8TopPt_;
    bool doMonojet_;

    //dijet variables
    double minAK8WMass_, maxAK8WMass_, maxWTau21_, minAK8WPt_, minAK4WPt_, dRMaxDiJet_;
    bool doDijet_;

    //trijet variables
    double minTopCandMass_, maxTopCandMass_, dRMaxTrijet_, minAK4ResolvedPt_;
    bool doTrijet_;

    //Implement the requirements to be tagged as an AK8 W
    bool passAK8WReqs(const Constituent& constituent) const;

    //Implement the requirements for AK4 jets to partner with AK8 W
    bool passAK4WReqs(const Constituent& constituent) const;

    //Implement the requirements to be tagged as an AK8 top
    bool passAK8TopReqs(const Constituent& constituent) const;

    //Implement the requirements for the AK4 resolved constituents
    bool passAK4ResolvedReqs(const Constituent& constituent) const;

public:
    void getParameters(const cfg::CfgDocument*, const std::string&);
    void run(TopTaggerResults&);
};
REGISTER_TTMODULE(TTMBasicClusterAlgo);

#endif
