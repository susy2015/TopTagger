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
    double dRmax_, discriminator_;
    std::vector<Constituent const *> constituents_;

    void updateVariables();

    std::map<const TLorentzVector*, std::set<const TLorentzVector*>> genMatchPossibilities_;
    int NGenMatchConstituents_;

public:
    TopObject() {};
    TopObject(std::vector<Constituent const *> constituents);
    
    void addConstituent(Constituent const *  constituent);
    void setDiscriminator(const double disc) { discriminator_ = disc; }

    const TLorentzVector& p() const { return p_; }
    const TLorentzVector& P() const { return p(); }
    double getDRmax() const { return dRmax_; }
    double getDiscriminator() const { return discriminator_; }

    const std::vector<Constituent const *>& getConstituents() const { return constituents_; }
    int getNConstituents() const { return constituents_.size(); }
    int getNBConstituents(double cvsCut) const;

    const decltype(genMatchPossibilities_)& getGenTopMatches() const { return genMatchPossibilities_; }
    const TLorentzVector* getBestGenTopMatch(const double dRMax = 0.6) const;
};

#endif
