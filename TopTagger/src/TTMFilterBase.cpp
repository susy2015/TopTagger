#include "TopTagger/TopTagger/include/TTMFilterBase.h"

#include "TopTagger/TopTagger/include/Constituent.h"
#include "TopTagger/CfgParser/include/Context.hh"
#include "TopTagger/CfgParser/include/CfgDocument.hh"

#include "Math/VectorUtil.h"

bool TTMFilterBase::constituentsAreUsed(const std::vector<const Constituent*>& constituents, const std::set<const Constituent*>& usedConsts, const float dRMax) const
{
    for(const auto& constituent : constituents)
    {
        if(usedConsts.count(constituent) > 0)
        {
            //First return true if constituent is found (this covers all AK4 and most AK8 jets)
            return true;
        }
        else if(constituent->getType() == AK8JET)
        {
            //If the constituent is AK8 we will also check its subjets are not used
            for(const auto& subjet : constituent->getSubjets())
            {
                for(const auto& usedConstituent : usedConsts)
                {
                    if(ROOT::Math::VectorUtil::DeltaR(subjet, usedConstituent->p()) < dRMax)
                    {
                        //we found a match
                        return true;
                    }
                }
            }
        }
    }

    //if nothing is found then we have an unused jet
    return false;
}

void TTMFilterBase::markConstituentsUsed(const std::vector<const Constituent *>& constituents, const std::vector<Constituent>& allConstituents, std::set<const Constituent*>& usedConstituents, const float dRMax) const
{
    for(const auto& constituent : constituents)
    {
        //No matter what, add the main constituent
        usedConstituents.insert(constituent);

        //If the constituent is an AK8JET, then add AK4JETs matching its subjets as well 
        if(constituent->getType() == AK8JET)
        {
            for(const auto& subjet : constituent->getSubjets())
            {
                for(const auto& matchConst : allConstituents) 
                {
                    if(ROOT::Math::VectorUtil::DeltaR(subjet, matchConst.p()) < dRMax)
                    {
                        usedConstituents.insert(&matchConst);
                    }
                }
            }
        }
    }
}


