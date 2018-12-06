#ifndef TTMCONSTITUENTREQS_H
#define TTMCONSTITUENTREQS_H

#include "TopTagger/TopTagger/interface/TTMFilterBase.h"

#include <string>

class Constituent;

namespace cfg
{
    class CfgDocument;
}

/**
 *This class implemens common selection requirements for constituents for use in other modules, but is not itself a TT Module
 *
 *@param dRMatch (float) <b> common context </b> The dR requirement used to select whether an AK4 jet matches a AK8 subjet
 *@param minAK8TopMass (float) Minimum AK8 mass to be considered an AK8 top candidate.
 *@param maxAK8TopMass (float) Maximum AK8 mass to be considered an AK8 top candidate.
 *@param maxTopTau32 (float) Maximum of the Nsubjettiness ratio tau3/tau2 to be considered an AK8 top candidate.
 *@param deepAK8TopDisc (float) Minimum deepAK8 discriminator threshold to be considered an AK8 top candidate.
 *@param minAK8TopPt (float) Minimum top pt to be considered as an AK8 top candidate.
 *@param minAK8WMass (float) Minimum AK8 mass to be considered an AK8 W candidate.
 *@param maxAK8WMass (float) Maximum AK8 mass to be considered an AK8 W candidate.
 *@param maxWTau21 (float) Maximum of the Nsubjettiness ratio tau2/tau1 to be considered an AK8 W candidate.
 *@param deepAK8WDisc (float) Minimum deepAK8 discriminator threshold to be considered an AK8 W candidate.
 *@param minAK8WPt (float) Minimum top AK8 jet pt to be considered as an AK8 + AK4 top candidate.
 *@param minAK4WPt (float) Minimum top AK4 jet pt to be considered as an AK8 + AK4 top candidate.
 *
 */ 
class TTMConstituentReqs : public TTMFilterBase
{
protected:
    //matching variable 
    double dRMatch_, dRMatchAK8_;

    //mono-jet variables
    double minAK8TopMass_, maxAK8TopMass_, maxTopTau32_, minAK8TopPt_, deepAK8WDisc_, deepAK8TopDisc_;

    //dijet variables
    double minAK8WMass_, maxAK8WMass_, maxWTau21_, minAK8WPt_, minAK4WPt_;

    //trijet variables

    ///Implement the requirements to be tagged as an AK8 W
    bool passAK8WReqs(const Constituent& constituent) const;

    ///Implement the requirements for AK4 jets to partner with AK8 W
    bool passAK4WReqs(const Constituent& constituent, const Constituent& constituentAK8) const;

    ///Implement the requirements to be tagged as an AK8 top
    bool passAK8TopReqs(const Constituent& constituent) const;

    ///Implement the requirements for the AK4 resolved constituents
    bool passAK4ResolvedReqs(const Constituent& constituent, const double minPt) const;

    ///Implement requirements on AK8 W tagged with deepAK8
    bool passDeepAK8WReqs(const Constituent& constituent) const;

    ///Implement requirements on AK8 tops tagged with deepAK8
    bool passDeepAK8TopReqs(const Constituent& constituent) const;

    ///Get the parameters needed for the constituent selection functions
    void getParameters(const cfg::CfgDocument*, const std::string&);
};

#endif
