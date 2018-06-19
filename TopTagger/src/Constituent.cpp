#include "TopTagger/TopTagger/include/Constituent.h"

Constituent::Constituent() : type_(NOTYPE), bTagDisc_(0.0), qgLikelihood_(0.0), tau1_(0.0), tau2_(0.0), tau3_(0.0), softDropMass_(0.0), wMassCorr_(0.0) {}

Constituent::Constituent(const TLorentzVector& p, const float& bTagDisc, const float& qgLikelihood) : p_(p), bTagDisc_(bTagDisc), qgLikelihood_(qgLikelihood), tau1_(-999.9), tau2_(-999.9), tau3_(-999.9), softDropMass_(-999.9), wMassCorr_(-999.9)
{
    type_ = AK4JET;
}

Constituent::Constituent(const TLorentzVector& p, const float& tau1, const float& tau2, const float& tau3, const float& softDropMass, const std::vector<TLorentzVector>& subjets, const float& wMassCorr) : p_(p), bTagDisc_(-999.9), qgLikelihood_(-999.9), tau1_(tau1), tau2_(tau2), tau3_(tau3), softDropMass_(softDropMass), subjets_(subjets), wMassCorr_(wMassCorr)
{
    type_ = AK8JET;
}

void Constituent::setPBtag(const TLorentzVector& p, const float& bTagDisc, const float& qgLikelihood)
{
    p_ = p;
    bTagDisc_ = bTagDisc;
    qgLikelihood_ = qgLikelihood;
}

void Constituent::setP(const TLorentzVector& p)
{
    p_ = p;
}

void Constituent::setBTag(const float&  bTagDisc)
{
    bTagDisc_ = bTagDisc;
}

void Constituent::setQGLikelihood(const float& qgLikelihood)
{
    qgLikelihood_ = qgLikelihood;
}

void Constituent::setType(const ConstituentType type)
{
    type_ = type;
}

void Constituent::setTau1(const float& tau1)
{
    tau1_ = tau1;
}

void Constituent::setTau2(const float& tau2)
{
    tau2_ = tau2;
}

void Constituent::setTau3(const float& tau3)
{
    tau3_ = tau3;
}

void Constituent::setSoftDropMass(const float& softDropMass)
{
    softDropMass_ = softDropMass;
}

void Constituent::setSubJets(const std::vector<TLorentzVector>& subjets)
{
    subjets_ = subjets;
}

void Constituent::setWMassCorr(const float& wMassCorr)
{
    wMassCorr_ = wMassCorr;
}

void Constituent::addGenMatch(const TLorentzVector& genTop, const TLorentzVector* genDaughter)
{
    genMatches_[&genTop].insert(genDaughter);
}
