#ifndef TTMTENSORFLOW_H
#define TTMTENSORFLOW_H

#include "TopTagger/TopTagger/interface/TTModule.h"
#include "TopTagger/TopTagger/interface/TopObject.h"

#include <string>
#include <memory>
#include <vector>

/**
 *This module implements an interface to Tensorflow through the tensorflow c-api for filtering top candidates.  This module places top candidates which pass the requirements directly into the final top list.
 *
 *@param discCut (float) Highest minimum discriminator threshold allowed (If discOffest is set > 1 and discSlope is positive then this serves as a basic discriminator threshold)
 *@param discOffset (float) Discriminator cut for zero pt top candidates 
 *@param discSlope (float) Pt dependent slopt for discriminator cut
 *@param type (int) Type of constituent to apply selection to (1 - monojet, 2 - dijet, 3 - trijet)
 *@param bdiscThreshold (float) Threshold on b-tag discriminator to be considered a b-jet.  
 *@param bEtaCut (float) Requirment on |eta| for a constituent to be considered a b-jet
 *@param maxNbInTop (int) The maximum number of constituent jets which can be b-tagged for the candidate to be a final top (set < 0 to disable)
 */
class TTMDiscriminatorFilter : public TTModule
{
private:
    double discriminator_;
    double discOffset_;
    double discSlope_;
    double bdiscThreshold_;
    double bEtaCut_;
    int maxNbInTop_;
    TopObject::Type type_;

public:
    void getParameters(const cfg::CfgDocument*, const std::string&);
    void run(TopTaggerResults&);
};
REGISTER_TTMODULE(TTMDiscriminatorFilter);

#endif
