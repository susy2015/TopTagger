#ifndef TOPOBJECT_H
#define TOPOBJECT_H

#include <vector>
#include <map>
#include <set>

#include "TLorentzVector.h"

#include "TopTagger/TopTagger/include/Constituent.h"

class TopObject 
{
private:
    TLorentzVector p_;
    float dRmax_, discriminator_;
    std::vector<Constituent const *> constituents_;

    void updateVariables();

    std::map<const TLorentzVector*, std::set<const TLorentzVector*>> genMatchPossibilities_;

public:
    TopObject() {};
    TopObject(std::vector<Constituent const *> constituents);
    
    void addConstituent(Constituent const *  constituent);
    void setDiscriminator(const float disc) { discriminator_ = disc; }

    const TLorentzVector& p() const { return p_; }
    const TLorentzVector& P() const { return p(); }
    float getDRmax() const { return dRmax_; }
    float getDiscriminator() const { return discriminator_; }

    const std::vector<Constituent const *>& getConstituents() const { return constituents_; }
    int getNConstituents() const { return constituents_.size(); }
    int getNBConstituents(float cvsCut, float etaCut = 2.4) const;

    const decltype(genMatchPossibilities_)& getGenTopMatches() const { return genMatchPossibilities_; }
    const TLorentzVector* getBestGenTopMatch(const float dRMax = 0.6) const;
};

#endif
