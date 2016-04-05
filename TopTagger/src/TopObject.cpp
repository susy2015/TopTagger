#include "TopTagger/TopTagger/include/TopObject.h"

#include "Math/VectorUtil.h"

TopObject::TopObject(std::vector<Constituent const *> constituents) : constituents_(constituents)
{
    updateVariables();
}

void TopObject::updateVariables()
{
    //reset sum p vector
    p_.SetPtEtaPhiM(0.0, 0.0, 0.0, 0.0);

    // calculate the total 4-vector 
    for(const auto& jet : constituents_) p_ += jet->p();

    // calculate other variables 
    dRmax_ = 0.0;
    for(const auto& jet : constituents_) 
    {
        double deltaR = ROOT::Math::VectorUtil::DeltaR(p_, jet->p());
        dRmax_ = std::max(dRmax_, deltaR);
    }
}

void TopObject::addConstituent(Constituent const * constituent)
{
    constituents_.push_back(constituent);
    updateVariables();
}
