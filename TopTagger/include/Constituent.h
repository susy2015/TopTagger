#ifndef CONSTITUENT_H
#define CONSTITUENT_H

#include "TLorentzVector.h"

enum ConstituentType
{
    AK4JET, AK6JET, AK8JET, CA8JET
};

class Constituent
{
private:
    TLorentzVector p_;
    double bTagDisc_, qgLikelihood_, jetChrg_;

public:
    Constituent();
    Constituent(const TLorentzVector& p, const double& bTagDisc, const double& qgLikelihood, const double& jetChrg);
    
    void setPBtag(const TLorentzVector& p, const double& bTagDisc, const double& qgLikelihood);
    void setP(const TLorentzVector& p);
    void setBTag(const double&  bTagDisc);
    void setQGLikelihood(const double& qgLikelihood);
    void setJetCharge(const double& jetChrg);

    const TLorentzVector& p() const      { return p_; }
    const TLorentzVector& P() const      { return p(); }
    const double getBTagDisc() const     { return bTagDisc_; }
    const double getQGLikelihood() const { return qgLikelihood_; }
    const double getJetCharge() const { return jetChrg_; }
};

#endif
