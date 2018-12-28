#ifndef TTMBASICCLUSTERALGO_H
#define TTMBASICCLUSTERALGO_H

#include "TopTagger/TopTagger/interface/TTModule.h"
#include "TopTagger/TopTagger/interface/TTMConstituentReqs.h"

class TopTaggerResults;
class TopObject;
class Constituent;

/**
 *This module is used to cluster top candidates from nanoAOD data format.  This algorithm assumes that the nanoAOD contains all deepAK8 and deepResolved candidates in the nanoAOD already.  This module is capable of clustering trijet (resolved tops) and monojet (fully merged top/W) candidates.
 *
 *@param doTrijet (bool) Enable the resolved top category clustering.
 *@param doMonojet (bool) Enable the fully merged top category clustering.
 *@param doMonoW (bool) Enable the fully merged W category clustering.
 *
 *See TTMConstituentReqs for more parameters
 */
class TTMNanoAODClusterAlgo : public TTModule, public TTMConstituentReqs
{
private:
    //mono-jet variables
    bool doMonojet_;

    //trijet variables
    bool doTrijet_;

    //W-jet variables
    bool doMonoW_;

public:
    void getParameters(const cfg::CfgDocument*, const std::string&);
    void run(TopTaggerResults&);
};
REGISTER_TTMODULE(TTMNanoAODClusterAlgo);

#endif
