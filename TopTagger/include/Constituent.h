#ifndef CONSTITUENT_H
#define CONSTITUENT_H

#include "TLorentzVector.h"

#include <vector>

enum ConstituentType
{
    AK4JET, AK6JET, AK8JET, CA8JET
};

class Constituent
{
private:
    TLorentzVector p_;
    ConstituentType type_;

    //AK4 specific variables 
    double bTagDisc_, qgLikelihood_;

    //AK8 specific variables 
    double tau1_, tau2_, tau3_, softDropMass_;
    std::vector<TLorentzVector> subjets_;

public:
    Constituent();
    Constituent(const TLorentzVector& p, const double& bTagDisc, const double& qgLikelihood);
    Constituent(const TLorentzVector& p, const double& tau1, const double& tau2, const double& tau3, const double& softDropMass, const std::vector<TLorentzVector>& subjets);
    
    void setPBtag(const TLorentzVector& p, const double& bTagDisc, const double& qgLikelihood);
    void setP(const TLorentzVector& p);
    void setBTag(const double&  bTagDisc);
    void setQGLikelihood(const double& qgLikelihood);
    void setType(const ConstituentType type);
    void setTau1(const double& tau1);
    void setTau2(const double& tau2);
    void setTau3(const double& tau3);
    void setSoftDropMass(const double& softDropMass);
    void setSubJets(const std::vector<TLorentzVector>& subjets);

    const TLorentzVector& p() const                       { return p_; }
    const TLorentzVector& P() const                       { return p(); }
    const TLorentzVector& getP() const                    { return p(); }
    const double getBTagDisc() const                      { return bTagDisc_; }
    const double getQGLikelihood() const                  { return qgLikelihood_; }
    const ConstituentType getType() const                 { return type_; }
    const double getTau1() const                          { return tau1_; }
    const double getTau2() const                          { return tau2_; }
    const double getTau3() const                          { return tau3_; }
    const double getSoftDropMass() const                  { return softDropMass_; }
    const std::vector<TLorentzVector>& getSubjets() const { return subjets_; }
};

#endif
