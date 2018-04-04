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
    NOTYPE, AK4JET, AK6JET, AK8JET, CA8JET, AK8SUBJET
};

/**
 *Serves as a container class for jet objects including the 4-vector for each jet along with supporting information such as the b-tag discriminator and jet type
 */
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
    std::vector<Constituent> subjets_;
    double wMassCorr_;

    //Extra Variables
    std::map<std::string, double> extraVars_;

    //Variables for gen matching studies
    std::map<const TLorentzVector*, std::set<const TLorentzVector*>> genMatches_;

public:
    /** Empty constructor */
    Constituent();
    /** Construct an AK4 jet from TLorentzVector, b-tag discriminator, and quark-gluonlikelihood */
    Constituent(const TLorentzVector& p, const double& bTagDisc, const double& qgLikelihood);
    /** Construct generic constituent jet from TLorentzVector and type */
    Constituent(const TLorentzVector& p, const ConstituentType& type);
    /** Construct an AK8 jet from TLorentzVector, Nsubjettiness inputs tau1/2/3, softdrop mass, softdrop subjets vector and wMassCorrection (if applicable). */
    Constituent(const TLorentzVector& p, const double& tau1, const double& tau2, const double& tau3, const double& softDropMass, const std::vector<Constituent>& subjets, const double& wMassCorr);
    
    void setPBtag(const TLorentzVector& p, const double& bTagDisc, const double& qgLikelihood);
    void setP(const TLorentzVector& p);
    void setBTag(const double&  bTagDisc);
    void setQGLikelihood(const double& qgLikelihood);
    void setType(const ConstituentType type);
    void setTau1(const double& tau1);
    void setTau2(const double& tau2);
    void setTau3(const double& tau3);
    void setSoftDropMass(const double& softDropMass);
    void setSubJets(const std::vector<Constituent>& subjets);
    void setQGLVars(const double qgMult, const double qgPtD, const double qgAxis1, const double qgAxis2);
    void setWMassCorr(const double& wMassCorr);

    /** 
     *Adds an extra variable which is not included in the primary members of the Constituent class.  These will be added and retrieved by name. 
     *@param [in] name Name used to store the parameter
     *@param [in] var Value of the parameter
     */ 
    void setExtraVar(const std::string& name, const double var);

    /** 
     *Add a generator level matched particle 
     *@param [in] genTop TLorentzVector of the generator top 
     *@param [in] genDaughter TLorentzVector of the generator level daughter directly matched to the constituent 
     */
    void addGenMatch(const TLorentzVector& genTop, const TLorentzVector* genDaughter);

    /** @return Constituent TLorentzVector */
    const TLorentzVector& p() const                       { return p_; }
    /** Alias for p() */
    const TLorentzVector& P() const                       { return p(); }
    //* Alias for p() */
    const TLorentzVector& getP() const                    { return p(); }
    const double getBTagDisc() const                      { return bTagDisc_; }
    const double getQGLikelihood() const                  { return qgLikelihood_; }
    const ConstituentType getType() const                 { return type_; }
    const double getTau1() const                          { return tau1_; }
    const double getTau2() const                          { return tau2_; }
    const double getTau3() const                          { return tau3_; }
    const double getSoftDropMass() const                  { return softDropMass_; }
    const decltype(subjets_)& getSubjets() const          { return subjets_; }
    const decltype(genMatches_)& getGenMatches() const    { return genMatches_; }
    const double getWMassCorr() const                     { return wMassCorr_; }
    const double getQGMult() const                        { return qgMult_; }
    const double getQGPtD() const                         { return qgPtD_; }
    const double getQGAxis1() const                       { return qgAxis1_; }
    const double getQGAxis2() const                       { return qgAxis2_; }
    /** 
     *Retrieve an extra variable based upon its name 
     *@param [in] var Name of the variable to retrieve
     *@return Value of the variable
     */
    const double getExtraVar(const std::string var) const;
};

#endif
