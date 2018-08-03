#ifndef TTMBASICCLUSTERALGO_H
#define TTMBASICCLUSTERALGO_H

#include "TopTagger/TopTagger/include/TTModule.h"
#include "TopTagger/TopTagger/include/TTMConstituentReqs.h"

class TopTaggerResults;
class TopObject;
class Constituent;

/**
 *This module is used to cluster top candidates using a combination of AK4 and AK8 jets.  This algorithm uses AK8 jets for the merged W and top candidates, and AK4 jets for the resolved category as well as to combine with W jets to for top candidates.  This module is capable of clustering trijet (resolved tops), dijet (W+jet), and monojet (fully merged top) candidates.
 *
 *@param doTrijet (bool) Enable the resolved top category clustering.
 *@param minTopCandMass (float) Minimum trijet mass for resolved candidates
 *@param maxTopCandMass (float) Maximum trijet mass for resolved candidates
 *@param dRMaxTrijet (float) The maximum dR seperation  between any AK4 jet and te trijet centroid for resolved AK4 candidates to be considered as a top candidate. 
 *@param nbSeed (int) If greater than 0, the number of highest b-tag discriminator jets to use as clustering seeds for trijet candidates. (Default -1)
 *@param minTrijetAK4JetPt (float) Minimum pt threshold for the lowest pt AK4 jet in th etrijet candidate.
 *@param midTrijetAK4JetPt (float) Minimum pt threshold for the medium pt AK4 jet in th etrijet candidate.
 *@param maxTrijetAK4JetPt (float) Minimum pt threshold for the highest pt AK4 jet in th etrijet candidate.
 *@param doDijet (bool) Enable the AK8W + AK4 jet category clustering.
 *@param dRMaxDijet (float) The maximum allowed seperation if dR between the AK4 and AK8 jet and the dijet centroid for the dijet catagory.  
 *@param doMonojet (bool) Enable the fully merged top category clustering.
 *@param useDeepAK8 (bool) Use deepAK8 discriminator to identify boosted objects from AK8 jets instead of NSubjettiness.
 *
 *See TTMConstituentReqs for more parameters
 */
class TTMBasicClusterAlgo : public TTModule, public TTMConstituentReqs
{
private:
    //mino-jet variables
    bool doMonojet_;
    bool useDeepAK8_;

    //dijet variables
    double dRMaxDiJet_;
    bool doDijet_;

    //trijet variables
    double minTopCandMass_, maxTopCandMass_, dRMaxTrijet_, minTrijetAK4JetPt_, midTrijetAK4JetPt_, maxTrijetAK4JetPt_;
    bool doTrijet_;
    int nbSeed_;

    void fillTriplet(const Constituent* const, const Constituent* const, const Constituent* const, std::vector<TopObject>&);

public:
    void getParameters(const cfg::CfgDocument*, const std::string&);
    void run(TopTaggerResults&);
};
REGISTER_TTMODULE(TTMBasicClusterAlgo);

#endif
