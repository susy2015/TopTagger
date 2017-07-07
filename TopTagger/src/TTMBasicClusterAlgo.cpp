#include "TopTagger/TopTagger/include/TTMBasicClusterAlgo.h"

#include "TopTagger/TopTagger/include/TopTaggerResults.h"
#include "TopTagger/CfgParser/include/Context.hh"
#include "TopTagger/CfgParser/include/CfgDocument.hh"

#include <iostream>

void TTMBasicClusterAlgo::getParameters(const cfg::CfgDocument* cfgDoc, const std::string& localContextName)
{
    //Construct contexts
    cfg::Context commonCxt("Common");
    cfg::Context localCxt(localContextName);
    
    //monojet parameters
    doMonojet_        = cfgDoc->get("doMonojet",        localCxt,  false);

    //dijet parameters
    doDijet_          = cfgDoc->get("doDijet",          localCxt,  false);
    dRMaxDiJet_       = cfgDoc->get("dRMaxDijet",       localCxt, -999.9);

    //trijet parameters
    minTopCandMass_   = cfgDoc->get("minTopCandMass",   localCxt, -999.9);
    maxTopCandMass_   = cfgDoc->get("maxTopCandMass",   localCxt, -999.9);
    doTrijet_         = cfgDoc->get("doTrijet",         localCxt,  false);
    dRMaxTrijet_      = cfgDoc->get("dRMaxTrijet",      localCxt, -999.9);
    nbSeed_           = cfgDoc->get("nbSeed",           localCxt, -1);

    //get vars for TTMConstituentReqs
    TTMConstituentReqs::getParameters(cfgDoc, localContextName);
}

void TTMBasicClusterAlgo::run(TopTaggerResults& ttResults)
{
    const std::vector< Constituent>& constituents = ttResults.getConstituents();
    std::vector<TopObject>& topCandidates = ttResults.getTopCandidates();

    std::vector<const Constituent*> constituentsCSVSort;
    if(nbSeed_ > 0)
    {
        for(const auto& constituent : constituents)
        {
            constituentsCSVSort.push_back(&constituent);
        }
        std::sort(constituentsCSVSort.begin(), constituentsCSVSort.end(), [](const Constituent* c1, const Constituent* c2) { return c1->getBTagDisc() > c2->getBTagDisc(); } );
    }

    for(unsigned int i = 0; i < constituents.size(); ++i)
    {
        //singlet tops
        if(doMonojet_)
        {
            //Only use AK8 tops here 
            if(passAK8TopReqs(constituents[i]))
            {
                TopObject topCand({&constituents[i]});

                topCandidates.push_back(topCand);
            }
        }

        //singlet w-bosons + jet
        if(doDijet_)
        {
            if(passAK8WReqs(constituents[i]))
            {
                //dijet combinations
                for(unsigned int j = 0; j < constituents.size(); ++j)
                {
                    //Ensure we never use the same jet twice
                    //Only pair the AK8 W with an AK4 jet
                    //the AK8 jet is passed to ensure the AK4 jet does not overlap with it
                    if(i == j || !passAK4WReqs(constituents[j], constituents[i])) continue;

                    TopObject topCand({&constituents[i], &constituents[j]});

                    //mass window on the top candidate mass
                    double m123 = topCand.p().M();
                    bool passMassWindow = (minTopCandMass_ < m123) && (m123 < maxTopCandMass_);

                    if(topCand.getDRmax() < dRMaxDiJet_ && passMassWindow)
                    {
                        topCandidates.push_back(topCand);
                    }
                }
            }
        }

        //Trijet combinations 
        if(doTrijet_)
        {
            if(passAK4ResolvedReqs(constituents[i]))
            {
                for(unsigned int j = 0; j < i; ++j)
                {
                    if(passAK4ResolvedReqs(constituents[j]))
                    {
                        if(nbSeed_ <= 0)
                        {
                            for(unsigned int k = 0; k < j; ++k)
                            {
                                if(passAK4ResolvedReqs(constituents[k]))
                                {
                                    fillTriplet(&constituents[k], &constituents[j], &constituents[i], topCandidates);
                                }
                            }
                        }
                        else
                        {
                            for(int k = 0; k < std::min(nbSeed_, static_cast<int>(constituentsCSVSort.size())); ++k)
                            {
                                if(constituentsCSVSort[k] == &constituents[i] || constituentsCSVSort[k] == &constituents[j]) continue;

                                if(passAK4ResolvedReqs(*constituentsCSVSort[k]))
                                {
                                    fillTriplet(constituentsCSVSort[k], &constituents[j], &constituents[i], topCandidates);
                                }
                                
                            }
                        }
                    }
                }
            }
        }
    }
}

void TTMBasicClusterAlgo::fillTriplet(const Constituent* const c1, const Constituent* const c2, const Constituent* const c3, std::vector<TopObject>& topCandidates)
{
    TopObject topCand({c1, c2, c3});

    //mass window on the top candidate mass
    double m123 = topCand.p().M();
    bool passMassWindow = (minTopCandMass_ < m123) && (m123 < maxTopCandMass_);

    if(topCand.getDRmax() < dRMaxTrijet_ && passMassWindow)
    {
        topCandidates.push_back(topCand);
    }
}
