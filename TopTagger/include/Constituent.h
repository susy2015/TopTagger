#ifndef CONSTITUENT_H
#define CONSTITUENT_H

#include "TLorentzVector.h"

#include <vector>
#include <utility>
#include <set>
#include <map>

enum ConstituentType
{
    NOTYPE, AK4JET, AK6JET, AK8JET, CA8JET
};

class Constituent
{
private:
    TLorentzVector p_;
    ConstituentType type_;

    //AK4 specific variables 
    float bTagDisc_, qgLikelihood_;

    //AK8 specific variables 
    float tau1_, tau2_, tau3_, softDropMass_;
    std::vector<TLorentzVector> subjets_;
    float wMassCorr_;

    //Variables for gen matching studies
    std::map<const TLorentzVector*, std::set<const TLorentzVector*>> genMatches_;

public:
    Constituent();
    Constituent(const TLorentzVector& p, const float& bTagDisc, const float& qgLikelihood);
    Constituent(const TLorentzVector& p, const float& tau1, const float& tau2, const float& tau3, const float& softDropMass, const std::vector<TLorentzVector>& subjets, const float& wMassCorr);
    
    void setPBtag(const TLorentzVector& p, const float& bTagDisc, const float& qgLikelihood);
    void setP(const TLorentzVector& p);
    void setBTag(const float&  bTagDisc);
    void setQGLikelihood(const float& qgLikelihood);
    void setType(const ConstituentType type);
    void setTau1(const float& tau1);
    void setTau2(const float& tau2);
    void setTau3(const float& tau3);
    void setSoftDropMass(const float& softDropMass);
    void setSubJets(const std::vector<TLorentzVector>& subjets);
    void setWMassCorr(const float& wMassCorr);

    void addGenMatch(const TLorentzVector& genTop, const TLorentzVector* genDaughter);

    const TLorentzVector& p() const                       { return p_; }
    const TLorentzVector& P() const                       { return p(); }
    const TLorentzVector& getP() const                    { return p(); }
    const float getBTagDisc() const                      { return bTagDisc_; }
    const float getQGLikelihood() const                  { return qgLikelihood_; }
    const ConstituentType getType() const                 { return type_; }
    const float getTau1() const                          { return tau1_; }
    const float getTau2() const                          { return tau2_; }
    const float getTau3() const                          { return tau3_; }
    const float getSoftDropMass() const                  { return softDropMass_; }
    const std::vector<TLorentzVector>& getSubjets() const { return subjets_; }
    const decltype(genMatches_)& getGenMatches() const    { return genMatches_; }
    const float getWMassCorr() const                     { return wMassCorr_; }
};

#endif
