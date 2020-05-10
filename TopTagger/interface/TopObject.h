#ifndef TOPOBJECT_H
#define TOPOBJECT_H

#include <vector>
#include <utility>
#include <map>
#include <set>
#include <memory>

#include "TLorentzVector.h"

#include "TopTagger/TopTagger/interface/Constituent.h"
#include "TopTagger/CfgParser/interface/TTException.h"

/** 
 *  Container class which represents a top candidate or final selected top object.  
 */

class TopObject 
{
public:
    /**
     * Enum to select what flavor of object this TopObject is.
     */
    enum Type
    {
        NONE, MERGED_TOP, SEMIMERGEDWB_TOP, RESOLVED_TOP, MERGED_W, SEMIMERGEDQB_TOP, NTYPE, ANY
    };

private:
    TLorentzVector p_;
    double dRmax_, discriminator_, dThetaMin_, dThetaMax_, scaleFactor_;

    std::vector<Constituent const *> constituents_;

    Type type_;

    void updateVariables();

    std::map<const TLorentzVector*, std::set<const TLorentzVector*>> genMatchPossibilities_;
    std::map<std::string, double> systematicUncertainties_;

    std::shared_ptr<std::map<std::string, float>> inputMVAVars_;

public:
    /// Construct default empty TopObject
    TopObject();
    /// Construct a TopObject from a vector of constituent jets.  
    TopObject(std::vector<Constituent const *> constituents, const Type& type = NONE);
    
    /// Add a new constituent to the TopObject 
    void addConstituent(Constituent const *  constituent);
    /// Set the Top discriminator for this candidate
    void setDiscriminator(const double disc) { discriminator_ = disc; }

    /// Set the scale factor used to scale simulation events so that they better match data.
    void setMCScaleFactor(const double& sf) { scaleFactor_ = sf; } 
    /// Set a systematic uncertainty 
    void setSystematicUncertainty(const std::string& source, const double& value) { systematicUncertainties_[source] = value; } 

    /// Returns the 4-vector momentum of the top candidate
    const TLorentzVector& p() const { return p_; }
    /// Returns the 4-vector momentum of the top candidate
    const TLorentzVector& P() const { return p(); }
    /// Returns the maximum dR seperation between the overall top candidate and any of the individual constituents
    double getDRmax() const { return dRmax_; }
    /// Returns the minimum angular seperation between the top candidate and any of the individual constituents 
    double getDThetaMin() const { return dThetaMin_; }
    /// Returns the maximum angular seperation between the top candidate and any of the individual constituents 
    double getDThetaMax() const { return dThetaMax_; }
    /// Returns the top discriminator for this candidate 
    double getDiscriminator() const { return discriminator_; }
    /// Returnes the type of top
    Type getType() const { return type_; }

    /// Returns the internal vector of constituents which are used to construct the TopObject
    const std::vector<Constituent const *>& getConstituents() const { return constituents_; }
    /// The number of constituents in this TopObject
    int getNConstituents() const { return constituents_.size(); }
    /// The number of b-tagged constituents based on the b-tagging discriminator cut and the jet eta
    int getNBConstituents(double cvsCut, double etaCut = 2.4) const;

    /// Returns the list of all possible generator level tops which could be a match to the TopObject.  This requires that generator level information is passed to the top tagger.  
    const decltype(genMatchPossibilities_)& getGenTopMatches() const { return genMatchPossibilities_; }
    /// Returns the best matched genrator level top based on the possible matches based on the parameter dRMax which defines the matching cone between the overall generator top and the top candidate.
    const TLorentzVector* getBestGenTopMatch(const double dRMax = 0.6, const int NGenPartMatch = 2, const int NConstituentMatch = -1) const;

    /// Return the scale factor used to scale simulation events so that they better match data.
    double getMCScaleFactor() const { return scaleFactor_; }
    /// Return systematic uncertainty 
    double getSystematicUncertainty(const std::string& source) const;
    /// Return all systematic uncertainties 
    const decltype(systematicUncertainties_)& getAllSystematicUncertainties() const { return systematicUncertainties_; }
    /// Return total systematic uncertainty 
    std::pair<double, double> getTotalSystematicUncertainty() const; 

    /// Store the MVA input variables in the TopObject, this is only used for debug purposes 
    template<typename IterType>
    void storeMVAInputs(const std::vector<std::string>& names, const IterType& begin, const IterType& end)
    {
        inputMVAVars_.reset(new std::map<std::string, float>());
        
        int i = 0;
        for(IterType iter = begin; iter != end; ++iter, ++i)
        {
            (*inputMVAVars_)[names[i]] = *iter;
        }
    }
    
    /// Get the MVA input variables in the TopObject, this is only used for debug purposes 
    std::map<std::string, float>& getMVAInputs() const { return *inputMVAVars_; }
};

#endif
