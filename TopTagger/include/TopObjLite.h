#include "TopTagger/TopTagger/include/TopObject.h"

#include "TLorentzVector.h"

class TopObjLite
{
public:
    TLorentzVector p4;
    float discriminator;
    int type;
    unsigned int j1Index, j2Index, j3Index;

    TopObjLite() {}
    TopObjLite(const TopObject& top) : p4(top.p()), discriminator(top.getDiscriminator()), type(top.getType()), j1Index(-1), j2Index(-1), j3Index(-1)
    {
        const auto& constituents = top.getConstituents();
        const int NCONST = constituents.size();
        if(NCONST >= 1) j1Index = constituents[0]->getIndex();
        if(NCONST >= 2) j2Index = constituents[1]->getIndex();
        if(NCONST >= 3) j3Index = constituents[2]->getIndex();
    }
};
