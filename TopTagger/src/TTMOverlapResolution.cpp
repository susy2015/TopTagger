#include "TopTagger/TopTagger/include/TTMOverlapResolution.h"

#include <set>
#include <algorithm>
#include <vector>
#include <cmath>

#include "TopTagger/TopTagger/include/TopTaggerResults.h"

void TTMOverlapResolution::run(TopTaggerResults& ttResults)
{
    std::vector<TopObject*>& tops = ttResults.getTops();

    //Sort the top vector by mass
    //NEED TO MAKE CONST HOLDER INCLUDE
    const double mt = 173.4;
    std::sort(tops.begin(), tops.end(), [mt](TopObject* t1, TopObject* t2){ return fabs(t1->p().M() - mt) < fabs(t2->p().M() - mt);});

    std::set<Constituent const *> usedJets;

    for(auto iTop = tops.begin(); iTop != tops.end();)
    {
        const std::vector<Constituent const *>& jets = (*iTop)->getConstituents();

        bool breakLoop = false;

        for(const auto& jet : jets)
        {
            if(usedJets.count(jet))
            {
                //This is inefficient and dumb
                iTop = tops.erase(iTop);
                breakLoop = true;
                break;
            }
        }

        if(breakLoop) continue;

        for(const auto& jet : jets)
        {
            usedJets.insert(jet);
        }

        ++iTop;
    }
}
