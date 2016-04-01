#ifndef TOPTAGGERRESULTS_H
#define TOPTAGGERRESULTS_H

#include "TopObject.h"
#include "Constituent.h"

class TTModule;

class TopTaggerResults
{
private:
    //List of input onjects which can be included in a resolved top
    //Will never be modified or changed by modules 
    std::vector<const Constituent> const * constituents_;

    //List of top candidates, will be manipulated by modules
    std::vector<TopObject> topCandidates_;

    //List of final top objects, will be filled out by the modules
    std::vector<TopObject*> tops_;

public:
    
    TopTaggerResults();

    //Setters
    void setConstituents(std::vector<const Constituent> const * constituents) {constituents_ = constituents;}

    //non-const getters (for modules)
    decltype(topCandidates_)& getTopCandidates() { return topCandidates_; }
    decltype(tops_)& getTops() { return tops_; }
    
    //const getters for public consumption
    const std::vector<const Constituent>& getConstituents() const { return *constituents_; }
    const decltype(topCandidates_)& getTopCandidates() const { return topCandidates_; }
    const decltype(tops_)& getTops() const { return tops_; }
};

#endif
