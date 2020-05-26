#include "TopTagger/TopTagger/interface/TTMBasicClusterAlgo.h"

#include "TopTagger/TopTagger/interface/TopTaggerResults.h"
#include "TopTagger/CfgParser/interface/Context.hh"
#include "TopTagger/CfgParser/interface/CfgDocument.hh"

#include <iostream>

void TTMBasicClusterAlgo::getParameters(const cfg::CfgDocument* cfgDoc, const std::string& localContextName)
{
    //Construct contexts
    cfg::Context commonCxt("Common");
    cfg::Context localCxt(localContextName);
    
    //monojet parameters
    doMonojet_        = cfgDoc->get("doMonojet",        localCxt,  false);
    doMonoW_          = cfgDoc->get("doMonoW",          localCxt,  false);
    useDeepAK8_       = cfgDoc->get("useDeepAK8",       localCxt,  false);

    //dijet parameters
    doDijet_          = cfgDoc->get("doDijet",          localCxt,  false);
    dRMaxDiJet_       = cfgDoc->get("dRMaxDijet",       localCxt, -999.9);

    //trijet parameters
    minTopCandMass_     = cfgDoc->get("minTopCandMass",    localCxt, -999.9);
    maxTopCandMass_     = cfgDoc->get("maxTopCandMass",    localCxt, -999.9);
    doTrijet_           = cfgDoc->get("doTrijet",          localCxt,  false);
    dRMaxTrijet_        = cfgDoc->get("dRMaxTrijet",       localCxt, -999.9);
    nbSeed_             = cfgDoc->get("nbSeed",            localCxt, -1);
    minTrijetAK4JetPt_  = cfgDoc->get("minTrijetAK4JetPt", localCxt, -999.0);
    midTrijetAK4JetPt_  = cfgDoc->get("midTrijetAK4JetPt", localCxt, -999.0);
    maxTrijetAK4JetPt_  = cfgDoc->get("maxTrijetAK4JetPt", localCxt, -999.0);
    maxTrijetPt_        = cfgDoc->get("maxTrijetPt",       localCxt, -999.0);

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
            if(passAK4ResolvedReqs(constituent, minTrijetAK4JetPt_)) constituentsCSVSort.push_back(&constituent);
        }
        std::sort(constituentsCSVSort.begin(), constituentsCSVSort.end(), [](const Constituent* c1, const Constituent* c2) { return c1->getBTagDisc() > c2->getBTagDisc(); } );
    }

    for(unsigned int i = 0; i < constituents.size(); ++i)
    {
        //singlet tops
        if(doMonojet_)
        {
            //Only use AK8 tops here 
            if(useDeepAK8_ && passDeepAK8TopReqs(constituents[i])) 
            {
                TopObject topCand({&constituents[i]}, TopObject::MERGED_TOP);
                topCand.setDiscriminator(constituents[i].getTopDisc());

                topCandidates.push_back(topCand);
            }
            else if (!useDeepAK8_ && passAK8TopReqs(constituents[i]))
            {
                TopObject topCand({&constituents[i]}, TopObject::MERGED_TOP);

                topCandidates.push_back(topCand);
            }
        }

        //single w-boson jet
        if(doMonoW_)
        {
            //Only use AK8 tops here 
            if(useDeepAK8_ && passDeepAK8WReqs(constituents[i])) 
            {
                TopObject topCand({&constituents[i]}, TopObject::MERGED_W);
                topCand.setDiscriminator(constituents[i].getTopDisc());

                topCandidates.push_back(topCand);
            }
            else if (!useDeepAK8_ && passAK8WReqs(constituents[i]))
            {
                TopObject topCand({&constituents[i]}, TopObject::MERGED_W);

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

                    TopObject topCand({&constituents[i], &constituents[j]}, TopObject::SEMIMERGEDWB_TOP);

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
            if(passAK4ResolvedReqs(constituents[i], minTrijetAK4JetPt_))
            {
                for(unsigned int j = 0; j < i; ++j)
                {
                    if(passAK4ResolvedReqs(constituents[j], midTrijetAK4JetPt_))
                    {
                        for(unsigned int k = 0; k < j; ++k)
                        {
                            if(passAK4ResolvedReqs(constituents[k], maxTrijetAK4JetPt_))
                            {
                                if(nbSeed_ > 0)
                                {
                                    //Require that each combination contain at least one of the nbSeed_ highest csv jets 
                                    bool reject = true;
                                    for(int l = 0; l < std::min(nbSeed_, static_cast<int>(constituentsCSVSort.size())); ++l)
                                    {
                                        if(&constituents[k] == constituentsCSVSort[l] || &constituents[j] == constituentsCSVSort[l] || &constituents[i] == constituentsCSVSort[l])
                                        {
                                            reject = false;
                                            break;
                                        }
                                    }
                                    if(reject) continue;
                                }
                                fillTriplet(&constituents[k], &constituents[j], &constituents[i], topCandidates);
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
    TopObject topCand({c1, c2, c3}, TopObject::RESOLVED_TOP);

    //mass window on the top candidate mass
    double m123 = topCand.p().M();
    bool passMassWindow = (minTopCandMass_ < m123) && (m123 < maxTopCandMass_);
    bool passMaxPt = (maxTrijetPt_ < 0.0) || topCand.p().Pt() < maxTrijetPt_;

    if(topCand.getDRmax() < dRMaxTrijet_ && passMassWindow && passMaxPt)
    {
        topCandidates.push_back(topCand);
    }
}
