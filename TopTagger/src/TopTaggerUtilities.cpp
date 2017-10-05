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

    ConstAK4Inputs::ConstAK4Inputs(const std::vector<TLorentzVector>& jetsLVec, const std::vector<double>& btagFactors, const std::vector<double>& qgLikelihood) : ConstGenInputs(), jetsLVec_(&jetsLVec), btagFactors_(&btagFactors), qgLikelihood_(&qgLikelihood), qgMult_(nullptr), qgPtD_(nullptr), qgAxis1_(nullptr), qgAxis2_(nullptr)    {}

    ConstAK4Inputs::ConstAK4Inputs(const std::vector<TLorentzVector>& jetsLVec, const std::vector<double>& btagFactors) : ConstGenInputs(), jetsLVec_(&jetsLVec), btagFactors_(&btagFactors), qgLikelihood_(nullptr), qgMult_(nullptr), qgPtD_(nullptr), qgAxis1_(nullptr), qgAxis2_(nullptr) {}

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

    void ConstAK4Inputs::addSupplamentalVector(const std::string& name, const std::vector<double>& vector)
    {
        extraInputVariables_[name] = &vector;
    }

    void ConstAK4Inputs::packageConstituents(std::vector<Constituent>& constituents)
    {
        //vector holding constituents to be created
        //std::vector<Constituent> constituents;

        //Safety check that jet and b-tag vectors are the same length
        //Special exception for qgLikelihood if it is empty (for slimplified tagger)
        if(jetsLVec_->size() != btagFactors_->size() || (qgLikelihood_ != nullptr && jetsLVec_->size() != qgLikelihood_->size()))
        {
            THROW_TTEXCEPTION("Unequal vector size!!!!!!!\n" + std::to_string(jetsLVec_->size()) + "\t" + std::to_string(btagFactors_->size()));
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
            constituents.emplace_back((*jetsLVec_)[iJet], (*btagFactors_)[iJet], (qgLikelihood_ != nullptr)?((*qgLikelihood_)[iJet]):(0.0));

            //Add additional QGL info if it is provided 
            if(qgMult_ && qgPtD_ && qgAxis1_ && qgAxis2_) 
            {
                constituents.back().setQGLVars((*qgMult_)[iJet], (*qgPtD_)[iJet], (*qgAxis1_)[iJet], (*qgAxis2_)[iJet]);
            }

            //Add any extra variables that have been added 
            for(const auto& extraVar : extraInputVariables_)
            {
                if(extraVar.second && iJet < extraVar.second->size()) constituents.back().setExtraVar(extraVar.first, (*extraVar.second)[iJet]);
                else THROW_TTEXCEPTION("Extra variable " + extraVar.first + "[" + std::to_string(iJet) + "] is not found!!!!!!!");
            }
            
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
                        double diff = fabs(diff_LV.M());
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

    inline double relu(const double x, const double bias = 0.0)
    {
        return (x > bias)?x:0.0;
    }

    std::map<std::string, double> createMVAInputs(const TopObject& topCand, const double csvThresh)
    {
        std::map<std::string, double> varMap;

        //Get top candidate variables
        varMap["cand_pt"]    = topCand.p().Pt();
        varMap["cand_p"]     = topCand.p().P();
        varMap["cand_eta"]   = topCand.p().Eta();
        varMap["cand_phi"]   = topCand.p().Phi();
        varMap["cand_m"]     = topCand.p().M();
        varMap["cand_dRMax"] = topCand.getDRmax();
        varMap["cand_dThetaMin"] = topCand.getDThetaMin();
        varMap["cand_dThetaMax"] = topCand.getDThetaMax();

        //Get Constituents
        //Get a copy instead of the reference
        std::vector<Constituent const *> top_constituents = topCand.getConstituents();

        //resort by CSV
        std::sort(top_constituents.begin(), top_constituents.end(), [](const Constituent * const c1, const Constituent * const c2){ return c1->getBTagDisc() > c2->getBTagDisc(); });

        //Get constituent variables before deboost
        for(unsigned int i = 0; i < top_constituents.size(); ++i)
        {
            //varMap["j" + std::to_string(i + 1) + "_phi_lab"] = top_constituents[i]->p().Phi();
            //varMap["j" + std::to_string(i + 1) + "_eta_lab"] = top_constituents[i]->p().Eta();
            //varMap["j" + std::to_string(i + 1) + "_pt_lab"]  = top_constituents[i]->p().Pt();
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

            //This is a bit silly
            TLorentzVector p4(RF_constituents[i].p());
            p4.Boost(topCand.p().BoostVector());
            varMap["j" + std::to_string(i + 1) + "_p_top"]     = p4.P();
            varMap["j" + std::to_string(i + 1) + "_theta_top"] = topCand.p().Angle(p4.Vect());
            varMap["j" + std::to_string(i + 1) + "_phi_top"]   = ROOT::Math::VectorUtil::DeltaPhi(RF_constituents[i].p(), RF_constituents[0].p());

            varMap["j" + std::to_string(i + 1) + "_phi_lab"]   = p4.Phi();
            varMap["j" + std::to_string(i + 1) + "_eta_lab"]   = p4.Eta();
            varMap["j" + std::to_string(i + 1) + "_pt_lab"]    = p4.Pt();
            
            varMap["j" + std::to_string(i + 1) + "_m"]     = RF_constituents[i].p().M();
            varMap["j" + std::to_string(i + 1) + "_CSV"]   = RF_constituents[i].getBTagDisc();
            //Here we fake the QGL if it is a b jet
            varMap["j" + std::to_string(i + 1) + "_QGL"]                                 = (RF_constituents[i].getBTagDisc() > csvThresh)?(1.0):(RF_constituents[i].getQGLikelihood());

            varMap["j" + std::to_string(i + 1) + "_qgMult"]                              = relu(RF_constituents[i].getExtraVar("qgMult"));
            varMap["j" + std::to_string(i + 1) + "_qgPtD"]                               = relu(RF_constituents[i].getExtraVar("qgPtD"));
            varMap["j" + std::to_string(i + 1) + "_qgAxis1"]                             = relu(RF_constituents[i].getExtraVar("qgAxis1"));
            varMap["j" + std::to_string(i + 1) + "_qgAxis2"]                             = relu(RF_constituents[i].getExtraVar("qgAxis2"));
            //varMap["j" + std::to_string(i + 1) + "_recoJetsFlavor"]                      = relu(RF_constituents[i].getExtraVar("recoJetsFlavor"));
            varMap["j" + std::to_string(i + 1) + "_recoJetsJecScaleRawToFull"]           = relu(RF_constituents[i].getExtraVar("recoJetsJecScaleRawToFull"));
            varMap["j" + std::to_string(i + 1) + "_recoJetschargedHadronEnergyFraction"] = relu(RF_constituents[i].getExtraVar("recoJetschargedHadronEnergyFraction"));
            varMap["j" + std::to_string(i + 1) + "_recoJetschargedEmEnergyFraction"]     = relu(RF_constituents[i].getExtraVar("recoJetschargedEmEnergyFraction"));
            varMap["j" + std::to_string(i + 1) + "_recoJetsneutralEmEnergyFraction"]     = relu(RF_constituents[i].getExtraVar("recoJetsneutralEmEnergyFraction"));
            varMap["j" + std::to_string(i + 1) + "_recoJetsmuonEnergyFraction"]          = relu(RF_constituents[i].getExtraVar("recoJetsmuonEnergyFraction"));
            varMap["j" + std::to_string(i + 1) + "_recoJetsHFHadronEnergyFraction"]      = relu(RF_constituents[i].getExtraVar("recoJetsHFHadronEnergyFraction"));
            varMap["j" + std::to_string(i + 1) + "_recoJetsHFEMEnergyFraction"]          = relu(RF_constituents[i].getExtraVar("recoJetsHFEMEnergyFraction"));
            varMap["j" + std::to_string(i + 1) + "_recoJetsneutralEnergyFraction"]       = relu(RF_constituents[i].getExtraVar("recoJetsneutralEnergyFraction"));
            varMap["j" + std::to_string(i + 1) + "_PhotonEnergyFraction"]                = relu(RF_constituents[i].getExtraVar("PhotonEnergyFraction"));
            varMap["j" + std::to_string(i + 1) + "_ElectronEnergyFraction"]              = relu(RF_constituents[i].getExtraVar("ElectronEnergyFraction"));
            varMap["j" + std::to_string(i + 1) + "_ChargedHadronMultiplicity"]           = relu(RF_constituents[i].getExtraVar("ChargedHadronMultiplicity"));
            varMap["j" + std::to_string(i + 1) + "_NeutralHadronMultiplicity"]           = relu(RF_constituents[i].getExtraVar("NeutralHadronMultiplicity"));
            varMap["j" + std::to_string(i + 1) + "_PhotonMultiplicity"]                  = relu(RF_constituents[i].getExtraVar("PhotonMultiplicity"));
            varMap["j" + std::to_string(i + 1) + "_ElectronMultiplicity"]                = relu(RF_constituents[i].getExtraVar("ElectronMultiplicity"));
            varMap["j" + std::to_string(i + 1) + "_MuonMultiplicity"]                    = relu(RF_constituents[i].getExtraVar("MuonMultiplicity"));
            varMap["j" + std::to_string(i + 1) + "_DeepCSVb"]                            = relu(RF_constituents[i].getExtraVar("DeepCSVb"));
            varMap["j" + std::to_string(i + 1) + "_DeepCSVc"]                            = relu(RF_constituents[i].getExtraVar("DeepCSVc"));
            varMap["j" + std::to_string(i + 1) + "_DeepCSVl"]                            = relu(RF_constituents[i].getExtraVar("DeepCSVl"));
            varMap["j" + std::to_string(i + 1) + "_DeepCSVbb"]                           = relu(RF_constituents[i].getExtraVar("DeepCSVbb"));
            varMap["j" + std::to_string(i + 1) + "_DeepCSVcc"]                           = relu(RF_constituents[i].getExtraVar("DeepCSVcc"));
            varMap["j" + std::to_string(i + 1) + "_CvsL"]                                = relu(RF_constituents[i].getExtraVar("CvsL"));
            varMap["j" + std::to_string(i + 1) + "_CvsB"]                                = relu(RF_constituents[i].getExtraVar("CvsB"));
            //varMap["j" + std::to_string(i + 1) + "_CombinedSvtx"]                        = relu(RF_constituents[i].getExtraVar("CombinedSvtx"));
            //varMap["j" + std::to_string(i + 1) + "_Svtx"]                                = relu(RF_constituents[i].getExtraVar("Svtx"));
            //varMap["j" + std::to_string(i + 1) + "_SoftM"]                               = relu(RF_constituents[i].getExtraVar("SoftM"));
            //varMap["j" + std::to_string(i + 1) + "_SoftE"]                               = relu(RF_constituents[i].getExtraVar("SoftE"));
            varMap["j" + std::to_string(i + 1) + "_JetProba"]                            = relu(RF_constituents[i].getExtraVar("JetProba"));
            varMap["j" + std::to_string(i + 1) + "_JetBprob"]                            = relu(RF_constituents[i].getExtraVar("JetBprob"));
            varMap["j" + std::to_string(i + 1) + "_recoJetsCharge"]                      = relu(RF_constituents[i].getExtraVar("recoJetsCharge"), -2.0);
            varMap["j" + std::to_string(i + 1) + "_CSVTrackJetPt"]                       = relu(RF_constituents[i].getExtraVar("CSVTrackJetPt"));
            varMap["j" + std::to_string(i + 1) + "_CSVVertexCategory"]                   = relu(RF_constituents[i].getExtraVar("CSVVertexCategory"));
            varMap["j" + std::to_string(i + 1) + "_CSVJetNSecondaryVertices"]            = relu(RF_constituents[i].getExtraVar("CSVJetNSecondaryVertices"));
            varMap["j" + std::to_string(i + 1) + "_CSVTrackSumJetEtRatio"]               = relu(RF_constituents[i].getExtraVar("CSVTrackSumJetEtRatio"));
            varMap["j" + std::to_string(i + 1) + "_CSVTrackSumJetDeltaR"]                = relu(RF_constituents[i].getExtraVar("CSVTrackSumJetDeltaR"));
            varMap["j" + std::to_string(i + 1) + "_CSVTrackSip2dValAboveCharm"]          = relu(RF_constituents[i].getExtraVar("CSVTrackSip2dValAboveCharm"), -0.5);
            varMap["j" + std::to_string(i + 1) + "_CSVTrackSip2dSigAboveCharm"]          = relu(RF_constituents[i].getExtraVar("CSVTrackSip2dSigAboveCharm"), -100.0);
            varMap["j" + std::to_string(i + 1) + "_CSVTrackSip3dValAboveCharm"]          = relu(RF_constituents[i].getExtraVar("CSVTrackSip3dValAboveCharm"), -0.98);
            varMap["j" + std::to_string(i + 1) + "_CSVTrackSip3dSigAboveCharm"]          = relu(RF_constituents[i].getExtraVar("CSVTrackSip3dSigAboveCharm"), -200.0);
            varMap["j" + std::to_string(i + 1) + "_CSVVertexMass"]                       = relu(RF_constituents[i].getExtraVar("CSVVertexMass"));
            varMap["j" + std::to_string(i + 1) + "_CSVVertexNTracks"]                    = relu(RF_constituents[i].getExtraVar("CSVVertexNTracks"));
            varMap["j" + std::to_string(i + 1) + "_CSVVertexEnergyRatio"]                = relu(RF_constituents[i].getExtraVar("CSVVertexEnergyRatio"));
            varMap["j" + std::to_string(i + 1) + "_CSVVertexJetDeltaR"]                  = relu(RF_constituents[i].getExtraVar("CSVVertexJetDeltaR"));
            varMap["j" + std::to_string(i + 1) + "_CSVFlightDistance2dVal"]              = relu(RF_constituents[i].getExtraVar("CSVFlightDistance2dVal"));
            varMap["j" + std::to_string(i + 1) + "_CSVFlightDistance2dSig"]              = relu(RF_constituents[i].getExtraVar("CSVFlightDistance2dSig"));
            varMap["j" + std::to_string(i + 1) + "_CSVFlightDistance3dVal"]              = relu(RF_constituents[i].getExtraVar("CSVFlightDistance3dVal"));
            varMap["j" + std::to_string(i + 1) + "_CSVFlightDistance3dSig"]              = relu(RF_constituents[i].getExtraVar("CSVFlightDistance3dSig"));
            varMap["j" + std::to_string(i + 1) + "_CTagVertexCategory"]                  = relu(RF_constituents[i].getExtraVar("CTagVertexCategory"));
            varMap["j" + std::to_string(i + 1) + "_CTagJetNSecondaryVertices"]           = relu(RF_constituents[i].getExtraVar("CTagJetNSecondaryVertices"));
            varMap["j" + std::to_string(i + 1) + "_CTagTrackSumJetEtRatio"]              = relu(RF_constituents[i].getExtraVar("CTagTrackSumJetEtRatio"));
            varMap["j" + std::to_string(i + 1) + "_CTagTrackSumJetDeltaR"]               = relu(RF_constituents[i].getExtraVar("CTagTrackSumJetDeltaR"));
            varMap["j" + std::to_string(i + 1) + "_CTagTrackSip2dSigAboveCharm"]         = relu(RF_constituents[i].getExtraVar("CTagTrackSip2dSigAboveCharm"), -100.0);
            varMap["j" + std::to_string(i + 1) + "_CTagTrackSip3dSigAboveCharm"]         = relu(RF_constituents[i].getExtraVar("CTagTrackSip3dSigAboveCharm"), -200.0);
            varMap["j" + std::to_string(i + 1) + "_CTagVertexMass"]                      = relu(RF_constituents[i].getExtraVar("CTagVertexMass"));
            varMap["j" + std::to_string(i + 1) + "_CTagVertexNTracks"]                   = relu(RF_constituents[i].getExtraVar("CTagVertexNTracks"));
            varMap["j" + std::to_string(i + 1) + "_CTagVertexEnergyRatio"]               = relu(RF_constituents[i].getExtraVar("CTagVertexEnergyRatio"));
            varMap["j" + std::to_string(i + 1) + "_CTagVertexJetDeltaR"]                 = relu(RF_constituents[i].getExtraVar("CTagVertexJetDeltaR"));
            varMap["j" + std::to_string(i + 1) + "_CTagFlightDistance2dSig"]             = relu(RF_constituents[i].getExtraVar("CTagFlightDistance2dSig"));
            varMap["j" + std::to_string(i + 1) + "_CTagFlightDistance3dSig"]             = relu(RF_constituents[i].getExtraVar("CTagFlightDistance3dSig"));
            varMap["j" + std::to_string(i + 1) + "_CTagMassVertexEnergyFraction"]        = relu(RF_constituents[i].getExtraVar("CTagMassVertexEnergyFraction"));
            varMap["j" + std::to_string(i + 1) + "_CTagVertexBoostOverSqrtJetPt"]        = relu(RF_constituents[i].getExtraVar("CTagVertexBoostOverSqrtJetPt"));
            varMap["j" + std::to_string(i + 1) + "_CTagVertexLeptonCategory"]            = relu(RF_constituents[i].getExtraVar("CTagVertexLeptonCategory"));
            

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
        return std::vector<std::string>({"cand_m", "cand_pt", "cand_p", "cand_dRMax", "cand_dThetaMax", "cand_dThetaMin", "j12_m", "j13_m", "j23_m", "j1_p", "j2_p", "j3_p", "dTheta12", "dTheta23", "dTheta13", "j1_CSV", "j2_CSV", "j3_CSV", "j1_QGL", "j2_QGL", "j3_QGL","j1_qgMult", "j1_qgPtD", "j1_qgAxis1", "j1_qgAxis2", "j1_recoJetsFlavor", "j1_recoJetsJecScaleRawToFull", "j1_recoJetschargedHadronEnergyFraction", "j1_recoJetschargedEmEnergyFraction", "j1_recoJetsneutralEmEnergyFraction", "j1_recoJetsmuonEnergyFraction", "j1_recoJetsHFHadronEnergyFraction", "j1_recoJetsHFEMEnergyFraction", "j1_recoJetsneutralEnergyFraction", "j1_PhotonEnergyFraction", "j1_ElectronEnergyFraction", "j1_ChargedHadronMultiplicity", "j1_NeutralHadronMultiplicity", "j1_PhotonMultiplicity", "j1_ElectronMultiplicity", "j1_MuonMultiplicity", "j1_DeepCSVb", "j1_DeepCSVc", "j1_DeepCSVl", "j1_DeepCSVbb", "j1_DeepCSVcc", "j1_CvsL", "j1_CvsB", /*"j1_CombinedSvtx", "j1_Svtx", "j1_SoftM", "j1_SoftE", */"j1_JetProba", "j1_JetBprob", "j1_recoJetsCharge", "j1_CSVTrackJetPt", "j1_CSVVertexCategory", "j1_CSVJetNSecondaryVertices", "j1_CSVTrackSumJetEtRatio", "j1_CSVTrackSumJetDeltaR", "j1_CSVTrackSip2dValAboveCharm", "j1_CSVTrackSip2dSigAboveCharm", "j1_CSVTrackSip3dValAboveCharm", "j1_CSVTrackSip3dSigAboveCharm", "j1_CSVVertexMass", "j1_CSVVertexNTracks", "j1_CSVVertexEnergyRatio", "j1_CSVVertexJetDeltaR", "j1_CSVFlightDistance2dVal", "j1_CSVFlightDistance2dSig", "j1_CSVFlightDistance3dVal", "j1_CSVFlightDistance3dSig", "j1_CTagVertexCategory", "j1_CTagJetNSecondaryVertices", "j1_CTagTrackSumJetEtRatio", "j1_CTagTrackSumJetDeltaR", "j1_CTagTrackSip2dSigAboveCharm", "j1_CTagTrackSip3dSigAboveCharm", "j1_CTagVertexMass", "j1_CTagVertexNTracks", "j1_CTagVertexEnergyRatio", "j1_CTagVertexJetDeltaR", "j1_CTagFlightDistance2dSig", "j1_CTagFlightDistance3dSig", "j1_CTagMassVertexEnergyFraction", "j1_CTagVertexBoostOverSqrtJetPt", "j1_CTagVertexLeptonCategory", "j2_qgMult", "j2_qgPtD", "j2_qgAxis1", "j2_qgAxis2", "j2_recoJetsFlavor", "j2_recoJetsJecScaleRawToFull", "j2_recoJetschargedHadronEnergyFraction", "j2_recoJetschargedEmEnergyFraction", "j2_recoJetsneutralEmEnergyFraction", "j2_recoJetsmuonEnergyFraction", "j2_recoJetsHFHadronEnergyFraction", "j2_recoJetsHFEMEnergyFraction", "j2_recoJetsneutralEnergyFraction", "j2_PhotonEnergyFraction", "j2_ElectronEnergyFraction", "j2_ChargedHadronMultiplicity", "j2_NeutralHadronMultiplicity", "j2_PhotonMultiplicity", "j2_ElectronMultiplicity", "j2_MuonMultiplicity", "j2_DeepCSVb", "j2_DeepCSVc", "j2_DeepCSVl", "j2_DeepCSVbb", "j2_DeepCSVcc", "j2_CvsL", "j2_CvsB", /*"j2_CombinedSvtx", "j2_Svtx", "j2_SoftM", "j2_SoftE",*/ "j2_JetProba", "j2_JetBprob", "j2_recoJetsCharge", "j2_CSVTrackJetPt", "j2_CSVVertexCategory", "j2_CSVJetNSecondaryVertices", "j2_CSVTrackSumJetEtRatio", "j2_CSVTrackSumJetDeltaR", "j2_CSVTrackSip2dValAboveCharm", "j2_CSVTrackSip2dSigAboveCharm", "j2_CSVTrackSip3dValAboveCharm", "j2_CSVTrackSip3dSigAboveCharm", "j2_CSVVertexMass", "j2_CSVVertexNTracks", "j2_CSVVertexEnergyRatio", "j2_CSVVertexJetDeltaR", "j2_CSVFlightDistance2dVal", "j2_CSVFlightDistance2dSig", "j2_CSVFlightDistance3dVal", "j2_CSVFlightDistance3dSig", "j2_CTagVertexCategory", "j2_CTagJetNSecondaryVertices", "j2_CTagTrackSumJetEtRatio", "j2_CTagTrackSumJetDeltaR", "j2_CTagTrackSip2dSigAboveCharm", "j2_CTagTrackSip3dSigAboveCharm", "j2_CTagVertexMass", "j2_CTagVertexNTracks", "j2_CTagVertexEnergyRatio", "j2_CTagVertexJetDeltaR", "j2_CTagFlightDistance2dSig", "j2_CTagFlightDistance3dSig", "j2_CTagMassVertexEnergyFraction", "j2_CTagVertexBoostOverSqrtJetPt", "j2_CTagVertexLeptonCategory", "j3_qgMult", "j3_qgPtD", "j3_qgAxis1", "j3_qgAxis2", "j3_recoJetsFlavor", "j3_recoJetsJecScaleRawToFull", "j3_recoJetschargedHadronEnergyFraction", "j3_recoJetschargedEmEnergyFraction", "j3_recoJetsneutralEmEnergyFraction", "j3_recoJetsmuonEnergyFraction", "j3_recoJetsHFHadronEnergyFraction", "j3_recoJetsHFEMEnergyFraction", "j3_recoJetsneutralEnergyFraction", "j3_PhotonEnergyFraction", "j3_ElectronEnergyFraction", "j3_ChargedHadronMultiplicity", "j3_NeutralHadronMultiplicity", "j3_PhotonMultiplicity", "j3_ElectronMultiplicity", "j3_MuonMultiplicity", "j3_DeepCSVb", "j3_DeepCSVc", "j3_DeepCSVl", "j3_DeepCSVbb", "j3_DeepCSVcc", "j3_CvsL", "j3_CvsB",/* "j3_CombinedSvtx", "j3_Svtx", "j3_SoftM", "j3_SoftE", */"j3_JetProba", "j3_JetBprob", "j3_recoJetsCharge", "j3_CSVTrackJetPt", "j3_CSVVertexCategory", "j3_CSVJetNSecondaryVertices", "j3_CSVTrackSumJetEtRatio", "j3_CSVTrackSumJetDeltaR", "j3_CSVTrackSip2dValAboveCharm", "j3_CSVTrackSip2dSigAboveCharm", "j3_CSVTrackSip3dValAboveCharm", "j3_CSVTrackSip3dSigAboveCharm", "j3_CSVVertexMass", "j3_CSVVertexNTracks", "j3_CSVVertexEnergyRatio", "j3_CSVVertexJetDeltaR", "j3_CSVFlightDistance2dVal", "j3_CSVFlightDistance2dSig", "j3_CSVFlightDistance3dVal", "j3_CSVFlightDistance3dSig", "j3_CTagVertexCategory", "j3_CTagJetNSecondaryVertices", "j3_CTagTrackSumJetEtRatio", "j3_CTagTrackSumJetDeltaR", "j3_CTagTrackSip2dSigAboveCharm", "j3_CTagTrackSip3dSigAboveCharm", "j3_CTagVertexMass", "j3_CTagVertexNTracks", "j3_CTagVertexEnergyRatio", "j3_CTagVertexJetDeltaR", "j3_CTagFlightDistance2dSig", "j3_CTagFlightDistance3dSig", "j3_CTagMassVertexEnergyFraction", "j3_CTagVertexBoostOverSqrtJetPt", "j3_CTagVertexLeptonCategory", "j1_p_top", "j1_theta_top", "j1_phi_top", "j2_p_top", "j2_theta_top", "j2_phi_top", "j3_p_top", "j3_theta_top", "j3_phi_top"});
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
