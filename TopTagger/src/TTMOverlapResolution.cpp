#include "TopTagger/TopTagger/include/TTMOverlapResolution.h"

#include "TopTagger/TopTagger/include/TopTaggerResults.h"
#include "TopTagger/CfgParser/include/Context.hh"
#include "TopTagger/CfgParser/include/CfgDocument.hh"

#include <set>
#include <algorithm>
#include <vector>
#include <cmath>

void TTMOverlapResolution::getParameters(const cfg::CfgDocument* cfgDoc)
{
    //Construct contexts
    cfg::Context commonCxt("Common");
    cfg::Context localCxt("TTMOverlapResolution");

    mt_                  = cfgDoc->get("mt",                  commonCxt, -999.9);
    minTopCandMass_      = cfgDoc->get("minTopCandMass",      localCxt,  -999.9);
    maxTopCandMass_      = cfgDoc->get("maxTopCandMass",      localCxt,  -999.9);
    minTopCandMassLoose_ = cfgDoc->get("minTopCandMassLoose", localCxt,  -999.9);
    maxTopCandMassLoose_ = cfgDoc->get("maxTopCandMassLoose", localCxt,  -999.9);
    maxTopEta_           = cfgDoc->get("maxTopEta",           localCxt,  -999.9);
}

void TTMOverlapResolution::run(TopTaggerResults& ttResults)
{
    //Get vector of final tops to prune
    std::vector<TopObject*>& tops = ttResults.getTops();

    //Sort the top vector by fabs(candMass - trueTopMass)
    std::sort(tops.begin(), tops.end(), [this](TopObject* t1, TopObject* t2){ return fabs(t1->p().M() - this->mt_) < fabs(t2->p().M() - this->mt_);});

    //This variable is necessary to account for bug in Hongxuan's code
    int nTops = 0;

    //This container will kep trach of which jets have been included in final tops
    //This is not the ideal method to track this, maybe have candidates?
    std::set<Constituent const *> usedJets;

    for(auto iTop = tops.begin(); iTop != tops.end();)
    {
        //Get cinstituent jets for this top
        const std::vector<Constituent const *>& jets = (*iTop)->getConstituents();

        //mass window on the top candidate mass
        double m123 = (*iTop)->p().M();
        bool passMassWindow      = (minTopCandMass_ < m123) && (m123 < maxTopCandMass_);
        bool passLooseMassWindow = (minTopCandMassLoose_ < m123) && (m123 < maxTopCandMassLoose_);

        //Requirement on top eta here
        //MORE TERRIBLE HARDCODING 
        bool passTopEta = (fabs((*iTop)->p().Eta()) < maxTopEta_);

        //Check if the candidates have been used in another top with better top mass
        bool overlaps = false;
        for(const auto& jet : jets)
        {
            if(usedJets.count(jet))
            {
                overlaps = true;
                break;
            }
        }

        //Prune top from final top collection if it fails the following requirements
        //Note the terrible hack with nTops >= 3 to try to match Hongxuan's code
        if(overlaps || ((nTops < 3)?(!passMassWindow || !passTopEta):(!passLooseMassWindow)) )
        {
            //This is inefficient and dumb
            iTop = tops.erase(iTop);
            continue;
        }

        //If the candidate survived, it must be a good top!!!

        ++nTops;

        //Add the good tops constituents to the set traching which constituents have been used
        for(const auto& jet : jets)
        {
            usedJets.insert(jet);
        }

        ++iTop;
    }
}
