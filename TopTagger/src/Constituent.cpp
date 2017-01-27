#include "TopTagger/TopTagger/include/Constituent.h"

Constituent::Constituent() : type_(NOTYPE), bTagDisc_(0.0), qgLikelihood_(0.0), tau1_(0.0), tau2_(0.0), tau3_(0.0), softDropMass_(0.0), wMassCorr_(1.0) {}

Constituent::Constituent(const TLorentzVector& p, const double& bTagDisc, const double& qgLikelihood) : p_(p), bTagDisc_(bTagDisc), qgLikelihood_(qgLikelihood), tau1_(-999.9), tau2_(-999.9), tau3_(-999.9), softDropMass_(-999.9), wMassCorr_(-999.9)
{
    type_ = AK4JET;
}

Constituent::Constituent(const TLorentzVector& p, const double& tau1, const double& tau2, const double& tau3, const double& softDropMass, const std::vector<TLorentzVector>& subjets, const double& wMassCorr) : p_(p), bTagDisc_(-999.9), qgLikelihood_(-999.9), tau1_(tau1), tau2_(tau2), tau3_(tau3), softDropMass_(softDropMass), subjets_(subjets), wMassCorr_(wMassCorr)
{
    type_ = AK8JET;

    updatePCorr();
}

void Constituent::updatePCorr()
{
    if(type_ == AK8JET) pCorr_.SetPtEtaPhiM(p_.Pt(), p_.Eta(), p_.Phi(), getSoftDropMass());
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
    updatePCorr();
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
    updatePCorr();
}

void Constituent::setSubJets(const std::vector<TLorentzVector>& subjets)
{
    subjets_ = subjets;
}

void Constituent::setWMassCorr(const double& wMassCorr)
{
    wMassCorr_ = wMassCorr;
    updatePCorr();
}

void Constituent::addGenMatch(const TLorentzVector& genTop, const TLorentzVector* genDaughter)
{
    genMatches_[&genTop].insert(genDaughter);
}
