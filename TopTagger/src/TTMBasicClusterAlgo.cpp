#include "TopTagger/TopTagger/include/TTMBasicClusterAlgo.h"

#include "TopTagger/TopTagger/include/TopTaggerResults.h"
#include "TopTagger/CfgParser/include/Context.hh"
#include "TopTagger/CfgParser/include/CfgDocument.hh"

void TTMBasicClusterAlgo::getParameters(const cfg::CfgDocument* cfgDoc)
{
    //Construct contexts
    cfg::Context commonCxt("Common");
    cfg::Context localCxt("TTMBasicClusterAlgo");
    
    lowWMassCut_    = cfgDoc->get("lowWJetMassCut",  commonCxt, -999.9);
    highWMassCut_   = cfgDoc->get("highWJetMassCut", commonCxt, -999.9);
    lowtMassCut_    = cfgDoc->get("lowtJetMassCut",  commonCxt, -999.9);
    hightMassCut_   = cfgDoc->get("hightJetMassCut", commonCxt, -999.9);
    minTopCandMass_ = cfgDoc->get("minTopCandMass",  commonCxt, -999.9);
    maxTopCandMass_ = cfgDoc->get("maxTopCandMass",  commonCxt, -999.9);
    dRMax_          = cfgDoc->get("dRMax",           commonCxt, -999.9);

    doMonojet_      = cfgDoc->get("doMonojet",      localCxt,  false);
    doDijet_        = cfgDoc->get("doDijet",        localCxt,  false);
    doTrijet_       = cfgDoc->get("doTrijet",       localCxt,  false);
}

void TTMBasicClusterAlgo::run(TopTaggerResults& ttResults)
{
    const std::vector< Constituent>& constituents = ttResults.getConstituents();
    std::vector<TopObject>& topCandidates = ttResults.getTopCandidates();

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

        //singlet w-bosons
        if(doDijet_)
        {
            if(passAK8WReqs(constituents[i]))
            {
                //dijet combinations
                for(unsigned int j = 0; j < constituents.size(); ++j)
                {
                    //Ensure we never use the same jet twice
                    //Only pair the AK8 W with an AK4 jet
                    if(i == j && constituents[j].getType() == AK4JET) continue;

                    TopObject topCand({&constituents[i], &constituents[j]});
                
                    if(topCand.getDRmax() < dRMax_)
                    {
                        topCandidates.push_back(topCand);
                    }
                }
            }
        }

        //Trijet combinations 
        if(doTrijet_)
        {
            if(constituents[i].getType() == AK4JET)
            {
                for(unsigned int j = 0; j < i; ++j)
                {
                    if(constituents[j].getType() == AK4JET)
                    {                
                        for(unsigned int k = 0; k < j; ++k)
                        {
                            if(constituents[k].getType() == AK4JET)
                            {
                                TopObject topCand({&constituents[k], &constituents[j], &constituents[i]});

                                //mass window on the top candidate mass
                                double m123 = topCand.p().M();
                                bool passMassWindow = (minTopCandMass_ < m123) && (m123 < maxTopCandMass_);

                                if(topCand.getDRmax() < dRMax_ && passMassWindow)
                                {
                                    topCandidates.push_back(topCand);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

bool TTMBasicClusterAlgo::passAK8TopReqs(const Constituent& constituent) const
{
    return constituent.getType() == AK8JET;
}

bool TTMBasicClusterAlgo::passAK8WReqs(const Constituent& constituent) const
{
    return constituent.getType() == AK8JET;
}
