#ifndef TOPOBJECT_H
#define TOPOBJECT_H

#include <vector>

#include "TLorentzVector.h"

#include "Constituent.h"

class TopObject 
{
private:
    TLorentzVector p_;
    double dRmax_;
    std::vector<Constituent const *> constituents_;

    void updateVariables();

public:
    TopObject() {};
    TopObject(std::vector<Constituent const *> constituents);
    
    void addConstituent(Constituent const *  constituent);

    const TLorentzVector& p() const { return p_; }
    const TLorentzVector& P() const { return p(); }
    double getDRmax() const { return dRmax_; }

    const std::vector<Constituent const *>& getConstituents() const { return constituents_; }
    int getNConstituents() const { return constituents_.size(); }
};

#endif
