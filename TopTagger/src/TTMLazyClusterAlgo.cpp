#include "TopTagger/TopTagger/include/TTMLazyClusterAlgo.h"

#include "TopTagger/TopTagger/include/TopTaggerResults.h"

void TTMLazyClusterAlgo::run(TopTaggerResults& ttResults)
{
    const std::vector< Constituent>& constituents = ttResults.getConstituents();
    std::vector<TopObject>& topCandidates = ttResults.getTopCandidates();

    for(unsigned int i = 0; i < constituents.size(); ++i)
    {
        for(unsigned int j = 0; j < i; ++j)
        {
            for(unsigned int k = 0; k < j; ++k)
            {
                TopObject topCand({&constituents[i], &constituents[j], &constituents[k]});

                if(topCand.getDRmax() < 1.5)
                {
                    topCandidates.push_back(topCand);
                }
            }
        }
    }
}
