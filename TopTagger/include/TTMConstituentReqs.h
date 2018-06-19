#ifndef TTMCONSTITUENTREQS_H
#define TTMCONSTITUENTREQS_H

#include "TopTagger/TopTagger/include/TTMFilterBase.h"

#include <string>

class Constituent;

namespace cfg
{
    class CfgDocument;
}

class TTMConstituentReqs : public TTMFilterBase
{
protected:
    //matching variable 
    float dRMatch_;

    //mono-jet variables
    float minAK8TopMass_, maxAK8TopMass_, maxTopTau32_, minAK8TopPt_;

    //dijet variables
    float minAK8WMass_, maxAK8WMass_, maxWTau21_, minAK8WPt_, minAK4WPt_;

    //trijet variables
    float dRMaxTrijet_, minAK4ResolvedPt_;

    //Implement the requirements to be tagged as an AK8 W
    bool passAK8WReqs(const Constituent& constituent) const;

    //Implement the requirements for AK4 jets to partner with AK8 W
    bool passAK4WReqs(const Constituent& constituent, const Constituent& constituentAK8) const;

    //Implement the requirements to be tagged as an AK8 top
    bool passAK8TopReqs(const Constituent& constituent) const;

    //Implement the requirements for the AK4 resolved constituents
    bool passAK4ResolvedReqs(const Constituent& constituent) const;

    //Get the parameters needed for te constituent selection functions
    void getParameters(const cfg::CfgDocument*, const std::string&);
};

#endif
