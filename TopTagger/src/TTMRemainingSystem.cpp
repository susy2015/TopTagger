#include "TopTagger/TopTagger/interface/TTMRemainingSystem.h"

#include "TopTagger/TopTagger/interface/TopTaggerResults.h"
#include "TopTagger/CfgParser/interface/Context.hh"
#include "TopTagger/CfgParser/interface/CfgDocument.hh"

void TTMRemainingSystem::getParameters(const cfg::CfgDocument* cfgDoc, const std::string& localContextName)
{
    //Construct contexts
    cfg::Context commonCxt("Common");
    cfg::Context localCxt(localContextName);

    CSVThresh_    = cfgDoc->get("csvThreshold", localCxt, -999.9);
    lowRsysMass_  = cfgDoc->get("lowRsysMass",  localCxt, -999.9);
    highRsysMass_ = cfgDoc->get("highRsysMass", localCxt, -999.9);
    dRMax_        = cfgDoc->get("dRMaxRsys",    localCxt, -999.9);

    useSecondJet_= cfgDoc->get("useSecondJet",  localCxt, true);
    allowW_      = cfgDoc->get("allowW",        localCxt, true);

    //get vars for TTMConstituentReqs
    TTMConstituentReqs::getParameters(cfgDoc, localContextName);
}

void TTMRemainingSystem::run(TopTaggerResults& ttResults)
{
    //List of constiturnt jets
    const std::vector<Constituent>& consituents = ttResults.getConstituents();

    //set containing all used jets
    const std::set<Constituent const *>& usedJets = ttResults.getUsedConstituents();

    TopObject& rsys = ttResults.getRsys();

    //First select the seed jet for the remaining system
    //The seed is ideally a b-jet, if no b-jet is found
    //the algorithm looks for a lone W-jet. Otherwise
    //the highest pt jet not included in a top will be chosen.
    //If multiple b/w-jets exist, that with the highest pt
    //is chosen

    Constituent const * seed = nullptr;

    for(auto& jet : consituents)
    {
        //check if jet is used in a top
        if(usedJets.count(&jet)) continue;

        //Take first jet not in a top (the constituents are pt ordered)
        if(seed == nullptr && jet.getType() == Constituent::AK4JET)
        {
            seed = &jet;
        }

        //switch to a W-jet if one is found
        if(allowW_ && seed->getType() != Constituent::AK8JET && passAK8WReqs(jet))
        {
            seed = &jet;
        }

        //or better yet the first (highest pt) b-tagged jet if one exists
        if(jet.getBTagDisc() > CSVThresh_)
        {
            seed = &jet;
            break;
        }
    }

    //if no seed is found, abort calculation
    if(seed == nullptr) return;

    //pointer to hold second jet if found
    Constituent const * secondJet = nullptr;

    if(useSecondJet_)
    {
        //Look for a companion jet to form the remaining system with the seed
        //The closest jet to the seed is chosen provided the seed + second jet mass
        //is in the specified range.  If the seed is a b jet, a W is always prefered 
        //over an AK4 jet.
        double mindR = 999.9;
        bool ak8wFound = false;
        for(auto& jet : consituents)
        {
            //check if jet is used in a top or is the seed
            if(usedJets.count(&jet) || &jet == seed) continue;

            double dR = seed->p().DeltaR(jet.p());
            double mdijet = (seed->p() + jet.p()).M();

            //select second jet based upon dR and dijet mass
            if((dRMax_  < 0 || dR < dRMax_)  //disable dR entirely if dRMax is less than 0
               && lowRsysMass_ < mdijet && mdijet < highRsysMass_)  //Rsys must satisify a mass window cut
            {
                //if seed is AK4 jet, prefer AK8W over another AK4 jet
                //else if seed is AK8, prefer AK4 matching jet
                if(allowW_ && seed->getType() == Constituent::AK4JET && passAK8WReqs(jet))
                {
                    if(!ak8wFound || dR < mindR)
                    {
                        mindR = dR;
                        secondJet = &jet;
                    }
                    ak8wFound = true;
                }

                //if no AK8 jet is found use AK4 jet
                if(!ak8wFound && jet.getType() == Constituent::AK4JET && dR < mindR)
                {
                    mindR = dR;
                    secondJet = &jet;
                }
            }
        }
    }

    //Form the final remaining system based upon seed and secondJet
    if(seed)      rsys.addConstituent(seed);
    if(secondJet) rsys.addConstituent(secondJet);
}
