#include "TopTagger/TopTagger/include/TopTaggerUtilities.h"

#include "TopTagger/TopTagger/include/Constituent.h"
#include "TopTagger/TopTagger/include/TopTaggerResults.h"

#include "TopTagger/CfgParser/include/TTException.h"

#include "Math/VectorUtil.h"

#include <map>

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


    std::map<std::string, double> createMVAInputs(const TopObject& topCand)
    {
        std::map<std::string, double> varMap;

        //Get top candidate variables
        varMap["cand_pt"]    = topCand.p().Pt();
        varMap["cand_eta"]   = topCand.p().Eta();
        varMap["cand_phi"]   = topCand.p().Phi();
        varMap["cand_m"]     = topCand.p().M();
        varMap["cand_dRMax"] = topCand.getDRmax();

        //Get Constituents
        const std::vector<Constituent const *>& top_constituents = topCand.getConstituents();
            
        //Get constituent variables before deboost
        for(unsigned int i = 0; i < top_constituents.size(); ++i)
        {
            varMap["j" + std::to_string(i + 1) + "_phi_lab"] = top_constituents[i]->p().Phi();
            varMap["j" + std::to_string(i + 1) + "_eta_lab"] = top_constituents[i]->p().Eta();
            varMap["j" + std::to_string(i + 1) + "_pt_lab"]  = top_constituents[i]->p().Pt();
        }

        std::vector<Constituent> RF_constituents;

        for(const auto& constitutent : top_constituents)
        {
            TLorentzVector p4(constitutent->p());
            p4.Boost(-topCand.p().BoostVector());
            RF_constituents.emplace_back(p4, constitutent->getBTagDisc(), constitutent->getQGLikelihood(), constitutent->getJetCharge());
        }
            
        //re-sort constituents by p after deboosting
        std::sort(RF_constituents.begin(), RF_constituents.end(), [](const Constituent& c1, const Constituent& c2){ return c1.p().P() > c2.p().P(); });

        //Get constituent variables
        for(unsigned int i = 0; i < RF_constituents.size(); ++i)
        {
            varMap["j" + std::to_string(i + 1) + "_p"]     = RF_constituents[i].p().P();
            varMap["j" + std::to_string(i + 1) + "_theta"] = RF_constituents[i].p().Angle(topCand.p().Vect());
            varMap["j" + std::to_string(i + 1) + "_phi"]   = RF_constituents[i].p().Phi();
            varMap["j" + std::to_string(i + 1) + "_eta"]   = RF_constituents[i].p().Eta();
            varMap["j" + std::to_string(i + 1) + "_pt"]    = RF_constituents[i].p().Pt();
            varMap["j" + std::to_string(i + 1) + "_m"]     = RF_constituents[i].p().M();
            varMap["j" + std::to_string(i + 1) + "_CSV"]   = RF_constituents[i].getBTagDisc();
            varMap["j" + std::to_string(i + 1) + "_QGL"]   = RF_constituents[i].getQGLikelihood();
            varMap["j" + std::to_string(i + 1) + "_Chrg"]  = RF_constituents[i].getJetCharge();

            //index of next jet (assumes < 4 jets)
            unsigned int iNext = (i + 1) % RF_constituents.size();
            unsigned int iMin = std::min(i, iNext);
            unsigned int iMax = std::max(i, iNext);

            //Calculate delta angle variables
            varMap["dTheta" + std::to_string(iMin + 1) + std::to_string(iMax + 1)] = RF_constituents[iMin].p().Angle(RF_constituents[iMax].p().Vect());

            //calculate pair masses
            auto jetPair = RF_constituents[i].p() + RF_constituents[iNext].p();
            varMap["j"   + std::to_string(iMin + 1) + std::to_string(iMax + 1) + "_m"] = jetPair.M();

            TLorentzVector j1 = RF_constituents[i].p();
            j1.Boost(-jetPair.BoostVector());
            TLorentzVector j2 = RF_constituents[iNext].p();
            j2.Boost(-jetPair.BoostVector());
            varMap["j"   + std::to_string(iMin + 1) + std::to_string(iMax + 1) + "_dTheta"] = jetPair.Angle(j1.Vect());
        }

        return varMap;
    }

    std::vector<std::string> getMVAVars()
    {
        return std::vector<std::string>({"cand_m", "j12_m", "j13_m", "j23_m", "j1_p", "j2_p", "j3_p", "dTheta12", "dTheta23", "dTheta13", "j1_CSV", "j2_CSV", "j3_CSV", "j1_QGL", "j2_QGL", "j3_QGL"});
        //"cand_m", "j12_m", "j13_m", "j23_m", "dPhi12", "dPhi23", "dPhi13", "j1_p", "j2_p", "j3_p", "j1_theta", "j2_theta", "j3_theta", "j1_CSV", "j2_CSV", "j3_CSV", "j1_QGL", "j2_QGL", "j3_QGL", "j12_dTheta", "j13_dTheta", "j23_dTheta"});
    }
}
