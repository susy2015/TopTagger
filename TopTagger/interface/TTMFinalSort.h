#ifndef TTMFINALSORT_H
#define TTMFINALSORT_H

#include "TopTagger/TopTagger/interface/TTModule.h"
#include "TopTagger/TopTagger/interface/TTMFilterBase.h"

#include <string>

class TopTaggerResults;

/**
 *This module is used to sort the final list of tops after overlap resolution.  
 *
 *@param mt (float) <b> Common context </b> Mass of the top quark
 *@param sortMethod (string) This parameter defines the sorting order.  The possible options are "topMass", "topPt", and "none".
*/
class TTMFinalSort : public TTModule
{
private:
    double mt_;
    std::string sortMethod_;

public:
    void getParameters(const cfg::CfgDocument*, const std::string&);
    void run(TopTaggerResults&);
};
REGISTER_TTMODULE(TTMFinalSort);

#endif
