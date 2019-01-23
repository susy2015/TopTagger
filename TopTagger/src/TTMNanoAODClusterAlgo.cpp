#include "TopTagger/TopTagger/interface/TTMNanoAODClusterAlgo.h"

#include "TopTagger/TopTagger/interface/TopTaggerResults.h"
#include "TopTagger/CfgParser/include/Context.hh"
#include "TopTagger/CfgParser/include/CfgDocument.hh"
#include "TopTagger/CfgParser/include/TTException.h"

void TTMNanoAODClusterAlgo::getParameters(const cfg::CfgDocument* cfgDoc, const std::string& localContextName)
{
    //Construct contexts
    cfg::Context commonCxt("Common");
    cfg::Context localCxt(localContextName);
    
    //monojet parameters
    doMonojet_        = cfgDoc->get("doMonojet",        localCxt,  false);
    doMonoW_          = cfgDoc->get("doMonoW",          localCxt,  false);

    //trijet parameters
    doTrijet_           = cfgDoc->get("doTrijet",          localCxt,  false);

    //get vars for TTMConstituentReqs
    TTMConstituentReqs::getParameters(cfgDoc, localContextName);
}

void TTMNanoAODClusterAlgo::run(TopTaggerResults& ttResults)
{
    const std::vector< Constituent>& constituents = ttResults.getConstituents();
    std::vector<TopObject>& topCandidates = ttResults.getTopCandidates();

    for(unsigned int i = 0; i < constituents.size(); ++i)
    {
        //singlet tops
        if(doMonojet_)
        {
            //Only use AK8 tops here 
            if(passDeepAK8TopReqs(constituents[i])) 
            {
                TopObject topCand({&constituents[i]}, TopObject::MERGED_TOP);
                topCand.setDiscriminator(constituents[i].getTopDisc());

                topCandidates.push_back(topCand);
            }
        }

        //single w-boson jet
        if(doMonoW_)
        {
            //Only use AK8 W here 
            if(passDeepAK8WReqs(constituents[i])) 
            {
                TopObject topCand({&constituents[i]}, TopObject::MERGED_W);
                topCand.setDiscriminator(constituents[i].getTopDisc());

                topCandidates.push_back(topCand);
            }
        }

        //Trijet combinations 
        if(doTrijet_)
        {
            //We assume all clustering was done before, we only need to check the type
            if(constituents[i].getType() == RESOLVEDTOPCAND)
            {
                //Fill the resolved top 

                //get top constituent indices 
                const auto& jetRefIndices = constituents[i].getJetRefIndicies();
                if(jetRefIndices.size() == 3)
                {
                    int jetIndex1 = jetRefIndices[0];
                    int jetIndex2 = jetRefIndices[1];
                    int jetIndex3 = jetRefIndices[2];

                    //Unfortunately we can't get a direct index to the AK4 constituents, so we have to search "by hand"
                    std::vector<const Constituent*> resolvedTopConstituents;
                    for(const auto& constituent : constituents)
                    {
                        if(constituent.getType() == AK4JET)
                        {
                            int constIndex = constituent.getIndex();
                            if(jetIndex1 == constIndex || jetIndex2 == constIndex || jetIndex3 == constIndex)
                            {
                                resolvedTopConstituents.push_back(&constituent);
                            }
                        }
                    }

                    TopObject topCand(resolvedTopConstituents, TopObject::RESOLVED_TOP);
                    topCand.setDiscriminator(constituents[i].getTopDisc());

                    topCandidates.push_back(topCand);
                }
                else
                {
                    THROW_TTEXCEPTION("Malformed resolved top constituent, must have exactly 3 jet references!!!");
                }
            }
        }
    }
}

