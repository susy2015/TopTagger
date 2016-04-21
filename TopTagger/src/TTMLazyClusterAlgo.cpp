#include "TopTagger/TopTagger/include/TTMLazyClusterAlgo.h"

#include "TopTagger/TopTagger/include/TopTaggerResults.h"
#include "TopTagger/CfgParser/include/Context.hh"
#include "TopTagger/CfgParser/include/CfgDocument.hh"

void TTMLazyClusterAlgo::getParameters(const cfg::CfgDocument* cfgDoc)
{
    //Construct contexts
    cfg::Context commonCxt("Common");
    cfg::Context localCxt("TTMLazyClusterAlgo");
    
    lowWMassCut_  = cfgDoc->get("lowWJetMassCut",  commonCxt, -999.9);
    highWMassCut_ = cfgDoc->get("highWJetMassCut", commonCxt, -999.9);
    lowtMassCut_  = cfgDoc->get("lowtJetMassCut",  commonCxt, -999.9);
    hightMassCut_ = cfgDoc->get("hightJetMassCut", commonCxt, -999.9);

    doMonojet_ = cfgDoc->get("doMonojet", localCxt, false);
    doDijet_   = cfgDoc->get("doDijet",   localCxt, false);
    doTrijet_  = cfgDoc->get("doTrijet",  localCxt, false);
}

void TTMLazyClusterAlgo::run(TopTaggerResults& ttResults)
{
    const std::vector< Constituent>& constituents = ttResults.getConstituents();
    std::vector<TopObject>& topCandidates = ttResults.getTopCandidates();

    for(unsigned int i = 0; i < constituents.size(); ++i)
    {
        //singlet tops
        if(doMonojet_)
        {
            if(constituents[i].p().M() >= lowtMassCut_ && constituents[i].p().M() <= hightMassCut_)
            {
                TopObject topCand({&constituents[i]});

                topCandidates.push_back(topCand);
            }
        }

        //singlet w-bosons
        if(doDijet_)
        {
            if(constituents[i].p().M() >= lowWMassCut_ && constituents[i].p().M() <= highWMassCut_)
            {
                //dijet combinations
                for(unsigned int j = 0; j < constituents.size(); ++j)
                {
                    if(i == j) continue;

                    TopObject topCand({&constituents[i], &constituents[j]});
                
                    if(topCand.getDRmax() < 1.5)
                    {
                        topCandidates.push_back(topCand);
                    }
                }
            }
        }

        //Trijet combinations 
        if(doTrijet_)
        {
            for(unsigned int j = 0; j < i; ++j)
            {
                for(unsigned int k = 0; k < j; ++k)
                {
                    TopObject topCand({&constituents[k], &constituents[j], &constituents[i]});

                    if(topCand.getDRmax() < 1.5)
                    {
                        topCandidates.push_back(topCand);
                    }
                }
            }
        }
    }
}
