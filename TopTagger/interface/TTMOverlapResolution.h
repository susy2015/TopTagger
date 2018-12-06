#ifndef TTMOVERLAPRESOLUTION_H
#define TTMOVERLAPRESOLUTION_H

#include "TopTagger/TopTagger/interface/TTModule.h"
#include "TopTagger/TopTagger/interface/TTMFilterBase.h"
#include "TopTagger/TopTagger/interface/TopObject.h"

#include <string>
#include <functional>

class TopTaggerResults;

/**
 *After top candidates are added to the final top list it is possible that some constituent jets can be included in more than one top in the list.  This module is used to remove the tops which share constituents.  It functions by first sorting the list of tops by a figure of merit, and then in the case that 2 tops share a constituent, the top with the better figure of merit is removed.  The module can by run on the entire list of tops, or it can be run category by category.
 *
 *@param mt (float) <b> Common context </b> Mass of the top quark
 *@param maxTopEta (float) <b> Common context </b> Maximum |eta| of a top candidate to be considered a final reconstructed top
 *@param dRMatch (float) <b> Common context </b> The dR requirement used to select whether an AK4 jet matches a AK8 subjet
 *@param cvsThreshold (float) Minimum cut value on the CSV discriminator for an AK4 jet to be considered a b-tagged jet
 *@param NConstituents (int) This parameter determines which tops to include in the overlap resolution process. The options include "-1" for all tops, or 1, 2, or 3, for monojet, dijet, or trijet categories respectively.  
 *@param sortMethod (string) The initial sorting order used to prioritize one top over another when resolving overlapping.  The options include "topMass", "topPt", "mvaDisc", "mvaDiscWithb", and "none".  
 */
class TTMOverlapResolution : public TTModule, public TTMFilterBase
{
private:
    double mt_, maxTopEta_, dRMatch_, dRMatchAK8_, cvsThreshold_;
    TopObject::Type type_;
    std::string sortMethod_;
    std::function<bool (const TopObject* t1, const TopObject* t2)> sortFunc_;
    bool doSort_;

public:
    void getParameters(const cfg::CfgDocument*, const std::string&);
    void run(TopTaggerResults&);
};
REGISTER_TTMODULE(TTMOverlapResolution);

#endif
