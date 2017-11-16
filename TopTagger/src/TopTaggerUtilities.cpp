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

    ConstAK8Inputs::ConstAK8Inputs(const std::vector<TLorentzVector>& jetsLVec, const std::vector<double>& tau1, const std::vector<double>& tau2, const std::vector<double>& tau3, const std::vector<double>& softDropMass, const std::vector<TLorentzVector>& subjetsLVec, const std::vector<double>& subjetsBtag, const std::vector<double>& subjetsMult, const std::vector<double>& subjetsPtD, const std::vector<double>& subjetsAxis1, const std::vector<double>& subjetsAxis2) : ConstGenInputs(), jetsLVec_(&jetsLVec), tau1_(&tau1), tau2_(&tau2), tau3_(&tau3), softDropMass_(&softDropMass), subjetsLVec_(&subjetsLVec), subjetsBtag_(&subjetsBtag), subjetsMult_(&subjetsMult), subjetsPtD_(&subjetsPtD), subjetsAxis1_(&subjetsAxis1), subjetsAxis2_(&subjetsAxis2) 
    {
        puppisd_corrGEN_ = nullptr;
        puppisd_corrRECO_cen_ = nullptr;
        puppisd_corrRECO_for_ = nullptr;
        vecSubjetsLVec_ = nullptr;
    }

    ConstAK8Inputs::ConstAK8Inputs(const std::vector<TLorentzVector>& jetsLVec, const std::vector<double>& tau1, const std::vector<double>& tau2, const std::vector<double>& tau3, const std::vector<double>& softDropMass, const std::vector<std::vector<TLorentzVector> >& vecSubJetsLVec) : ConstGenInputs(), jetsLVec_(&jetsLVec), tau1_(&tau1), tau2_(&tau2), tau3_(&tau3), softDropMass_(&softDropMass), vecSubjetsLVec_(&vecSubJetsLVec) 
    {
        puppisd_corrGEN_ = nullptr;
        puppisd_corrRECO_cen_ = nullptr;
        puppisd_corrRECO_for_ = nullptr;
        subjetsLVec_ = nullptr;
    }
    
    ConstAK8Inputs::ConstAK8Inputs(const std::vector<TLorentzVector>& jetsLVec, const std::vector<double>& tau1, const std::vector<double>& tau2, const std::vector<double>& tau3, const std::vector<double>& softDropMass, const std::vector<TLorentzVector>& subjetsLVec, const std::vector<TLorentzVector>& hadGenTops, const std::vector<std::vector<const TLorentzVector*>>& hadGenTopDaughters) : ConstGenInputs(hadGenTops, hadGenTopDaughters), jetsLVec_(&jetsLVec), tau1_(&tau1), tau2_(&tau2), tau3_(&tau3), softDropMass_(&softDropMass), subjetsLVec_(&subjetsLVec) 
    {
        puppisd_corrGEN_ = nullptr;
        puppisd_corrRECO_cen_ = nullptr;
        puppisd_corrRECO_for_ = nullptr;
        vecSubjetsLVec_ = nullptr;
    }

    ConstAK8Inputs::ConstAK8Inputs(const std::vector<TLorentzVector>& jetsLVec, const std::vector<double>& tau1, const std::vector<double>& tau2, const std::vector<double>& tau3, const std::vector<double>& softDropMass, const std::vector<std::vector<TLorentzVector> >& vecSubJetsLVec, const std::vector<TLorentzVector>& hadGenTops, const std::vector<std::vector<const TLorentzVector*>>& hadGenTopDaughters) : ConstGenInputs(hadGenTops, hadGenTopDaughters), jetsLVec_(&jetsLVec), tau1_(&tau1), tau2_(&tau2), tau3_(&tau3), softDropMass_(&softDropMass), vecSubjetsLVec_(&vecSubJetsLVec) 
    {
        puppisd_corrGEN_ = nullptr;
        puppisd_corrRECO_cen_ = nullptr;
        puppisd_corrRECO_for_ = nullptr;
        subjetsLVec_ = nullptr;
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
            THROW_TTEXCEPTION("Unequal AK8 vector size!!!!!!!\n");
        }

        //Safety check that jet and b-tag vectors are the same length
        if(subjetsLVec_->size() != subjetsBtag_->size() || subjetsLVec_->size() != subjetsMult_->size() || subjetsLVec_->size() != subjetsPtD_->size() || subjetsLVec_->size() != subjetsAxis1_->size() || subjetsLVec_->size() != subjetsAxis2_->size())
        {
            std::cout << subjetsLVec_->size() << "\t" << subjetsBtag_->size() << "\t" << subjetsMult_->size() << "\t" << subjetsPtD_->size() << "\t" << subjetsAxis1_->size() << "\t" << subjetsAxis2_->size() << std::endl;
            THROW_TTEXCEPTION("Unequal subjet vector size!!!!!!!\n");
        }

        //Construct constituents in place in the vector
        for(unsigned int iJet = 0; iJet < jetsLVec_->size(); ++iJet)
        {
            // For each tagged top/W, find the corresponding subjets
            std::vector<Constituent> subjets;
            if(vecSubjetsLVec_ != nullptr)
            {
                for(auto& subjetLV : (*vecSubjetsLVec_)[iJet])
                {
                    subjets.push_back(Constituent(subjetLV, AK8SUBJET));
                }
            } 
            else if (subjetsLVec_ != nullptr) 
            {
                // Calculate matching subjets if a single list was given 
                for(int iSJ = 0; iSJ < subjetsLVec_->size(); ++iSJ)
                {
                    double myDR = ROOT::Math::VectorUtil::DeltaR((*jetsLVec_)[iJet], (*subjetsLVec_)[iSJ]);
                    if (myDR < 0.8)
                    {
                        subjets.emplace_back((*subjetsLVec_)[iSJ], AK8SUBJET);
                        subjets.back().setBTag((*subjetsBtag_)[iSJ]);
                        subjets.back().setExtraVar("mult", (*subjetsMult_)[iSJ]);
                        subjets.back().setExtraVar("ptD", (*subjetsPtD_)[iSJ]);
                        subjets.back().setExtraVar("axis1", (*subjetsAxis1_)[iSJ]);
                        subjets.back().setExtraVar("axis2", (*subjetsAxis2_)[iSJ]);
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
                            TLorentzVector diff_LV = (*jetsLVec_)[iJet] - subjets[j].p() - subjets[k].p();
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
                            double dR = ROOT::Math::VectorUtil::DeltaR(subjet.p(), *genDaughter);
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

        //AK8 test variables
        if(topCand.getNConstituents() == 1)
        {
            const auto& constituent = *topCand.getConstituents()[0];
            varMap["ak8_sdmass"] = constituent.getSoftDropMass();
            varMap["ak8_tau21"] =  constituent.getTau1() > 0 ? constituent.getTau2()/constituent.getTau1() : 1e9;
            varMap["ak8_tau32"] =  constituent.getTau2() > 0 ? constituent.getTau3()/constituent.getTau2() : 1e9;

            const auto* sj1 = &constituent.getSubjets()[0];
            const auto* sj2 = &constituent.getSubjets()[0];
            double fj_deltaR = ROOT::Math::VectorUtil::DeltaR(sj1->p(), sj2->p());
            varMap["ak8_ptDR"] =       fj_deltaR*constituent.p().Pt();
            varMap["ak8_rel_ptdiff"] = fabs(sj1->p().Pt() - sj2->p().Pt()) / constituent.p().Pt();
            if(sj1->getBTagDisc() < sj2->getBTagDisc()) std::swap(sj1,sj2);
            varMap["ak8_csv1_mass"] =  sj1->p().M();
            varMap["ak8_csv1_csv"] =   (sj1->getBTagDisc() > 0 ? sj1->getBTagDisc() : 0.);
            varMap["ak8_csv1_ptD"] =   sj1->getExtraVar("ptD");
            varMap["ak8_csv1_axis1"] = sj1->getExtraVar("axis1");
            varMap["ak8_csv1_mult"] =  sj1->getExtraVar("mult");
            varMap["ak8_csv2_mass"] =  sj2->p().M();
            varMap["ak8_csv2_ptD"] =   sj2->getExtraVar("ptD");
            varMap["ak8_csv2_axis1"] = sj2->getExtraVar("axis1");
            varMap["ak8_csv2_mult"] =  sj2->getExtraVar("mult");
        }
        else
        {
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

            varMap["j" + std::to_string(i + 1) + "_recoJetsJecScaleRawToFull"]           = relu(RF_constituents[i].getExtraVar("recoJetsJecScaleRawToFull"));
            varMap["j" + std::to_string(i + 1) + "_qgLikelihood"]                        = relu(RF_constituents[i].getExtraVar("qgLikelihood"));
            varMap["j" + std::to_string(i + 1) + "_qgPtD"]                               = relu(RF_constituents[i].getExtraVar("qgPtD"));
            varMap["j" + std::to_string(i + 1) + "_qgAxis1"]                             = relu(RF_constituents[i].getExtraVar("qgAxis1"));
            varMap["j" + std::to_string(i + 1) + "_qgAxis2"]                             = relu(RF_constituents[i].getExtraVar("qgAxis2"));
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
            varMap["j" + std::to_string(i + 1) + "_DeepFlavorb"]                         = relu(RF_constituents[i].getExtraVar("DeepFlavorb"));
            varMap["j" + std::to_string(i + 1) + "_DeepFlavorbb"]                        = relu(RF_constituents[i].getExtraVar("DeepFlavorbb"));
            varMap["j" + std::to_string(i + 1) + "_DeepFlavorlepb"]                      = relu(RF_constituents[i].getExtraVar("DeepFlavorlepb"));
            varMap["j" + std::to_string(i + 1) + "_DeepFlavorc"]                         = relu(RF_constituents[i].getExtraVar("DeepFlavorc"));
            varMap["j" + std::to_string(i + 1) + "_DeepFlavoruds"]                       = relu(RF_constituents[i].getExtraVar("DeepFlavoruds"));
            varMap["j" + std::to_string(i + 1) + "_DeepFlavorg"]                         = relu(RF_constituents[i].getExtraVar("DeepFlavorg"));
            varMap["j" + std::to_string(i + 1) + "_CvsL"]                                = relu(RF_constituents[i].getExtraVar("CvsL"));
            varMap["j" + std::to_string(i + 1) + "_CvsB"]                                = relu(RF_constituents[i].getExtraVar("CvsB"));
            varMap["j" + std::to_string(i + 1) + "_CombinedSvtx"]                        = relu(RF_constituents[i].getExtraVar("CombinedSvtx"));
            varMap["j" + std::to_string(i + 1) + "_JetProba"]                            = relu(RF_constituents[i].getExtraVar("JetProba"));
            varMap["j" + std::to_string(i + 1) + "_JetBprob"]                            = relu(RF_constituents[i].getExtraVar("JetBprob"));
            varMap["j" + std::to_string(i + 1) + "_recoJetsBtag"]                        = relu(RF_constituents[i].getExtraVar("recoJetsBtag"));
            varMap["j" + std::to_string(i + 1) + "_recoJetsCharge"]                      = relu(RF_constituents[i].getExtraVar("recoJetsCharge"), -2);
            varMap["j" + std::to_string(i + 1) + "_qgMult"]                              = relu(RF_constituents[i].getExtraVar("qgMult"));
            

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
        }

        return varMap;
    }

    std::vector<std::string> getMVAVars()
    {
        return std::vector<std::string>({"genTopPt",
                    "MET",
                    "cand_dRMax",
                    "cand_dThetaMin",
                    "cand_dThetaMax",
                    "cand_eta",
                    "cand_m",
                    "cand_phi",
                    "cand_pt",
                    "cand_p",
                    "dR12_lab",
                    "dR13_lab",
                    "dR1_23_lab",
                    "dR23_lab",
                    "dR2_13_lab",
                    "dR3_12_lab",
                    "dRPtTop",
                    "dRPtW",
                    "dTheta12",
                    "dTheta13",
                    "dTheta23",
                    "j12_m",
                    "j12_m_lab",
                    "j13_m",
                    "j13_m_lab",

                    "j1_recoJetsJecScaleRawToFull",
                    "j1_qgLikelihood",
                    "j1_qgPtD",
                    "j1_qgAxis1",
                    "j1_qgAxis2",
                    "j1_recoJetschargedHadronEnergyFraction",
                    "j1_recoJetschargedEmEnergyFraction",
                    "j1_recoJetsneutralEmEnergyFraction",
                    "j1_recoJetsmuonEnergyFraction",
                    "j1_recoJetsHFHadronEnergyFraction",
                    "j1_recoJetsHFEMEnergyFraction",
                    "j1_recoJetsneutralEnergyFraction",
                    "j1_PhotonEnergyFraction",
                    "j1_ElectronEnergyFraction",
                    "j1_ChargedHadronMultiplicity",
                    "j1_NeutralHadronMultiplicity",
                    "j1_PhotonMultiplicity",
                    "j1_ElectronMultiplicity",
                    "j1_MuonMultiplicity",
                    "j1_DeepCSVb",
                    "j1_DeepCSVc",
                    "j1_DeepCSVl",
                    "j1_DeepCSVbb",
                    "j1_DeepCSVcc",
                    "j1_DeepFlavorb",
                    "j1_DeepFlavorbb",
                    "j1_DeepFlavorlepb",
                    "j1_DeepFlavorc",
                    "j1_DeepFlavoruds",
                    "j1_DeepFlavorg",
                    "j1_CvsL",
                    "j1_CvsB",
                    "j1_CombinedSvtx",
                    "j1_JetProba",
                    "j1_JetBprob",
                    "j1_recoJetsBtag",
                    "j1_recoJetsCharge",
                    "j1_qgMult",
                    "j1_CSV",
                    "j1_CSV_lab",
                    "j1_QGL",
                    "j1_QGL_lab",
                    "j1_eta_lab",
                    "j1_m",
                    "j1_m_lab",
                    "j1_p",
                    "j1_phi_lab",
                    "j1_pt_lab",
                    "j1_qgAxis1_lab",
                    "j1_qgAxis2_lab",
                    "j1_qgMult_lab",
                    "j1_qgPtD_lab",
                    "j2_recoJetsJecScaleRawToFull",
                    "j2_qgLikelihood",
                    "j2_qgPtD",
                    "j2_qgAxis1",
                    "j2_qgAxis2",
                    "j2_recoJetschargedHadronEnergyFraction",
                    "j2_recoJetschargedEmEnergyFraction",
                    "j2_recoJetsneutralEmEnergyFraction",
                    "j2_recoJetsmuonEnergyFraction",
                    "j2_recoJetsHFHadronEnergyFraction",
                    "j2_recoJetsHFEMEnergyFraction",
                    "j2_recoJetsneutralEnergyFraction",
                    "j2_PhotonEnergyFraction",
                    "j2_ElectronEnergyFraction",
                    "j2_ChargedHadronMultiplicity",
                    "j2_NeutralHadronMultiplicity",
                    "j2_PhotonMultiplicity",
                    "j2_ElectronMultiplicity",
                    "j2_MuonMultiplicity",
                    "j2_DeepCSVb",
                    "j2_DeepCSVc",
                    "j2_DeepCSVl",
                    "j2_DeepCSVbb",
                    "j2_DeepCSVcc",
                    "j2_DeepFlavorb",
                    "j2_DeepFlavorbb",
                    "j2_DeepFlavorlepb",
                    "j2_DeepFlavorc",
                    "j2_DeepFlavoruds",
                    "j2_DeepFlavorg",
                    "j2_CvsL",
                    "j2_CvsB",
                    "j2_CombinedSvtx",
                    "j2_JetProba",
                    "j2_JetBprob",
                    "j2_recoJetsBtag",
                    "j2_recoJetsCharge",
                    "j2_qgMult",
                    "j2_CSV",
                    "j2_CSV_lab",
                    "j2_QGL",
                    "j2_QGL_lab",
                    "j2_eta_lab",
                    "j2_m",
                    "j2_m_lab",
                    "j2_p",
                    "j2_phi_lab",
                    "j2_pt_lab",
                    "j2_qgAxis1_lab",
                    "j2_qgAxis2_lab",
                    "j2_qgMult_lab",
                    "j2_qgPtD_lab",

                    "j3_recoJetsJecScaleRawToFull",
                    "j3_qgLikelihood",
                    "j3_qgPtD",
                    "j3_qgAxis1",
                    "j3_qgAxis2",
                    "j3_recoJetschargedHadronEnergyFraction",
                    "j3_recoJetschargedEmEnergyFraction",
                    "j3_recoJetsneutralEmEnergyFraction",
                    "j3_recoJetsmuonEnergyFraction",
                    "j3_recoJetsHFHadronEnergyFraction",
                    "j3_recoJetsHFEMEnergyFraction",
                    "j3_recoJetsneutralEnergyFraction",
                    "j3_PhotonEnergyFraction",
                    "j3_ElectronEnergyFraction",
                    "j3_ChargedHadronMultiplicity",
                    "j3_NeutralHadronMultiplicity",
                    "j3_PhotonMultiplicity",
                    "j3_ElectronMultiplicity",
                    "j3_MuonMultiplicity",
                    "j3_DeepCSVb",
                    "j3_DeepCSVc",
                    "j3_DeepCSVl",
                    "j3_DeepCSVbb",
                    "j3_DeepCSVcc",
                    "j3_DeepFlavorb",
                    "j3_DeepFlavorbb",
                    "j3_DeepFlavorlepb",
                    "j3_DeepFlavorc",
                    "j3_DeepFlavoruds",
                    "j3_DeepFlavorg",
                    "j3_CvsL",
                    "j3_CvsB",
                    "j3_CombinedSvtx",
                    "j3_JetProba",
                    "j3_JetBprob",
                    "j3_recoJetsBtag",
                    "j3_recoJetsCharge",
                    "j3_qgMult",
                    "j3_CSV",
                    "j3_CSV_lab",
                    "j3_QGL",
                    "j3_QGL_lab",
                    "j3_eta_lab",
                    "j3_m",
                    "j3_m_lab",
                    "j3_p",
                    "j3_phi_lab",
                    "j3_pt_lab",
                    "j3_qgAxis1_lab",
                    "j3_qgAxis2_lab",
                    "j3_qgMult_lab",
                    "j3_qgPtD_lab",

                    "j23_m",
                    "j23_m_lab",
                    "sd_n2",
                    "j1_p_top", "j1_theta_top", "j1_phi_top", "j2_p_top", "j2_theta_top", "j2_phi_top", "j3_p_top", "j3_theta_top", "j3_phi_top"});

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
