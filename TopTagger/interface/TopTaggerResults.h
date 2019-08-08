#ifndef TOPTAGGERRESULTS_H
#define TOPTAGGERRESULTS_H

#include "TopTagger/TopTagger/interface/TopObject.h"
#include "TopTagger/TopTagger/interface/Constituent.h"

#include <vector>
#include <map>
#include <set>
#include <memory>

/**
 *This is a holder class which holds the final collection of top objects, along with the constituents used to construct them and any intermediate information used by modules.  This serves both as the user interface for the results and a container to pass between modules.
 */
class TopTaggerResults
{
private:
    ///List of input objects which can be included in a resolved top
    ///Will never be modified or changed by modules 
    std::shared_ptr<const std::vector<Constituent>> constituents_;

    ///Number of ak4 jets in the event, necessary for systematics
    mutable int nAK4Jets_;

    ///List of jets used to construct final tops, needed for Rsys
    std::set<Constituent const *> usedConstituents_;

    ///List of top candidates, will be manipulated by modules
    std::vector<TopObject> topCandidates_;

    ///List of final top objects, will be filled out by the modules
    std::vector<TopObject*> tops_;

    ///Final tops sorted by type
    std::map<TopObject::Type, std::vector<TopObject*>> topsByType_;

    ///The remaining system container
    TopObject rsys_;

public:
    
    /**
     *This constructor makes a copy of constituents to ensure it remains in scope while 
     *the top tagger results are in scope.  This copy is totally internal and is
     *managed by the shared pointer.  
     */
    TopTaggerResults(const std::vector<Constituent>& constituents) : constituents_(new std::vector<Constituent>(constituents)), nAK4Jets_(-1) {}

    TopTaggerResults(std::vector<Constituent>&& constituents) : constituents_(new std::vector<Constituent>(std::move(constituents))), nAK4Jets_(-1) {}

    ~TopTaggerResults() {}

    //Setters
    /** Set/reset the internal copy of the constituents vector */
    void setConstituents(const std::vector<Constituent>& constituents)
    {
        //Again a copy is made to ensure this vector remains in scope
        constituents_.reset(new std::vector<Constituent>(constituents));
        nAK4Jets_ = -1;
    }

    /** Set/reset the internal copy of the constituents vector */
    void setConstituents(std::vector<Constituent>&& constituents)
    {
        //Again a copy is made to ensure this vector remains in scope
        constituents_.reset(new std::vector<Constituent>(constituents));
        nAK4Jets_ = -1;
    }

    //non-const getters (for modules)
    decltype(topCandidates_)& getTopCandidates() { return topCandidates_; }
    decltype(usedConstituents_)& getUsedConstituents() { return usedConstituents_; }
    decltype(tops_)& getTops() { return tops_; }
    decltype(rsys_)& getRsys() { return rsys_; }
    decltype(topsByType_)& getTopsByType() { return topsByType_; }
    
    //const getters for public consumption
    /** Get the internal vector of constituents */
    const std::vector<Constituent>& getConstituents() const { return *constituents_; }
    /** Get the set of constituens which have been flagged as used in final reconstructed TopObjects */
    const decltype(usedConstituents_)& getUsedConstituents() const { return usedConstituents_; }
    /** Get the vector of top candidates */
    const decltype(topCandidates_)& getTopCandidates() const { return topCandidates_; }
    /** Get the vector of final reconstructed tops */
    const decltype(tops_)& getTops() const { return tops_; }
    /** Get a map of final top objects split by type */
    const decltype(topsByType_)& getTopsByType() const { return topsByType_; }
    /** Get the remaining system used for MT2 calculations in the case when there is only one reconstructed top */
    const decltype(rsys_)& getRsys() const { return rsys_; }

    ///Internal function to calculate the number of ak4Jets in the event 
    int countAK4Jets(double pt = 30, double eta = 2.4) const 
    {
        //if this number is positive or zero it has already been calculated, else recalculate
        if(nAK4Jets_ < 0) 
        {
            nAK4Jets_ = 0;
            for(const auto& constituent : getConstituents())
            {
                if(constituent.getType() == Constituent::AK4JET &&
                   constituent.p().Pt() >= pt &&
                   fabs(constituent.p().Eta()) <= eta)
                {
                    ++nAK4Jets_;
                }
                    
            }
        }

        return nAK4Jets_;
    }
};

#endif
