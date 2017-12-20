#ifndef TTMHEPREQUIREMENTS_H
#define TTMHEPREQUIREMENTS_H

#include "TopTagger/TopTagger/include/TTModule.h"

class TopTaggerResults;

/**
 *This module applies the HEP top tagging criterion to the candidates as well as applying basic requirements on the number of b-jets in the candidate.  For trijet candidates the full HEP requirements are applied.  For dijet candidates the module applies a simplified version of the HEP criterion designed for use with a W and an additional jet.  The dijet and trijet categories will function with either inputs from TTMLazyClusterAlgo or TTMBasicClusterAlgo.  If the monojet mode is activated it will simply pass all AK4 monojet top candidates, but not AK8 monojet tops (this is instead done by "TTMAK8TopFilter").  Candidates passed by this module are placed into the final top list.
 *
 *@param Rmin,Rmax (float) These parameters define the valid range for the R parameter in the HEP top tagging requirements.  Under the basic assumption that the input jets are massless, this requires that ratio of dijet to trijet masses (inside a jet triplet) is consistent with the ratio of the w to top masses within the specified bounds.  This applies to the full trijet HEP requirements as well as the simplified requirements for the dijet case.  
 *@param csvThreshold (float) The minimum cut value on the CSV discriminator for an AK4 jet to be considered a b-tagged jet.
 *@param bEtaCut (float) The maximum absolute psudorapidity requirement placed on jets to be considered b-tagged.  
 *@param maxNbInTop (integer) The maximum number of b-jets to allow in a single top candidate.  
 *@param doMonojet (boolean) Enable the processing of AK4 monojet top candidates by this module (they are currently all automatically passed by this module).
 *@param doDijet (boolean) Enable the processing of AK4 dijet candidates or AK8 W + AK4 jet candidates (depending which clustering algorithm is used).
 *@param doTrijet (boolean) Enable the selection criterion for resolved AK4 trijet top candidates.  
 */
class TTMHEPRequirements : public TTModule
{
private:
    double mW_, mt_, Rmin_, Rmax_, csvThresh_, bEtaCut_;
    int maxNbInTop_;
    bool doTrijet_, doDijet_, doMonojet_;

public:
    void getParameters(const cfg::CfgDocument*, const std::string&);
    void run(TopTaggerResults&);
};
REGISTER_TTMODULE(TTMHEPRequirements);

#endif
