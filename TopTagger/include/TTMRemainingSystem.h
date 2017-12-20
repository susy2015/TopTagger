#ifndef TTMREMAININGSYSTEM_H
#define TTMREMAININGSYSTEM_H

#include "TopTagger/TopTagger/include/TTModule.h"
#include "TopTagger/TopTagger/include/TTMConstituentReqs.h"

/**
 *This module calclates the remaining system.  The purpose of the remaining system is to partially reconstruct a top quark to give a second input into the MT2 calculation for the 1-top catagory.  The reconstruction algorithm used here starts by looking for a b-jet not included in a top.  If no b jet is found then the highest pt jet not in a top is selected.  The algorithm can then combine nearby jets with the first selected jet if certain requirements are satisified.  If no second jet satisifies these requirments, the fist chosen jet is used by itself.  This algorithm only finds one such object using this method.  If multiple jet pairings satisify the requirements, the pairing with the smallest dR is chosen.  
 *
 *@param csvThreshold (float) Threshold on b-tag discriminator to be considered a b-jet
 *@param lowRsysMass (float) Lower limit on the mass of the remaining system
 *@param highRsysMass (float) Upper limit on the mass of the remaining system
 *@param dRMaxRsys (float) Maximum allowed seperation in dR between the jets of the remaining system and their centroid
 *@param useSecondJet (bool) Sets whether the algorithm will look for a second jet to combine with the seed jet
 *@param allowW (bool) Sets whether a AK8W candidate can be a seed for the remaining system
*/
class TTMRemainingSystem : public TTModule, public TTMConstituentReqs
{
private:
    double CSVThresh_, lowRsysMass_, highRsysMass_, dRMax_;
    bool useSecondJet_, allowW_;

public:
    void getParameters(const cfg::CfgDocument*, const std::string&);
    void run(TopTaggerResults&);
};
REGISTER_TTMODULE(TTMRemainingSystem);

#endif
