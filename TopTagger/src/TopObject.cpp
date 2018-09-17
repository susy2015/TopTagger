#include "TopTagger/TopTagger/include/TopObject.h"

#include "Math/VectorUtil.h"

TopObject::TopObject() : dRmax_(999.9), discriminator_(-999.9), dThetaMin_(999.9), dThetaMax_(-999.9), scaleFactor_(0.0), type_(TopObject::NONE)
{
}

TopObject::TopObject(std::vector<Constituent const *> constituents, const Type& type) : constituents_(constituents), type_(type)
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
    dThetaMin_ = 9999.0;
    dThetaMax_ = 0.0;
    for(const auto& jet : constituents_) 
    {
        double deltaR = ROOT::Math::VectorUtil::DeltaR(p_, jet->p());
        double deltaTheta = p_.Angle(jet->p().Vect());
        dRmax_ = std::max(dRmax_, deltaR);
        dThetaMin_ = std::min(dThetaMin_, deltaTheta);
        dThetaMax_ = std::max(dThetaMax_, deltaTheta);

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

int TopObject::getNBConstituents(double cvsCut, double etaCut) const
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

const TLorentzVector* TopObject::getBestGenTopMatch(const double dRMax) const
{
    //int genDaughterMatches = 0;
    const TLorentzVector* bestMatch = nullptr;
    double bestMatchDR = 999.9;
    for(const auto& genTop : genMatchPossibilities_)
    {
        if(genTop.second.size() < 2) continue;
        double deltaR = ROOT::Math::VectorUtil::DeltaR(p_, *genTop.first);
        if(deltaR < bestMatchDR)
        {
            bestMatchDR = deltaR;
            bestMatch = genTop.first;
        }
    }
    if(bestMatchDR < dRMax) return bestMatch;
    else                    return nullptr;
}

double TopObject::getMCScaleFactor() const
{
    return 0.0;
}

double TopObject::getSystematicUncertainty(const std::string& source) const
{
    return 0.0;
}
