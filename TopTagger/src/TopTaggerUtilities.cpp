#include "TopTagger/TopTagger/include/TopTaggerUtilities.h"

#include "TopTagger/TopTagger/include/Constituent.h"

#include "TopTagger/CfgParser/include/TTException.h"

namespace ttUtility
{
    std::vector<Constituent> packageCandidates(const std::vector<TLorentzVector>& jetsLVec, const std::vector<double>& btagFactors)
    {
        //vector holding constituents to be created
        std::vector<Constituent> constituents;

        //Safety check that jet and b-tag vectors are the same length
        if(jetsLVec.size() != btagFactors.size())
        {
            //throw "ttUtility::packageCandidates(...) : Unequal vector size!!!!!!!";
            THROW_TTEXCEPTION("Unequal vector size!!!!!!!");
        }
        
        //Construct constituents in place in the vector
        for(unsigned int iJet = 0; iJet < jetsLVec.size(); ++iJet)
        {
            constituents.emplace_back(jetsLVec[iJet], btagFactors[iJet]);
        }

        //This will proceed as a move, not a deep copy
        return constituents;
    }
}
