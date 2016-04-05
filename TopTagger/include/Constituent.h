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
    double bTagDisc_;

public:
    Constituent();
    Constituent(const TLorentzVector& p, const double& bTagDisc);
    
    void setPBtag(const TLorentzVector& p, const double& bTagDisc);
    void setP(const TLorentzVector& p);
    void setBTag(const double&  bTagDisc);

    const TLorentzVector& p() const  { return p_; }
    const TLorentzVector& P() const  { return p(); }
    const double getBTagDisc() const { return bTagDisc_; }
};

#endif
