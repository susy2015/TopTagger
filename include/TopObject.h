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

    const TLorentzVector& p() { return p_; }
    const TLorentzVector& P() { return p(); }

    const std::vector<Constituent const *>& getConstituents() { return constituents_; }
    int getNConstituents() { return constituents_.size(); }
};

#endif
