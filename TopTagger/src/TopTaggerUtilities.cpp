#include "TopTagger/TopTagger/include/TopTaggerUtilities.h"

#include "TopTagger/TopTagger/include/Constituent.h"
#include "TopTagger/TopTagger/include/TopTaggerResults.h"

#include "TopTagger/CfgParser/include/TTException.h"

namespace ttUtility
{
  std::vector<Constituent> packageConstituents(const std::vector<TLorentzVector>& jetsLVec, const std::vector<double>& btagFactors, const std::vector<double>& qgLikelihood, const std::vector<double>& jetChrg)
    {
        //vector holding constituents to be created
        std::vector<Constituent> constituents;

        //Safety check that jet and b-tag vectors are the same length
        if(jetsLVec.size() != btagFactors.size() || jetsLVec.size() != qgLikelihood.size() || jetsLVec.size() != jetChrg.size())
        {
            THROW_TTEXCEPTION("Unequal vector size!!!!!!!\n" + std::to_string(jetsLVec.size()) + "\t" + std::to_string(qgLikelihood.size()));
        }
        
        //Construct constituents in place in the vector
        for(unsigned int iJet = 0; iJet < jetsLVec.size(); ++iJet)
        {
	  constituents.emplace_back(jetsLVec[iJet], btagFactors[iJet], qgLikelihood[iJet], jetChrg[iJet]);
        }

        //This will proceed as a move, not a deep copy
        return constituents;
    }

    double calculateMT2(const TopTaggerResults& ttr)
    {
        return 0.0;
    }
}
