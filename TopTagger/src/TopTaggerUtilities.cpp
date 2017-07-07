#include "TopTagger/TopTagger/include/TopTaggerUtilities.h"

#include "TopTagger/TopTagger/include/Constituent.h"
#include "TopTagger/TopTagger/include/TopTaggerResults.h"

#include "TopTagger/CfgParser/include/TTException.h"

#include "Math/VectorUtil.h"
#include "TF1.h"
#include "TFile.h"

#include <map>

namespace ttUtility
{
    ConstGenInputs::ConstGenInputs() : hadGenTops_(nullptr), hadGenTopDaughters_(nullptr) {}

    ConstGenInputs::ConstGenInputs(const std::vector<TLorentzVector>& hadGenTops, const std::vector<std::vector<const TLorentzVector*>>& hadGenTopDaughters) : hadGenTops_(&hadGenTops), hadGenTopDaughters_(&hadGenTopDaughters) {}

    ConstAK4Inputs::ConstAK4Inputs(const std::vector<TLorentzVector>& jetsLVec, const std::vector<double>& btagFactors, const std::vector<double>& qgLikelihood) : ConstGenInputs(), jetsLVec_(&jetsLVec), btagFactors_(&btagFactors), qgLikelihood_(&qgLikelihood), qgMult_(nullptr), qgPtD_(nullptr), qgAxis1_(nullptr), qgAxis2_(nullptr)
    {
        recoJetschargedHadronEnergyFraction_ = nullptr;
        recoJetschargedEmEnergyFraction_ = nullptr;
        recoJetsneutralEmEnergyFraction_ = nullptr;
        recoJetsmuonEnergyFraction_ = nullptr;
        PhotonEnergyFraction_ = nullptr;
        ElectronEnergyFraction_ = nullptr;
        ChargedHadronMultiplicity_ = nullptr;
        NeutralHadronMultiplicity_ = nullptr;
        PhotonMultiplicity_ = nullptr;
        ElectronMultiplicity_ = nullptr;
        MuonMultiplicity_ = nullptr;
        recoJetsCharge_0_ = nullptr;
    }

    ConstAK4Inputs::ConstAK4Inputs(const std::vector<TLorentzVector>& jetsLVec, const std::vector<double>& btagFactors, const std::vector<double>& qgLikelihood, const std::vector<TLorentzVector>& hadGenTops, const std::vector<std::vector<const TLorentzVector*>>& hadGenTopDaughters) : ConstGenInputs(hadGenTops, hadGenTopDaughters), jetsLVec_(&jetsLVec), btagFactors_(&btagFactors), qgLikelihood_(&qgLikelihood), qgMult_(nullptr), qgPtD_(nullptr), qgAxis1_(nullptr), qgAxis2_(nullptr) {}

    ConstAK8Inputs::ConstAK8Inputs(const std::vector<TLorentzVector>& jetsLVec, const std::vector<double>& tau1, const std::vector<double>& tau2, const std::vector<double>& tau3, const std::vector<double>& softDropMass, const std::vector<TLorentzVector>& subJetsLVec) : ConstGenInputs(), jetsLVec_(&jetsLVec), tau1_(&tau1), tau2_(&tau2), tau3_(&tau3), softDropMass_(&softDropMass), subjetsLVec_(&subJetsLVec)
    {
        puppisd_corrGEN_ = nullptr;
        puppisd_corrRECO_cen_ = nullptr;
        puppisd_corrRECO_for_ = nullptr;
    }

    ConstAK8Inputs::ConstAK8Inputs(const std::vector<TLorentzVector>& jetsLVec, const std::vector<double>& tau1, const std::vector<double>& tau2, const std::vector<double>& tau3, const std::vector<double>& softDropMass, const std::vector<TLorentzVector>& subJetsLVec, const std::vector<TLorentzVector>& hadGenTops, const std::vector<std::vector<const TLorentzVector*>>& hadGenTopDaughters) : ConstGenInputs(hadGenTops, hadGenTopDaughters), jetsLVec_(&jetsLVec), tau1_(&tau1), tau2_(&tau2), tau3_(&tau3), softDropMass_(&softDropMass), subjetsLVec_(&subJetsLVec)
    {
        puppisd_corrGEN_ = nullptr;
        puppisd_corrRECO_cen_ = nullptr;
        puppisd_corrRECO_for_ = nullptr;
    }

    void ConstAK4Inputs::addQGLVectors(const std::vector<int>& qgMult, const std::vector<double>& qgPtD, const std::vector<double>& qgAxis1, const std::vector<double>& qgAxis2)
    {
        qgMult_ = &qgMult;
        qgPtD_ = &qgPtD;
        qgAxis1_ = &qgAxis1;
        qgAxis2_ = &qgAxis2;
    }

    void ConstAK4Inputs::addSupplamentalVectors(const std::vector<double>& recoJetschargedHadronEnergyFraction,
                                                const std::vector<double>& recoJetschargedEmEnergyFraction,
                                                const std::vector<double>& recoJetsneutralEmEnergyFraction,
                                                const std::vector<double>& recoJetsmuonEnergyFraction,
                                                const std::vector<double>& PhotonEnergyFraction,
                                                const std::vector<double>& ElectronEnergyFraction,
                                                const std::vector<double>& ChargedHadronMultiplicity,
                                                const std::vector<double>& NeutralHadronMultiplicity,
                                                const std::vector<double>& PhotonMultiplicity,
                                                const std::vector<double>& ElectronMultiplicity,
                                                const std::vector<double>& MuonMultiplicity,
                                                const std::vector<double>& recoJetsCharge_0)
    {
        recoJetschargedHadronEnergyFraction_ = &recoJetschargedHadronEnergyFraction;
        recoJetschargedEmEnergyFraction_ = &recoJetschargedEmEnergyFraction;
        recoJetsneutralEmEnergyFraction_ = &recoJetsneutralEmEnergyFraction;
        recoJetsmuonEnergyFraction_ = &recoJetsmuonEnergyFraction;
        PhotonEnergyFraction_ = &PhotonEnergyFraction;
        ElectronEnergyFraction_ = &ElectronEnergyFraction;
        ChargedHadronMultiplicity_ = &ChargedHadronMultiplicity;
        NeutralHadronMultiplicity_ = &NeutralHadronMultiplicity;
        PhotonMultiplicity_ = &PhotonMultiplicity;
        ElectronMultiplicity_ = &ElectronMultiplicity;
        MuonMultiplicity_ = &MuonMultiplicity;
        recoJetsCharge_0_ = &recoJetsCharge_0;
    }

    void ConstAK4Inputs::packageConstituents(std::vector<Constituent>& constituents)
    {
        //vector holding constituents to be created
        //std::vector<Constituent> constituents;

        //Safety check that jet and b-tag vectors are the same length
        if(jetsLVec_->size() != btagFactors_->size() || jetsLVec_->size() != qgLikelihood_->size())
        {
            THROW_TTEXCEPTION("Unequal vector size!!!!!!!\n" + std::to_string(jetsLVec_->size()) + "\t" + std::to_string(qgLikelihood_->size()));
        }

        if(qgMult_ && qgPtD_ && qgAxis1_ && qgAxis2_) 
        {
            if(jetsLVec_->size() != qgMult_->size() || jetsLVec_->size() != qgPtD_->size() || jetsLVec_->size() != qgAxis1_->size() || jetsLVec_->size() != qgAxis2_->size())
            {
                THROW_TTEXCEPTION("Unequal vector size (QGL)!!!!!!!\n");
            }
        }

        //Construct constituents in place in the vector
        for(unsigned int iJet = 0; iJet < jetsLVec_->size(); ++iJet)
        {
            constituents.emplace_back((*jetsLVec_)[iJet], (*btagFactors_)[iJet], (*qgLikelihood_)[iJet]);

            //Add additional QGL info if it is provided 
            if(qgMult_ && qgPtD_ && qgAxis1_ && qgAxis2_) 
            {
                constituents.back().setQGLVars((*qgMult_)[iJet], (*qgPtD_)[iJet], (*qgAxis1_)[iJet], (*qgAxis2_)[iJet]);
            }

            if(recoJetschargedHadronEnergyFraction_) constituents.back().setExtraVar("recoJetschargedHadronEnergyFraction", (*recoJetschargedHadronEnergyFraction_)[iJet]);
            if(recoJetschargedEmEnergyFraction_)     constituents.back().setExtraVar("recoJetschargedEmEnergyFraction",     (*recoJetschargedEmEnergyFraction_)[iJet]);
            if(recoJetsneutralEmEnergyFraction_)     constituents.back().setExtraVar("recoJetsneutralEmEnergyFraction",     (*recoJetsneutralEmEnergyFraction_)[iJet]);
            if(recoJetsmuonEnergyFraction_)          constituents.back().setExtraVar("recoJetsmuonEnergyFraction",          (*recoJetsmuonEnergyFraction_)[iJet]);
            if(PhotonEnergyFraction_)                constituents.back().setExtraVar("PhotonEnergyFraction",                (*PhotonEnergyFraction_)[iJet]);
            if(ElectronEnergyFraction_)              constituents.back().setExtraVar("ElectronEnergyFraction",              (*ElectronEnergyFraction_)[iJet]);
            if(ChargedHadronMultiplicity_)           constituents.back().setExtraVar("ChargedHadronMultiplicity",           (*ChargedHadronMultiplicity_)[iJet]);
            if(NeutralHadronMultiplicity_)           constituents.back().setExtraVar("NeutralHadronMultiplicity",           (*NeutralHadronMultiplicity_)[iJet]);
            if(PhotonMultiplicity_)                  constituents.back().setExtraVar("PhotonMultiplicity",                  (*PhotonMultiplicity_)[iJet]);
            if(ElectronMultiplicity_)                constituents.back().setExtraVar("ElectronMultiplicity",                (*ElectronMultiplicity_)[iJet]);
            if(MuonMultiplicity_)                    constituents.back().setExtraVar("MuonMultiplicity",                    (*MuonMultiplicity_)[iJet]);
            if(recoJetsCharge_0_)                    constituents.back().setExtraVar("recoJetsCharge_0",                    (*recoJetsCharge_0_)[iJet]);

            //Get gen matches if the required info is provided
            if(hadGenTops_ && hadGenTopDaughters_)
            {
                for(unsigned int iGenTop = 0; iGenTop < hadGenTops_->size(); ++iGenTop)
                {
                    for(const auto& genDaughter : (*hadGenTopDaughters_)[iGenTop])
                    {
                        double dR = ROOT::Math::VectorUtil::DeltaR((*jetsLVec_)[iJet], *genDaughter);
                        if(dR < 0.4)
                        {
                            constituents.back().addGenMatch((*hadGenTops_)[iGenTop], genDaughter);
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
            constituents.emplace_back((*jetsLVec_)[iJet], (*tau1_)[iJet], (*tau2_)[iJet], (*tau3_)[iJet], (*softDropMass_)[iJet], subjets, getPUPPIweight((*jetsLVec_)[iJet].Pt(), (*jetsLVec_)[iJet].Eta()));

            //Get gen matches if the required info is provided
            if(hadGenTops_ && hadGenTopDaughters_)
            {
                for(unsigned int iGenTop = 0; iGenTop < hadGenTops_->size(); ++iGenTop)
                {
                    for(const auto& genDaughter : (*hadGenTopDaughters_)[iGenTop])
                    {
                        for(const auto& subjet : subjets)
                        {
                            double dR = ROOT::Math::VectorUtil::DeltaR(subjet, *genDaughter);
                            if(dR < 0.4)
                            {
                                constituents.back().addGenMatch((*hadGenTops_)[iGenTop], genDaughter);
                            }
                        }
                    }
                }
            }
        }
    }

    std::vector<TLorentzVector> ConstAK8Inputs::denominator(const double ptCut) const
    {
        std::vector<TLorentzVector> returnVector;
        for(auto& jet : *jetsLVec_)
        {
            if(jet.Pt() > ptCut) returnVector.push_back(jet);
        }
        return returnVector;
    }

    double ConstAK8Inputs::getPUPPIweight(double puppipt, double puppieta ) const
    {
        double genCorr  = 1.;
        double recoCorr = 1.;

        //The correction is derived for jet > 200GeV.
        //It would return negative weight for low PT jet
        if (puppipt < 200) return 1.;
        if(puppisd_corrGEN_ && puppisd_corrRECO_cen_ && puppisd_corrRECO_for_)
        {
            genCorr =  puppisd_corrGEN_->Eval( puppipt );
            if( fabs(puppieta) <= 1.3 )
            {
                recoCorr = puppisd_corrRECO_cen_->Eval( puppipt );
            }
            else
            {
                recoCorr = puppisd_corrRECO_for_->Eval( puppipt );
            }
        }

        return genCorr * recoCorr;
    }

    void ConstAK8Inputs::setWMassCorrHistos(const std::string& fname)
    {
        TF1* puppisd_corrGEN = nullptr;
        TF1* puppisd_corrRECO_cen = nullptr;
        TF1* puppisd_corrRECO_for = nullptr;

        ConstAK8Inputs::prepHistosForWCorrectionFactors(fname, puppisd_corrGEN, puppisd_corrRECO_cen, puppisd_corrRECO_for);
        setWMassCorrHistos(puppisd_corrGEN,puppisd_corrRECO_cen, puppisd_corrRECO_for);
    }

    void ConstAK8Inputs::setWMassCorrHistos(TF1* puppisd_corrGEN, TF1* puppisd_corrRECO_cen, TF1* puppisd_corrRECO_for)
    {
        puppisd_corrGEN_ = puppisd_corrGEN;
        puppisd_corrRECO_cen_ = puppisd_corrRECO_cen;
        puppisd_corrRECO_for_ = puppisd_corrRECO_for;
    }

    void ConstAK8Inputs::prepHistosForWCorrectionFactors(const std::string& fname, TF1* puppisd_corrGEN, TF1* puppisd_corrRECO_cen, TF1* puppisd_corrRECO_for)
    {
        TFile* file = TFile::Open(fname.c_str(),"READ");
        if(file)
        {
            puppisd_corrGEN      = (TF1*)file->Get("puppiJECcorr_gen");
            puppisd_corrRECO_cen = (TF1*)file->Get("puppiJECcorr_reco_0eta1v3");
            puppisd_corrRECO_for = (TF1*)file->Get("puppiJECcorr_reco_1v3eta2v5");

            file->Close();
            delete file;
        }
        else
        {
            THROW_TTEXCEPTION("W mass correction file not found w mass!!!!!!!" + fname + "\n");
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


    std::map<std::string, double> createMVAInputs(const TopObject& topCand, const double csvThresh)
    {
        std::map<std::string, double> varMap;

        //Get top candidate variables
        varMap["cand_pt"]    = topCand.p().Pt();
        varMap["cand_eta"]   = topCand.p().Eta();
        varMap["cand_phi"]   = topCand.p().Phi();
        varMap["cand_m"]     = topCand.p().M();
        varMap["cand_dRMax"] = topCand.getDRmax();

        //Get Constituents
        //Get a copy instead of the reference
        std::vector<Constituent const *> top_constituents = topCand.getConstituents();

        //resort by CSV
        std::sort(top_constituents.begin(), top_constituents.end(), [](const Constituent * const c1, const Constituent * const c2){ return c1->getBTagDisc() > c2->getBTagDisc(); });

        //Get constituent variables before deboost
        for(unsigned int i = 0; i < top_constituents.size(); ++i)
        {
            varMap["j" + std::to_string(i + 1) + "_phi_lab"] = top_constituents[i]->p().Phi();
            varMap["j" + std::to_string(i + 1) + "_eta_lab"] = top_constituents[i]->p().Eta();
            varMap["j" + std::to_string(i + 1) + "_pt_lab"]  = top_constituents[i]->p().Pt();
            varMap["j" + std::to_string(i + 1) + "_m_lab"]   = top_constituents[i]->p().M();
            varMap["j" + std::to_string(i + 1) + "_CSV_lab"] = top_constituents[i]->getBTagDisc();
            //Here we fake the QGL if it is a b jet
            varMap["j" + std::to_string(i + 1) + "_QGL_lab"] = (top_constituents[i]->getBTagDisc() > csvThresh)?(1.0):(top_constituents[i]->getQGLikelihood());
            varMap["j" + std::to_string(i + 1) + "_qgMult_lab"]  = top_constituents[i]->getQGMult();
            varMap["j" + std::to_string(i + 1) + "_qgPtD_lab"]   = top_constituents[i]->getQGPtD();
            varMap["j" + std::to_string(i + 1) + "_qgAxis1_lab"] = top_constituents[i]->getQGAxis1();
            varMap["j" + std::to_string(i + 1) + "_qgAxis2_lab"] = top_constituents[i]->getQGAxis2();

            //index of next jet (assumes < 4 jets)
            unsigned int iNext = (i + 1) % top_constituents.size();
            unsigned int iNNext = (i + 2) % top_constituents.size();
            unsigned int iMin = std::min(i, iNext);
            unsigned int iMax = std::max(i, iNext);

            //Calculate the angle variables
            varMap["dR" + std::to_string(iMin + 1) + std::to_string(iMax + 1) + "_lab"] = ROOT::Math::VectorUtil::DeltaR(top_constituents[i]->p(), top_constituents[iNext]->p());
            varMap["dR" + std::to_string(iNNext + 1) + "_" + std::to_string(iMin + 1) + std::to_string(iMax + 1) + "_lab"] = ROOT::Math::VectorUtil::DeltaR(top_constituents[iNNext]->p(), top_constituents[i]->p() + top_constituents[iNext]->p());

            //calculate pair masses
            auto jetPair = top_constituents[i]->p() + top_constituents[iNext]->p();
            varMap["j"   + std::to_string(iMin + 1) + std::to_string(iMax + 1) + "_m_lab"] = jetPair.M();
        }

        varMap["dRPtTop"] = varMap["dR1_23_lab"] * varMap["cand_pt"];
        if( top_constituents.size() >= 3)
        {
            varMap["dRPtW"] = varMap["dR23_lab"] * (top_constituents[1]->p() + top_constituents[2]->p()).Pt();
            double var_sd_0 = top_constituents[2]->p().Pt()/(top_constituents[1]->p().Pt()+top_constituents[2]->p().Pt());
            varMap["sd_n2"] = var_sd_0/std::pow(varMap["dR23_lab"], -2);
        }
        else
        {
            varMap["dRPtW"] = 0.0;
            varMap["sd_n2"] = 0.0;
        }



        std::vector<Constituent> RF_constituents;

        for(const auto& constitutent : top_constituents)
        {
            TLorentzVector p4(constitutent->p());
            p4.Boost(-topCand.p().BoostVector());
            RF_constituents.emplace_back(*constitutent);
            RF_constituents.back().setP(p4);
        }

        //re-sort constituents by p after deboosting
        std::sort(RF_constituents.begin(), RF_constituents.end(), [](const Constituent& c1, const Constituent& c2){ return c1.p().P() > c2.p().P(); });

        //Get constituent variables
        for(unsigned int i = 0; i < RF_constituents.size(); ++i)
        {
            varMap["j" + std::to_string(i + 1) + "_p"]     = RF_constituents[i].p().P();
            //varMap["j" + std::to_string(i + 1) + "_phi"]   = RF_constituents[i].p().Phi();
            //varMap["j" + std::to_string(i + 1) + "_eta"]   = RF_constituents[i].p().Eta();
            //varMap["j" + std::to_string(i + 1) + "_pt"]    = RF_constituents[i].p().Pt();
            varMap["j" + std::to_string(i + 1) + "_m"]     = RF_constituents[i].p().M();
            varMap["j" + std::to_string(i + 1) + "_CSV"]   = RF_constituents[i].getBTagDisc();
            //Here we fake the QGL if it is a b jet
            varMap["j" + std::to_string(i + 1) + "_QGL"]   = (RF_constituents[i].getBTagDisc() > csvThresh)?(1.0):(RF_constituents[i].getQGLikelihood());
            varMap["j" + std::to_string(i + 1) + "_qgMult"]   = RF_constituents[i].getQGMult();
            varMap["j" + std::to_string(i + 1) + "_qgPtD"]   = RF_constituents[i].getQGPtD();
            varMap["j" + std::to_string(i + 1) + "_qgAxis1"]   = RF_constituents[i].getQGAxis1();
            varMap["j" + std::to_string(i + 1) + "_qgAxis2"]   = RF_constituents[i].getQGAxis2();

            varMap["j" + std::to_string(i + 1) + "_chargedHadEFrac"] = RF_constituents[i].getExtraVar("recoJetschargedHadronEnergyFraction");
            varMap["j" + std::to_string(i + 1) + "_chargedEmEFrac"] = RF_constituents[i].getExtraVar("recoJetschargedEmEnergyFraction");
            varMap["j" + std::to_string(i + 1) + "_neutralEmEFrac"] = RF_constituents[i].getExtraVar("recoJetsneutralEmEnergyFraction");
            varMap["j" + std::to_string(i + 1) + "_muonEFrac"] = RF_constituents[i].getExtraVar("recoJetsmuonEnergyFraction");
            varMap["j" + std::to_string(i + 1) + "_photonEFrac"] = RF_constituents[i].getExtraVar("PhotonEnergyFraction");
            varMap["j" + std::to_string(i + 1) + "_elecEFrac"] = RF_constituents[i].getExtraVar("ElectronEnergyFraction");
            varMap["j" + std::to_string(i + 1) + "_chargedHadMult"] = RF_constituents[i].getExtraVar("ChargedHadronMultiplicity");
            varMap["j" + std::to_string(i + 1) + "_neutralHadMult"] = RF_constituents[i].getExtraVar("NeutralHadronMultiplicity");
            varMap["j" + std::to_string(i + 1) + "_photonMult"] = RF_constituents[i].getExtraVar("PhotonMultiplicity");
            varMap["j" + std::to_string(i + 1) + "_elecMult"] = RF_constituents[i].getExtraVar("ElectronMultiplicity");
            varMap["j" + std::to_string(i + 1) + "_muonMult"] = RF_constituents[i].getExtraVar("MuonMultiplicity");
            varMap["j" + std::to_string(i + 1) + "_jetCharge"] = RF_constituents[i].getExtraVar("recoJetsCharge_0");

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
