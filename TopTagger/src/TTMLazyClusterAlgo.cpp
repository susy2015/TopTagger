#include "TopTagger/TopTagger/include/TTMLazyClusterAlgo.h"

#include "TopTagger/TopTagger/include/TopTaggerResults.h"

void TTMLazyClusterAlgo::run(TopTaggerResults& ttResults)
{
    const std::vector< Constituent>& constituents = ttResults.getConstituents();
    std::vector<TopObject>& topCandidates = ttResults.getTopCandidates();

    for(unsigned int i = 0; i < constituents.size(); ++i)
    {
        //singlet tops
        if(constituents[i].p().M() >= 110 && constituents[i].p().M() <= 220)
        {
            TopObject topCand({&constituents[i]});

            topCandidates.push_back(topCand);
        }

        //singlet w-bosons
        if(constituents[i].p().M() >= 70 && constituents[i].p().M() <= 110)
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

        for(unsigned int j = 0; j < i; ++j)
        {
            //Trijet combinations 
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
