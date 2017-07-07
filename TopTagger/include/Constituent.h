#ifndef CONSTITUENT_H
#define CONSTITUENT_H

#include "TLorentzVector.h"

#include <vector>
#include <utility>
#include <set>
#include <map>
#include <string>

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
    double bTagDisc_, qgLikelihood_;
    double qgMult_, qgPtD_, qgAxis1_, qgAxis2_;

    //AK8 specific variables 
    double tau1_, tau2_, tau3_, softDropMass_;
    std::vector<TLorentzVector> subjets_;
    double wMassCorr_;

    //Extra Variables
    std::map<std::string, double> extraVars_;

    //Variables for gen matching studies
    std::map<const TLorentzVector*, std::set<const TLorentzVector*>> genMatches_;

public:
    Constituent();
    Constituent(const TLorentzVector& p, const double& bTagDisc, const double& qgLikelihood);
    Constituent(const TLorentzVector& p, const double& tau1, const double& tau2, const double& tau3, const double& softDropMass, const std::vector<TLorentzVector>& subjets, const double& wMassCorr);
    
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
    void setQGLVars(const double qgMult, const double qgPtD, const double qgAxis1, const double qgAxis2);
    void setWMassCorr(const double& wMassCorr);
    void setExtraVar(const std::string& name, const double var);

    void addGenMatch(const TLorentzVector& genTop, const TLorentzVector* genDaughter);

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
    const decltype(genMatches_)& getGenMatches() const    { return genMatches_; }
    const double getWMassCorr() const                     { return wMassCorr_; }
    const double getQGMult() const                        { return qgMult_; }
    const double getQGPtD() const                         { return qgPtD_; }
    const double getQGAxis1() const                       { return qgAxis1_; }
    const double getQGAxis2() const                       { return qgAxis2_; }
    const double getExtraVar(const std::string var) const;
};

#endif
