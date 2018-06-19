#include "TopTagger/TopTagger/include/TopObject.h"

#include "Math/VectorUtil.h"

TopObject::TopObject(std::vector<Constituent const *> constituents) : constituents_(constituents)
{
    discriminator_ = -999.9;
    updateVariables();
}

void TopObject::updateVariables()
{
    // reset sum p vector
    p_.SetPtEtaPhiM(0.0, 0.0, 0.0, 0.0);

    // calculate the total 4-vector 
    for(const auto& jet : constituents_) p_ += jet->p();

    // calculate other variables 
    dRmax_ = 0.0;
    for(const auto& jet : constituents_) 
    {
        float deltaR = ROOT::Math::VectorUtil::DeltaR(p_, jet->p());
        dRmax_ = std::max(dRmax_, deltaR);

        //If there is gen information in the constituent
        //add it to the top gen match map
        for(const auto& genMatch : jet->getGenMatches())
        {
            genMatchPossibilities_[genMatch.first].insert(genMatch.second.begin(), genMatch.second.end());
        }
    }
}

void TopObject::addConstituent(Constituent const * constituent)
{
    constituents_.push_back(constituent);
    updateVariables();
}

int TopObject::getNBConstituents(float cvsCut, float etaCut) const
{
    int nb = 0;
    for(const auto* constituent : constituents_)
    {
        if(constituent->getBTagDisc() > cvsCut && fabs(constituent->p().Eta()) < etaCut)
        {
            ++nb;
        }
    }
    return nb;
}

const TLorentzVector* TopObject::getBestGenTopMatch(const float dRMax) const
{
    //int genDaughterMatches = 0;
    const TLorentzVector* bestMatch = nullptr;
    float bestMatchDR = 999.9;
    for(const auto& genTop : genMatchPossibilities_)
    {
        if(genTop.second.size() < 2) continue;
        float deltaR = ROOT::Math::VectorUtil::DeltaR(p_, *genTop.first);
        if(deltaR < bestMatchDR)
        {
            bestMatchDR = deltaR;
            bestMatch = genTop.first;
        }
    }
    if(bestMatchDR < dRMax) return bestMatch;
    else                    return nullptr;
}
