#include "TopTagger/TopTagger/include/TTMFinalSort.h"

#include "TopTagger/TopTagger/include/TopTaggerResults.h"
#include "TopTagger/CfgParser/include/Context.hh"
#include "TopTagger/CfgParser/include/TTException.h"
#include "TopTagger/CfgParser/include/CfgDocument.hh"

#include <algorithm>
#include <vector>
#include <cmath>

void TTMFinalSort::getParameters(const cfg::CfgDocument* cfgDoc, const std::string& localContextName)
{
    //Construct contexts
    cfg::Context commonCxt("Common");
    cfg::Context localCxt(localContextName);

    mt_         = cfgDoc->get("mt",         commonCxt, -999.9);

    sortMethod_    = cfgDoc->get("sortMethod",    localCxt,  "topPt");
}

void TTMFinalSort::run(TopTaggerResults& ttResults)
{
    //Get vector of final tops to sort
    std::vector<TopObject*>& tops = ttResults.getTops();
    std::map<TopObject::Type, std::vector<TopObject*>>& topsByType = ttResults.getTopsByType();

    //Sort the top vector for overlap resolution
    if(sortMethod_.compare("topMass") == 0)
    {
        std::sort(tops.begin(), tops.end(), [this](TopObject* t1, TopObject* t2){ return fabs(t1->p().M() - this->mt_) < fabs(t2->p().M() - this->mt_); } );
    }
    else if(sortMethod_.compare("topPt") == 0)
    {
        std::sort(tops.begin(), tops.end(), [this](TopObject* t1, TopObject* t2){ return t1->p().Pt() > t2->p().Pt(); } );
    }
    else if(sortMethod_.compare("none") == 0)
    {
        //do nothing 
    }
    else
    {
        THROW_TTEXCEPTION("Invalid sorting option");
    }

    //After final sort, create the catagory seperated version of the final top collection 
    for(TopObject* top : tops) topsByType[top->getType()].push_back(top);
}
