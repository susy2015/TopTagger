#include "TopTagger/TopTagger/interface/Constituent.h"
#include "TopTagger/CfgParser/include/TTException.h"

Constituent::Constituent() : type_(NOTYPE), bTagDisc_(0.0), qgLikelihood_(0.0), tau1_(0.0), tau2_(0.0), tau3_(0.0), softDropMass_(0.0), wMassCorr_(0.0) {}

Constituent::Constituent(const TLorentzVector& p, const double& bTagDisc, const double& qgLikelihood) : p_(p), type_(AK4JET), bTagDisc_(bTagDisc), qgLikelihood_(qgLikelihood), tau1_(-999.9), tau2_(-999.9), tau3_(-999.9), softDropMass_(-999.9), wMassCorr_(-999.9)
{
}

Constituent::Constituent(const TLorentzVector& p, const ConstituentType& type) : p_(p), type_(type), bTagDisc_(-999.9), qgLikelihood_(-999.9), tau1_(-999.9), tau2_(-999.9), tau3_(-999.9), softDropMass_(-999.9), wMassCorr_(-999.9)
{
}

Constituent::Constituent(const TLorentzVector& p, const double& tau1, const double& tau2, const double& tau3, const double& softDropMass, const std::vector<Constituent>& subjets, const double& wMassCorr) : p_(p), type_(AK8JET), bTagDisc_(-999.9), qgLikelihood_(-999.9), tau1_(tau1), tau2_(tau2), tau3_(tau3), softDropMass_(softDropMass), subjets_(subjets), wMassCorr_(wMassCorr)
{
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

void Constituent::setSubJets(const std::vector<Constituent>& subjets)
{
    subjets_ = subjets;
}

void Constituent::setWMassCorr(const double& wMassCorr)
{
    wMassCorr_ = wMassCorr;
}

void Constituent::setTopDisc(const double& topDisc)
{
    topDisc_ = topDisc;
}

void Constituent::setWDisc(const double& WDisc)
{
    WDisc_ = WDisc;
}

void Constituent::setIndex(const unsigned int& index)
{
    index_ = index;
}

void Constituent::addJetRefIndex(const int& jetIndex)
{
    jetRefIndices_.push_back(jetIndex);
}

void Constituent::addGenMatch(const TLorentzVector& genTop, const TLorentzVector* genDaughter)
{
    genMatches_[&genTop].insert(genDaughter);
}

void Constituent::setQGLVars(const double qgMult, const double qgPtD, const double qgAxis1, const double qgAxis2)
{
    qgMult_ = qgMult;
    qgPtD_ = qgPtD;
    qgAxis1_ = qgAxis1;
    qgAxis2_ = qgAxis2;
}

void Constituent::setExtraVar(const std::string& name, const double var)
{
    extraVars_[name] = var;
}

double Constituent::getExtraVar(const std::string var) const 
{
    auto iter = extraVars_.find(var);

    if(iter == extraVars_.end())
    {
        THROW_TTEXCEPTION("ExtraVar: " + var + " not found!!!");
    }

    return iter->second; 
}

const std::vector<int>& Constituent::getJetRefIndicies() const
{
    return jetRefIndices_;
}
