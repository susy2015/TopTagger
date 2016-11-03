#include "TopTagger/TopTagger/include/Constituent.h"

Constituent::Constituent() : bTagDisc_(0.0), qgLikelihood_(0.0) {}

Constituent::Constituent(const TLorentzVector& p, const double& bTagDisc, const double& qgLikelihood) : p_(p), bTagDisc_(bTagDisc), qgLikelihood_(qgLikelihood) 
{
    type_ = AK4JET;
}

Constituent::Constituent(const TLorentzVector& p, const double& tau1, const double& tau2, const double& tau3, const double& softDropMass) : p_(p), tau1_(tau1), tau2_(tau2), tau3_(tau3), softDropMass_(softDropMass)
{
    type_ = AK8JET;
}

void Constituent::setPBtag(const TLorentzVector& p, const double& bTagDisc, const double& qgLikelihood)
{
    p_ = p;
    bTagDisc_ = bTagDisc;
    qgLikelihood_ = qgLikelihood;
}

void Constituent::setP(const TLorentzVector& p)
{
    p_ = p;
}

void Constituent::setBTag(const double&  bTagDisc)
{
    bTagDisc_ = bTagDisc;
}

void Constituent::setQGLikelihood(const double& qgLikelihood)
{
    qgLikelihood_ = qgLikelihood;
}

void Constituent::setType(const ConstituentType type)
{
    type_ = type;
}

void Constituent::setTau1(const double& tau1)
{
    tau1_ = tau1;
}

void Constituent::setTau2(const double& tau2)
{
    tau2_ = tau2;
}

void Constituent::setTau3(const double& tau3)
{
    tau3_ = tau3;
}

void Constituent::setSoftDropMass(const double& softDropMass)
{
    softDropMass_ = softDropMass;
}

