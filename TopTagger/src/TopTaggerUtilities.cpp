#include "TopTagger/TopTagger/include/TopTaggerUtilities.h"

#include "TopTagger/TopTagger/include/Constituent.h"
#include "TopTagger/TopTagger/include/TopTaggerResults.h"

#include "TopTagger/CfgParser/include/TTException.h"

#include "Math/VectorUtil.h"

#include <map>

namespace ttUtility
{
    ConstGenInputs::ConstGenInputs() : genDecayLVec_(nullptr), genDecayPdgIdVec_(nullptr), genDecayIdxVec_(nullptr), genDecayMomIdxVec_(nullptr) {}

    ConstGenInputs::ConstGenInputs(const std::vector<TLorentzVector>& genDecayLVec, const std::vector<int>& genDecayPdgIdVec, const std::vector<int>& genDecayIdxVec, const std::vector<int>& genDecayMomIdxVec) : genDecayLVec_(&genDecayLVec), genDecayPdgIdVec_(&genDecayPdgIdVec), genDecayIdxVec_(&genDecayIdxVec), genDecayMomIdxVec_(&genDecayMomIdxVec)
    {
        hadGenTops_ = GetHadTopLVec(*genDecayLVec_, *genDecayPdgIdVec_, *genDecayIdxVec_, *genDecayMomIdxVec_);
        for(const auto& top : hadGenTops_)
        {
            hadGenTopDaughters_.push_back(GetTopdauLVec(top, *genDecayLVec_, *genDecayPdgIdVec_, *genDecayIdxVec_, *genDecayMomIdxVec_));
        }
    }

    ConstAK4Inputs::ConstAK4Inputs(const std::vector<TLorentzVector>& jetsLVec, const std::vector<double>& btagFactors, const std::vector<double>& qgLikelihood) : ConstGenInputs(), jetsLVec_(&jetsLVec), btagFactors_(&btagFactors), qgLikelihood_(&qgLikelihood) {}

    ConstAK4Inputs::ConstAK4Inputs(const std::vector<TLorentzVector>& jetsLVec, const std::vector<double>& btagFactors, const std::vector<double>& qgLikelihood, const std::vector<TLorentzVector>& genDecayLVec, const std::vector<int>& genDecayPdgIdVec, const std::vector<int>& genDecayIdxVec, const std::vector<int>& genDecayMomIdxVec) : ConstGenInputs(genDecayLVec, genDecayPdgIdVec, genDecayIdxVec, genDecayMomIdxVec), jetsLVec_(&jetsLVec), btagFactors_(&btagFactors), qgLikelihood_(&qgLikelihood) {}

    ConstAK8Inputs::ConstAK8Inputs(const std::vector<TLorentzVector>& jetsLVec, const std::vector<double>& tau1, const std::vector<double>& tau2, const std::vector<double>& tau3, const std::vector<double>& softDropMass, const std::vector<TLorentzVector>& subJetsLVec) : ConstGenInputs(), jetsLVec_(&jetsLVec), tau1_(&tau1), tau2_(&tau2), tau3_(&tau3), softDropMass_(&softDropMass), subjetsLVec_(&subJetsLVec) {}

    ConstAK8Inputs::ConstAK8Inputs(const std::vector<TLorentzVector>& jetsLVec, const std::vector<double>& tau1, const std::vector<double>& tau2, const std::vector<double>& tau3, const std::vector<double>& softDropMass, const std::vector<TLorentzVector>& subJetsLVec, const std::vector<TLorentzVector>& genDecayLVec, const std::vector<int>& genDecayPdgIdVec, const std::vector<int>& genDecayIdxVec, const std::vector<int>& genDecayMomIdxVec) : ConstGenInputs(genDecayLVec, genDecayPdgIdVec, genDecayIdxVec, genDecayMomIdxVec), jetsLVec_(&jetsLVec), tau1_(&tau1), tau2_(&tau2), tau3_(&tau3), softDropMass_(&softDropMass), subjetsLVec_(&subJetsLVec) {}

    void ConstAK4Inputs::packageConstituents(std::vector<Constituent>& constituents)
    {
        //vector holding constituents to be created
        //std::vector<Constituent> constituents;

        //Safety check that jet and b-tag vectors are the same length
        if(jetsLVec_->size() != btagFactors_->size() || jetsLVec_->size() != qgLikelihood_->size())
        {
            THROW_TTEXCEPTION("Unequal vector size!!!!!!!\n" + std::to_string(jetsLVec_->size()) + "\t" + std::to_string(qgLikelihood_->size()));
        }
        
        //Construct constituents in place in the vector
        for(unsigned int iJet = 0; iJet < jetsLVec_->size(); ++iJet)
        {
            constituents.emplace_back((*jetsLVec_)[iJet], (*btagFactors_)[iJet], (*qgLikelihood_)[iJet]);

            //Get gen matches if the required info is provided
            if(genDecayLVec_ && genDecayPdgIdVec_ && genDecayIdxVec_ && genDecayMomIdxVec_)
            {
                for(unsigned int iGenTop = 0; iGenTop < hadGenTops_.size(); ++iGenTop)
                {
                    for(const auto& genDaughter : hadGenTopDaughters_[iGenTop])
                    {
                        double dR = ROOT::Math::VectorUtil::DeltaR((*jetsLVec_)[iJet], *genDaughter);
                        if(dR < 0.4)
                        {
                            constituents.back().addGenMatch(hadGenTops_[iGenTop], genDaughter);
                        }
                    }
                }
            }
        }
    }

    void ConstAK8Inputs::packageConstituents(std::vector<Constituent>& constituents)
    {
        //vector holding constituents to be created
        //std::vector<Constituent> constituents;

        //Safety check that jet and b-tag vectors are the same length
        if(jetsLVec_->size() != tau1_->size() || jetsLVec_->size() != tau2_->size() || jetsLVec_->size() != tau3_->size() || jetsLVec_->size() != softDropMass_->size())
        {
            THROW_TTEXCEPTION("Unequal vector size!!!!!!!\n");
        }

        //Construct constituents in place in the vector
        for(unsigned int iJet = 0; iJet < jetsLVec_->size(); ++iJet)
        {
            //Calculate matching subjets 
            // For each tagged top/W, find the corresponding subjets
            std::vector<TLorentzVector> subjets;
            for(const TLorentzVector& puppiSubJet : *subjetsLVec_)
            {
                double myDR = ROOT::Math::VectorUtil::DeltaR((*jetsLVec_)[iJet], puppiSubJet);
                if (myDR < 0.8)
                {
                    subjets.push_back(puppiSubJet);
                }
            }
            // If more than 2 matches, find the best combination of two subjets
            if (subjets.size() > 2) 
            {
                double min_diff = 999999.;
                int min_j=0, min_k=1;
                for (unsigned int j=0 ; j<subjets.size(); ++j)
                {
                    for (unsigned int k=j+1; k<subjets.size(); ++k)
                    {
                        TLorentzVector diff_LV = (*jetsLVec_)[iJet] - subjets[j] - subjets[k];
                        double diff = abs(diff_LV.M());
                        if(diff < min_diff)
                        {
                            min_diff = diff;
                            min_j = j;
                            min_k = k;
                        }
                    }
                }
                subjets = {subjets[min_j], subjets[min_k]};
            }

            //Emplace new constituent into vector
            constituents.emplace_back((*jetsLVec_)[iJet], (*tau1_)[iJet], (*tau2_)[iJet], (*tau3_)[iJet], (*softDropMass_)[iJet], subjets);
        }
    }

    std::vector<Constituent> packageConstituents(const std::vector<TLorentzVector>& jetsLVec, const std::vector<double>& btagFactors, const std::vector<double>& qgLikelihood)
    {
        return packageConstituents(ConstAK4Inputs(jetsLVec, btagFactors, qgLikelihood));
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
            RF_constituents.emplace_back(p4, constitutent->getBTagDisc(), constitutent->getQGLikelihood());
        }
            
        //re-sort constituents by p after deboosting
        std::sort(RF_constituents.begin(), RF_constituents.end(), [](const Constituent& c1, const Constituent& c2){ return c1.p().P() > c2.p().P(); });

        //Get constituent variables
        for(unsigned int i = 0; i < RF_constituents.size(); ++i)
        {
            varMap["j" + std::to_string(i + 1) + "_p"]     = RF_constituents[i].p().P();
            //varMap["j" + std::to_string(i + 1) + "_theta"] = RF_constituents[i].p().Angle(topCand.p().Vect());
            varMap["j" + std::to_string(i + 1) + "_phi"]   = RF_constituents[i].p().Phi();
            varMap["j" + std::to_string(i + 1) + "_eta"]   = RF_constituents[i].p().Eta();
            varMap["j" + std::to_string(i + 1) + "_pt"]    = RF_constituents[i].p().Pt();
            varMap["j" + std::to_string(i + 1) + "_m"]     = RF_constituents[i].p().M();
            varMap["j" + std::to_string(i + 1) + "_CSV"]   = RF_constituents[i].getBTagDisc();
            //Here we face the QGL if it is a b jet
            varMap["j" + std::to_string(i + 1) + "_QGL"]   = (RF_constituents[i].getBTagDisc() > 0.800)?(1.0):(RF_constituents[i].getQGLikelihood());

            //index of next jet (assumes < 4 jets)
            unsigned int iNext = (i + 1) % RF_constituents.size();
            unsigned int iMin = std::min(i, iNext);
            unsigned int iMax = std::max(i, iNext);

            //Calculate delta angle variables
            varMap["dTheta" + std::to_string(iMin + 1) + std::to_string(iMax + 1)] = RF_constituents[iMin].p().Angle(RF_constituents[iMax].p().Vect());

            //calculate pair masses
            auto jetPair = RF_constituents[i].p() + RF_constituents[iNext].p();
            varMap["j"   + std::to_string(iMin + 1) + std::to_string(iMax + 1) + "_m"] = jetPair.M();
        }

        return varMap;
    }

    std::vector<std::string> getMVAVars()
    {
        return std::vector<std::string>({"cand_m", "j12_m", "j13_m", "j23_m", "j1_p", "j2_p", "j3_p", "dTheta12", "dTheta23", "dTheta13", "j1_CSV", "j2_CSV", "j3_CSV", "j1_QGL", "j2_QGL", "j3_QGL"});
    }

    std::vector<TLorentzVector> GetHadTopLVec(const std::vector<TLorentzVector>& genDecayLVec, const std::vector<int>& genDecayPdgIdVec, const std::vector<int>& genDecayIdxVec, const std::vector<int>& genDecayMomIdxVec)
    {
        std::vector<TLorentzVector> tLVec;
        for(unsigned it=0; it<genDecayLVec.size(); it++)
        {
            int pdgId = genDecayPdgIdVec.at(it);
            if(abs(pdgId)==6)
            {
                for(unsigned ig=0; ig<genDecayLVec.size(); ig++)
                {
                    if( genDecayMomIdxVec.at(ig) == genDecayIdxVec.at(it) )
                    {
                        int pdgId = genDecayPdgIdVec.at(ig);
                        if(abs(pdgId)==24)
                        {
                            int flag = 0;
                            for(unsigned iq=0; iq<genDecayLVec.size(); iq++)
                            {
                                if( genDecayMomIdxVec.at(iq) == genDecayIdxVec.at(ig) ) 
                                {
                                    int pdgid = genDecayPdgIdVec.at(iq);
                                    if(abs(pdgid)== 11 || abs(pdgid)== 13 || abs(pdgid)== 15) flag++;
                                }
                            }
                            if(!flag) tLVec.push_back(genDecayLVec.at(it));
                        }
                    }
                }//dau. loop
            }//top cond
        }//genloop
        return tLVec;
    }

    std::vector<const TLorentzVector*> GetTopdauLVec(const TLorentzVector& top, const std::vector<TLorentzVector>& genDecayLVec, const std::vector<int>& genDecayPdgIdVec, const std::vector<int>& genDecayIdxVec, const std::vector<int>& genDecayMomIdxVec)
    {
        std::vector<const TLorentzVector*>topdauLVec;
        for(unsigned it=0; it<genDecayLVec.size(); it++)
        {
            if(genDecayLVec[it]==top){
                for(unsigned ig=0; ig<genDecayLVec.size(); ig++)
                {
                    if( genDecayMomIdxVec.at(ig) == genDecayIdxVec.at(it) )
                    {
                        int pdgId = genDecayPdgIdVec.at(ig);
                        if(abs(pdgId)==5) topdauLVec.push_back(&(genDecayLVec[ig]));
                        if(abs(pdgId)==24)
                        {
                            //topdauLVec.push_back(genDecayLVec[ig]);	 
                            for(unsigned iq=0; iq<genDecayLVec.size(); iq++)
                            {
                                if( genDecayMomIdxVec.at(iq) == genDecayIdxVec.at(ig) ) 
                                {
                                    int pdgid = genDecayPdgIdVec.at(iq);
                                    if(abs(pdgid)!= 11 && abs(pdgid)!= 13 && abs(pdgid)!= 15) topdauLVec.push_back(&(genDecayLVec[iq]));
                                }
                            }
                        }
                    }
                }//dau. loop
            }//top cand.
        }//gen loop
        return topdauLVec;
    }

}
