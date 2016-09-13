#include "TopTagger/TopTagger/include/Constituent.h"

Constituent::Constituent() : bTagDisc_(0.0), qgLikelihood_(0.0) {}

Constituent::Constituent(const TLorentzVector& p, const double& bTagDisc, const double& qgLikelihood) : p_(p), bTagDisc_(bTagDisc), qgLikelihood_(qgLikelihood) {}

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

