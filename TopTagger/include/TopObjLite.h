#include "DataFormats/Candidate/interface/LeafCandidate.h"

#include "TopTagger/TopTagger/include/TopObject.h"

#include "TLorentzVector.h"

/**
 * TopObjLite is a version of top object which is derived from cmssw reco::Candidate (through reco::LeafCandidate) to allow the critical information to be saved into nanoAOD data format
 */
class TopObjLite : public reco::LeafCandidate
{
private:
    float discriminator_;
    int type_;
    unsigned int j1Index_, j2Index_, j3Index_;

public:
    TopObjLite() {}
    TopObjLite(const TopObject& top) : reco::LeafCandidate(), discriminator_(top.getDiscriminator()), type_(top.getType()), j1Index_(-1), j2Index_(-1), j3Index_(-1)
    {
        //Initialize the LeafCandidate's p4 with the top p4
        construct(0, top.p().Pt(), top.p().Eta(), top.p().Phi(), top.p().M(), reco::LeafCandidate::Point(0, 0, 0), 0, 0);

        //Get the constituents of the top and set the index pointing to each jet
        const auto& constituents = top.getConstituents();
        const int NCONST = constituents.size();
        if(NCONST >= 1) j1Index_ = constituents[0]->getIndex();
        if(NCONST >= 2) j2Index_ = constituents[1]->getIndex();
        if(NCONST >= 3) j3Index_ = constituents[2]->getIndex();
    }

    //getters for the internal variables
    float getDiscriminator() const { return discriminator_; }
    int getType() const { return type_; }
    unsigned int getJ1Idx() const { return j1Index_; }
    unsigned int getJ2Idx() const { return j2Index_; }
    unsigned int getJ3Idx() const { return j3Index_; }
};
