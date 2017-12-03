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

    ConstAK8Inputs::ConstAK8Inputs(const std::vector<TLorentzVector>& jetsLVec, const std::vector<double>& tau1, const std::vector<double>& tau2, const std::vector<double>& tau3, const std::vector<double>& softDropMass, const std::vector<TLorentzVector>& subjetsLVec) : ConstGenInputs(), jetsLVec_(&jetsLVec), tau1_(&tau1), tau2_(&tau2), tau3_(&tau3), softDropMass_(&softDropMass), subjetsLVec_(&subjetsLVec)
    {
        subjetsBtag_ = nullptr;
        subjetsMult_ = nullptr;
        subjetsPtD_ = nullptr;
        subjetsAxis1_ = nullptr;
        subjetsAxis2_ = nullptr;
        puppisd_corrGEN_ = nullptr;
        puppisd_corrRECO_cen_ = nullptr;
        puppisd_corrRECO_for_ = nullptr;
        vecSubjetsLVec_ = nullptr;
    }

    ConstAK8Inputs::ConstAK8Inputs(const std::vector<TLorentzVector>& jetsLVec, const std::vector<double>& tau1, const std::vector<double>& tau2, const std::vector<double>& tau3, const std::vector<double>& softDropMass, const std::vector<TLorentzVector>& subjetsLVec, const std::vector<double>& subjetsBtag, const std::vector<double>& subjetsMult, const std::vector<double>& subjetsPtD, const std::vector<double>& subjetsAxis1, const std::vector<double>& subjetsAxis2) : ConstGenInputs(), jetsLVec_(&jetsLVec), tau1_(&tau1), tau2_(&tau2), tau3_(&tau3), softDropMass_(&softDropMass), subjetsLVec_(&subjetsLVec), subjetsBtag_(&subjetsBtag), subjetsMult_(&subjetsMult), subjetsPtD_(&subjetsPtD), subjetsAxis1_(&subjetsAxis1), subjetsAxis2_(&subjetsAxis2) 
    {
        puppisd_corrGEN_ = nullptr;
        puppisd_corrRECO_cen_ = nullptr;
        puppisd_corrRECO_for_ = nullptr;
        vecSubjetsLVec_ = nullptr;
    }

    ConstAK8Inputs::ConstAK8Inputs(const std::vector<TLorentzVector>& jetsLVec, const std::vector<double>& tau1, const std::vector<double>& tau2, const std::vector<double>& tau3, const std::vector<double>& softDropMass, const std::vector<std::vector<TLorentzVector> >& vecSubJetsLVec) : ConstGenInputs(), jetsLVec_(&jetsLVec), tau1_(&tau1), tau2_(&tau2), tau3_(&tau3), softDropMass_(&softDropMass), vecSubjetsLVec_(&vecSubJetsLVec) 
    {
        subjetsBtag_ = nullptr;
        subjetsMult_ = nullptr;
        subjetsPtD_ = nullptr;
        subjetsAxis1_ = nullptr;
        subjetsAxis2_ = nullptr;
        puppisd_corrGEN_ = nullptr;
        puppisd_corrRECO_cen_ = nullptr;
        puppisd_corrRECO_for_ = nullptr;
        subjetsLVec_ = nullptr;
    }
    
    ConstAK8Inputs::ConstAK8Inputs(const std::vector<TLorentzVector>& jetsLVec, const std::vector<double>& tau1, const std::vector<double>& tau2, const std::vector<double>& tau3, const std::vector<double>& softDropMass, const std::vector<TLorentzVector>& subjetsLVec, const std::vector<TLorentzVector>& hadGenTops, const std::vector<std::vector<const TLorentzVector*>>& hadGenTopDaughters) : ConstGenInputs(hadGenTops, hadGenTopDaughters), jetsLVec_(&jetsLVec), tau1_(&tau1), tau2_(&tau2), tau3_(&tau3), softDropMass_(&softDropMass), subjetsLVec_(&subjetsLVec) 
    {
        subjetsBtag_ = nullptr;
        subjetsMult_ = nullptr;
        subjetsPtD_ = nullptr;
        subjetsAxis1_ = nullptr;
        subjetsAxis2_ = nullptr;
        puppisd_corrGEN_ = nullptr;
        puppisd_corrRECO_cen_ = nullptr;
        puppisd_corrRECO_for_ = nullptr;
        vecSubjetsLVec_ = nullptr;
    }

    ConstAK8Inputs::ConstAK8Inputs(const std::vector<TLorentzVector>& jetsLVec, const std::vector<double>& tau1, const std::vector<double>& tau2, const std::vector<double>& tau3, const std::vector<double>& softDropMass, const std::vector<TLorentzVector>& subjetsLVec, const std::vector<double>& subjetsBtag, const std::vector<double>& subjetsMult, const std::vector<double>& subjetsPtD, const std::vector<double>& subjetsAxis1, const std::vector<double>& subjetsAxis2, const std::vector<TLorentzVector>& hadGenTops, const std::vector<std::vector<const TLorentzVector*>>& hadGenTopDaughters) : ConstGenInputs(hadGenTops, hadGenTopDaughters), jetsLVec_(&jetsLVec), tau1_(&tau1), tau2_(&tau2), tau3_(&tau3), softDropMass_(&softDropMass), subjetsLVec_(&subjetsLVec), subjetsBtag_(&subjetsBtag), subjetsMult_(&subjetsMult), subjetsPtD_(&subjetsPtD), subjetsAxis1_(&subjetsAxis1), subjetsAxis2_(&subjetsAxis2)  
    {
        puppisd_corrGEN_ = nullptr;
        puppisd_corrRECO_cen_ = nullptr;
        puppisd_corrRECO_for_ = nullptr;
        vecSubjetsLVec_ = nullptr;
    }

    ConstAK8Inputs::ConstAK8Inputs(const std::vector<TLorentzVector>& jetsLVec, const std::vector<double>& tau1, const std::vector<double>& tau2, const std::vector<double>& tau3, const std::vector<double>& softDropMass, const std::vector<std::vector<TLorentzVector> >& vecSubJetsLVec, const std::vector<TLorentzVector>& hadGenTops, const std::vector<std::vector<const TLorentzVector*>>& hadGenTopDaughters) : ConstGenInputs(hadGenTops, hadGenTopDaughters), jetsLVec_(&jetsLVec), tau1_(&tau1), tau2_(&tau2), tau3_(&tau3), softDropMass_(&softDropMass), vecSubjetsLVec_(&vecSubJetsLVec) 
    {
        subjetsBtag_ = nullptr;
        subjetsMult_ = nullptr;
        subjetsPtD_ = nullptr;
        subjetsAxis1_ = nullptr;
        subjetsAxis2_ = nullptr;
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
        if((subjetsBtag_ &&  subjetsLVec_->size() != subjetsBtag_->size()) || 
           (subjetsMult_ &&  subjetsLVec_->size() != subjetsMult_->size()) || 
           (subjetsPtD_ &&   subjetsLVec_->size() != subjetsPtD_->size()) || 
           (subjetsAxis1_ && subjetsLVec_->size() != subjetsAxis1_->size()) || 
           (subjetsAxis2_ && subjetsLVec_->size() != subjetsAxis2_->size()))
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
                        if(subjetsBtag_)  subjets.back().setBTag((*subjetsBtag_)[iSJ]);
                        if(subjetsMult_)  subjets.back().setExtraVar("mult", (*subjetsMult_)[iSJ]);
                        if(subjetsPtD_)   subjets.back().setExtraVar("ptD", (*subjetsPtD_)[iSJ]);
                        if(subjetsAxis1_) subjets.back().setExtraVar("axis1", (*subjetsAxis1_)[iSJ]);
                        if(subjetsAxis2_) subjets.back().setExtraVar("axis2", (*subjetsAxis2_)[iSJ]);
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
            const auto* sj2 = &constituent.getSubjets()[1];
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
        if(topCand.getNConstituents() == 2)
        {
            const auto* fatjet = topCand.getConstituents()[0];
            if(fatjet->getType() != AK8JET) fatjet = topCand.getConstituents()[1];
            varMap["var_fj_sdmass"]   = fatjet->getSoftDropMass();
            varMap["var_fj_tau21"]    = fatjet->getTau1() > 0 ? fatjet->getTau2()/fatjet->getTau1() : 1e9;
            // filling subjet variables
            const auto *sj1 = &fatjet->getSubjets()[0];
            const auto *sj2 = &fatjet->getSubjets()[1];
            double fj_deltaR =  ROOT::Math::VectorUtil::DeltaR(sj1->p(), sj2->p());
            varMap["var_fj_ptDR"]     = fj_deltaR*fatjet->p().Pt();
            varMap["var_fj_rel_ptdiff"]= std::abs(sj1->p().Pt()-sj2->p().Pt())/fatjet->p().Pt();
            varMap["var_sj1_ptD"]     = sj1->getExtraVar("ptD");
            varMap["var_sj1_axis1"]   = sj1->getExtraVar("axis1");
            varMap["var_sj1_mult"]    = sj1->getExtraVar("mult");
            varMap["var_sj2_ptD"]     = sj2->getExtraVar("ptD");
            varMap["var_sj2_axis1"]   = sj2->getExtraVar("axis1");
            varMap["var_sj2_mult"]    = sj2->getExtraVar("mult");
            varMap["var_sjmax_csv"]   = std::max(std::max(sj1->getBTagDisc(),sj2->getBTagDisc()),0.0);
            double var_sd_0 = sj2->p().Pt()/(sj1->p().Pt()+sj2->p().Pt());
            varMap["var_sd_n2"]       = var_sd_0/std::pow(fj_deltaR,-2);
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

            //varMap["j" + std::to_string(i + 1) + "_recoJetsJecScaleRawToFull"]           = relu(RF_constituents[i].getExtraVar("recoJetsJecScaleRawToFull"));
            //varMap["j" + std::to_string(i + 1) + "_qgLikelihood"]                        = relu(RF_constituents[i].getExtraVar("qgLikelihood"));
            //varMap["j" + std::to_string(i + 1) + "_qgPtD"]                               = relu(RF_constituents[i].getExtraVar("qgPtD"));
            //varMap["j" + std::to_string(i + 1) + "_qgAxis1"]                             = relu(RF_constituents[i].getExtraVar("qgAxis1"));
            //varMap["j" + std::to_string(i + 1) + "_qgAxis2"]                             = relu(RF_constituents[i].getExtraVar("qgAxis2"));
            //varMap["j" + std::to_string(i + 1) + "_recoJetschargedHadronEnergyFraction"] = relu(RF_constituents[i].getExtraVar("recoJetschargedHadronEnergyFraction"));
            //varMap["j" + std::to_string(i + 1) + "_recoJetschargedEmEnergyFraction"]     = relu(RF_constituents[i].getExtraVar("recoJetschargedEmEnergyFraction"));
            //varMap["j" + std::to_string(i + 1) + "_recoJetsneutralEmEnergyFraction"]     = relu(RF_constituents[i].getExtraVar("recoJetsneutralEmEnergyFraction"));
            //varMap["j" + std::to_string(i + 1) + "_recoJetsmuonEnergyFraction"]          = relu(RF_constituents[i].getExtraVar("recoJetsmuonEnergyFraction"));
            //varMap["j" + std::to_string(i + 1) + "_recoJetsHFHadronEnergyFraction"]      = relu(RF_constituents[i].getExtraVar("recoJetsHFHadronEnergyFraction"));
            //varMap["j" + std::to_string(i + 1) + "_recoJetsHFEMEnergyFraction"]          = relu(RF_constituents[i].getExtraVar("recoJetsHFEMEnergyFraction"));
            //varMap["j" + std::to_string(i + 1) + "_recoJetsneutralEnergyFraction"]       = relu(RF_constituents[i].getExtraVar("recoJetsneutralEnergyFraction"));
            //varMap["j" + std::to_string(i + 1) + "_PhotonEnergyFraction"]                = relu(RF_constituents[i].getExtraVar("PhotonEnergyFraction"));
            //varMap["j" + std::to_string(i + 1) + "_ElectronEnergyFraction"]              = relu(RF_constituents[i].getExtraVar("ElectronEnergyFraction"));
            //varMap["j" + std::to_string(i + 1) + "_ChargedHadronMultiplicity"]           = relu(RF_constituents[i].getExtraVar("ChargedHadronMultiplicity"));
            //varMap["j" + std::to_string(i + 1) + "_NeutralHadronMultiplicity"]           = relu(RF_constituents[i].getExtraVar("NeutralHadronMultiplicity"));
            //varMap["j" + std::to_string(i + 1) + "_PhotonMultiplicity"]                  = relu(RF_constituents[i].getExtraVar("PhotonMultiplicity"));
            //varMap["j" + std::to_string(i + 1) + "_ElectronMultiplicity"]                = relu(RF_constituents[i].getExtraVar("ElectronMultiplicity"));
            //varMap["j" + std::to_string(i + 1) + "_MuonMultiplicity"]                    = relu(RF_constituents[i].getExtraVar("MuonMultiplicity"));
            //varMap["j" + std::to_string(i + 1) + "_DeepCSVb"]                            = relu(RF_constituents[i].getExtraVar("DeepCSVb"));
            //varMap["j" + std::to_string(i + 1) + "_DeepCSVc"]                            = relu(RF_constituents[i].getExtraVar("DeepCSVc"));
            //varMap["j" + std::to_string(i + 1) + "_DeepCSVl"]                            = relu(RF_constituents[i].getExtraVar("DeepCSVl"));
            //varMap["j" + std::to_string(i + 1) + "_DeepCSVbb"]                           = relu(RF_constituents[i].getExtraVar("DeepCSVbb"));
            //varMap["j" + std::to_string(i + 1) + "_DeepCSVcc"]                           = relu(RF_constituents[i].getExtraVar("DeepCSVcc"));
            //varMap["j" + std::to_string(i + 1) + "_DeepFlavorb"]                         = relu(RF_constituents[i].getExtraVar("DeepFlavorb"));
            //varMap["j" + std::to_string(i + 1) + "_DeepFlavorbb"]                        = relu(RF_constituents[i].getExtraVar("DeepFlavorbb"));
            //varMap["j" + std::to_string(i + 1) + "_DeepFlavorlepb"]                      = relu(RF_constituents[i].getExtraVar("DeepFlavorlepb"));
            //varMap["j" + std::to_string(i + 1) + "_DeepFlavorc"]                         = relu(RF_constituents[i].getExtraVar("DeepFlavorc"));
            //varMap["j" + std::to_string(i + 1) + "_DeepFlavoruds"]                       = relu(RF_constituents[i].getExtraVar("DeepFlavoruds"));
            //varMap["j" + std::to_string(i + 1) + "_DeepFlavorg"]                         = relu(RF_constituents[i].getExtraVar("DeepFlavorg"));
            //varMap["j" + std::to_string(i + 1) + "_CvsL"]                                = relu(RF_constituents[i].getExtraVar("CvsL"));
            //varMap["j" + std::to_string(i + 1) + "_CvsB"]                                = relu(RF_constituents[i].getExtraVar("CvsB"));
            //varMap["j" + std::to_string(i + 1) + "_CombinedSvtx"]                        = relu(RF_constituents[i].getExtraVar("CombinedSvtx"));
            //varMap["j" + std::to_string(i + 1) + "_JetProba"]                            = relu(RF_constituents[i].getExtraVar("JetProba"));
            //varMap["j" + std::to_string(i + 1) + "_JetBprob"]                            = relu(RF_constituents[i].getExtraVar("JetBprob"));
            //varMap["j" + std::to_string(i + 1) + "_recoJetsBtag"]                        = relu(RF_constituents[i].getExtraVar("recoJetsBtag"));
            //varMap["j" + std::to_string(i + 1) + "_recoJetsCharge"]                      = relu(RF_constituents[i].getExtraVar("recoJetsCharge"), -2);
            //varMap["j" + std::to_string(i + 1) + "_qgMult"]                              = relu(RF_constituents[i].getExtraVar("qgMult"));
            

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

    BDTMonojetInputCalculator::BDTMonojetInputCalculator()
    {
        ak8_sdmass_ = ak8_tau21_ = ak8_tau32_ = ak8_ptDR_ = ak8_rel_ptdiff_ = ak8_csv1_mass_ = ak8_csv1_csv_ = ak8_csv1_ptD_ = ak8_csv1_axis1_ = ak8_csv1_mult_ = ak8_csv2_mass_ = ak8_csv2_ptD_ = ak8_csv2_axis1_ = ak8_csv2_mult_ = nullptr;
    }

    void BDTMonojetInputCalculator::mapVars(const std::vector<std::string>& vars, float* data)
    {
        for(int i = 0; i < vars.size(); ++i)
        {
            if(     vars[i].compare("ak8_sdmass") == 0)      ak8_sdmass_ = data + i;
            else if(vars[i].compare("ak8_tau21") == 0)       ak8_tau21_ = data + i;
            else if(vars[i].compare("ak8_tau32") == 0)       ak8_tau32_ = data + i;
            else if(vars[i].compare("ak8_ptDR") == 0)        ak8_ptDR_ = data + i;
            else if(vars[i].compare("ak8_rel_ptdiff") == 0)  ak8_rel_ptdiff_ = data + i;
            else if(vars[i].compare("ak8_csv1_mass") == 0)   ak8_csv1_mass_ = data + i;
            else if(vars[i].compare("ak8_csv1_csv") == 0)    ak8_csv1_csv_ = data + i;
            else if(vars[i].compare("ak8_csv1_ptD") == 0)    ak8_csv1_ptD_ = data + i;
            else if(vars[i].compare("ak8_csv1_axis1") == 0)  ak8_csv1_axis1_ = data + i;
            else if(vars[i].compare("ak8_csv1_mult") == 0)   ak8_csv1_mult_ = data + i;
            else if(vars[i].compare("ak8_csv2_mass") == 0)   ak8_csv2_mass_ = data + i;
            else if(vars[i].compare("ak8_csv2_ptD") == 0)    ak8_csv2_ptD_ = data + i;
            else if(vars[i].compare("ak8_csv2_axis1") == 0)  ak8_csv2_axis1_ = data + i;
            else if(vars[i].compare("ak8_csv2_mult") == 0)   ak8_csv2_mult_ = data + i;
        }
    }
        
    bool BDTMonojetInputCalculator::calculateVars(const TopObject& topCand)
    {
        if(topCand.getNConstituents() == 1 
           && topCand.getConstituents()[0]->getType() == AK8JET
           && topCand.getConstituents()[0]->getSubjets().size() == 2)
        {
            const auto& constituent = *topCand.getConstituents()[0];
            if(ak8_sdmass_)     *ak8_sdmass_ = constituent.getSoftDropMass();
            if(ak8_tau21_)      *ak8_tau21_ =  constituent.getTau1() > 0 ? constituent.getTau2()/constituent.getTau1() : 1e9;
            if(ak8_tau32_)      *ak8_tau32_ =  constituent.getTau2() > 0 ? constituent.getTau3()/constituent.getTau2() : 1e9;

            const auto* sj1 = &constituent.getSubjets()[0];
            const auto* sj2 = &constituent.getSubjets()[1];
            double fj_deltaR = ROOT::Math::VectorUtil::DeltaR(sj1->p(), sj2->p());
            if(ak8_ptDR_)       *ak8_ptDR_ =       fj_deltaR*constituent.p().Pt();
            if(ak8_rel_ptdiff_) *ak8_rel_ptdiff_ = fabs(sj1->p().Pt() - sj2->p().Pt()) / constituent.p().Pt();
            if(sj1->getBTagDisc() < sj2->getBTagDisc()) std::swap(sj1,sj2);
            if(ak8_csv1_mass_)  *ak8_csv1_mass_ =  sj1->p().M();
            if(ak8_csv1_csv_)   *ak8_csv1_csv_ =   (sj1->getBTagDisc() > 0 ? sj1->getBTagDisc() : 0.);
            if(ak8_csv1_ptD_)   *ak8_csv1_ptD_ =   sj1->getExtraVar("ptD");
            if(ak8_csv1_axis1_) *ak8_csv1_axis1_ = sj1->getExtraVar("axis1");
            if(ak8_csv1_mult_)  *ak8_csv1_mult_ =  sj1->getExtraVar("mult");
            if(ak8_csv2_mass_)  *ak8_csv2_mass_ =  sj2->p().M();
            if(ak8_csv2_ptD_)   *ak8_csv2_ptD_ =   sj2->getExtraVar("ptD");
            if(ak8_csv2_axis1_) *ak8_csv2_axis1_ = sj2->getExtraVar("axis1");
            if(ak8_csv2_mult_)  *ak8_csv2_mult_ =  sj2->getExtraVar("mult");

            return true;
        }
        return false;
    }

    BDTDijetInputCalculator::BDTDijetInputCalculator()
    {
        var_fj_sdmass_ = var_fj_tau21_ = var_fj_ptDR_ = var_fj_rel_ptdiff_ = var_sj1_ptD_ = var_sj1_axis1_ = var_sj1_mult_ = var_sj2_ptD_ = var_sj2_axis1_ = var_sj2_mult_ = var_sjmax_csv_ = var_sd_n2_ = nullptr;
    }

    void BDTDijetInputCalculator::mapVars(const std::vector<std::string>& vars, float* data)
    {
        for(int i = 0; i < vars.size(); ++i)
        {
            if(     vars[i].compare("var_fj_sdmass") == 0)      var_fj_sdmass_ = data + i;
            else if(vars[i].compare("var_fj_tau21") == 0)       var_fj_tau21_ = data + i;
            else if(vars[i].compare("var_fj_ptDR") == 0)        var_fj_ptDR_ = data + i;
            else if(vars[i].compare("var_fj_rel_ptdiff") == 0)  var_fj_rel_ptdiff_ = data + i;
            else if(vars[i].compare("var_sj1_ptD") == 0)        var_sj1_ptD_ = data + i;
            else if(vars[i].compare("var_sj1_axis1") == 0)      var_sj1_axis1_ = data + i;
            else if(vars[i].compare("var_sj1_mult") == 0)       var_sj1_mult_ = data + i;
            else if(vars[i].compare("var_sj2_ptD") == 0)        var_sj2_ptD_ = data + i;
            else if(vars[i].compare("var_sj2_axis1") == 0)      var_sj2_axis1_ = data + i;
            else if(vars[i].compare("var_sj2_mult") == 0)       var_sj2_mult_ = data + i;
            else if(vars[i].compare("var_sjmax_csv") == 0)      var_sjmax_csv_ = data + i;
            else if(vars[i].compare("var_sd_n2") == 0)          var_sd_n2_ = data + i;
        }
    }
        
    bool BDTDijetInputCalculator::calculateVars(const TopObject& topCand)
    {
        if(topCand.getNConstituents() == 2)
        {
            const auto* fatjet = topCand.getConstituents()[0];
            if(fatjet->getType() != AK8JET) fatjet = topCand.getConstituents()[1];
            if(var_fj_sdmass_)     *var_fj_sdmass_   = fatjet->getSoftDropMass();
            if(var_fj_tau21_)      *var_fj_tau21_    = fatjet->getTau1() > 0 ? fatjet->getTau2()/fatjet->getTau1() : 1e9;
            // filling subjet variables
            const auto *sj1 = &fatjet->getSubjets()[0];
            const auto *sj2 = &fatjet->getSubjets()[1];
            double fj_deltaR =  ROOT::Math::VectorUtil::DeltaR(sj1->p(), sj2->p());
            if(var_fj_ptDR_)       *var_fj_ptDR_     = fj_deltaR*fatjet->p().Pt();
            if(var_fj_rel_ptdiff_) *var_fj_rel_ptdiff_= std::abs(sj1->p().Pt()-sj2->p().Pt())/fatjet->p().Pt();
            if(var_sj1_ptD_)       *var_sj1_ptD_     = sj1->getExtraVar("ptD");
            if(var_sj1_axis1_)     *var_sj1_axis1_   = sj1->getExtraVar("axis1");
            if(var_sj1_mult_)      *var_sj1_mult_    = sj1->getExtraVar("mult");
            if(var_sj2_ptD_)       *var_sj2_ptD_     = sj2->getExtraVar("ptD");
            if(var_sj2_axis1_)     *var_sj2_axis1_   = sj2->getExtraVar("axis1");
            if(var_sj2_mult_)      *var_sj2_mult_    = sj2->getExtraVar("mult");
            if(var_sjmax_csv_)     *var_sjmax_csv_   = std::max(std::max(sj1->getBTagDisc(),sj2->getBTagDisc()),0.0);
            if(var_sd_n2_)
            {
                double var_sd_0 = sj2->p().Pt()/(sj1->p().Pt()+sj2->p().Pt());
                *var_sd_n2_       = var_sd_0/std::pow(fj_deltaR,-2);
            }

            return true;
        }

        return false;
    }

    TrijetInputCalculator::TrijetInputCalculator()
    {
        cand_pt_ = nullptr;
        cand_p_ = nullptr;
        cand_eta_ = nullptr;
        cand_phi_ = nullptr;
        cand_m_ = nullptr;
        cand_dRMax_ = nullptr;
        cand_dThetaMin_ = nullptr;
        cand_dThetaMax_ = nullptr;
        dRPtTop_ = nullptr;
        dRPtW_ = nullptr;
        sd_n2_ = nullptr;

        for(unsigned int i = 0; i < NCONST; ++i)
        {
            j_m_lab_[i] = nullptr;
            j_CSV_lab_[i] = nullptr;
            j_QGL_lab_[i] = nullptr;
            j_qgMult_lab_[i] = nullptr;
            j_qgPtD_lab_[i] = nullptr;
            j_qgAxis1_lab_[i] = nullptr;
            j_qgAxis2_lab_[i] = nullptr;
            dR12_lab_[i] = nullptr;
            dR12_3_lab_[i] = nullptr;
            j12_m_lab_[i] = nullptr;
            j_p_[i] = nullptr;
            j_p_top_[i] = nullptr;
            j_theta_top_[i] = nullptr;
            j_phi_top_[i] = nullptr;
            j_phi_lab_[i] = nullptr;
            j_eta_lab_[i] = nullptr;
            j_pt_lab_[i] = nullptr;
            j_m_[i] = nullptr;
            j_CSV_[i] = nullptr;
            j_QGL_[i] = nullptr;
            j_recoJetsJecScaleRawToFull_[i] = nullptr;
            j_qgLikelihood_[i] = nullptr;
            j_qgPtD_[i] = nullptr;
            j_qgAxis1_[i] = nullptr;
            j_qgAxis2_[i] = nullptr;
            j_recoJetschargedHadronEnergyFraction_[i] = nullptr;
            j_recoJetschargedEmEnergyFraction_[i] = nullptr;
            j_recoJetsneutralEmEnergyFraction_[i] = nullptr;
            j_recoJetsmuonEnergyFraction_[i] = nullptr;
            j_recoJetsHFHadronEnergyFraction_[i] = nullptr;
            j_recoJetsHFEMEnergyFraction_[i] = nullptr;
            j_recoJetsneutralEnergyFraction_[i] = nullptr;
            j_PhotonEnergyFraction_[i] = nullptr;
            j_ElectronEnergyFraction_[i] = nullptr;
            j_ChargedHadronMultiplicity_[i] = nullptr;
            j_NeutralHadronMultiplicity_[i] = nullptr;
            j_PhotonMultiplicity_[i] = nullptr;
            j_ElectronMultiplicity_[i] = nullptr;
            j_MuonMultiplicity_[i] = nullptr;
            j_DeepCSVb_[i] = nullptr;
            j_DeepCSVc_[i] = nullptr;
            j_DeepCSVl_[i] = nullptr;
            j_DeepCSVbb_[i] = nullptr;
            j_DeepCSVcc_[i] = nullptr;
            j_DeepFlavorb_[i] = nullptr;
            j_DeepFlavorbb_[i] = nullptr;
            j_DeepFlavorlepb_[i] = nullptr;
            j_DeepFlavorc_[i] = nullptr;
            j_DeepFlavoruds_[i] = nullptr;
            j_DeepFlavorg_[i] = nullptr;
            j_CvsL_[i] = nullptr;
            j_CvsB_[i] = nullptr;
            j_CombinedSvtx_[i] = nullptr;
            j_JetProba_[i] = nullptr;
            j_JetBprob_[i] = nullptr;
            j_recoJetsBtag_[i] = nullptr;
            j_recoJetsCharge_[i] = nullptr;
            j_qgMult_[i] = nullptr;
            dTheta_[i] = nullptr;
            j12_m_[i] = nullptr;
        }
    }

    void TrijetInputCalculator::mapVars(const std::vector<std::string>& vars, float* data)
    {
        for(int j = 0; j < vars.size(); ++j)
        {
            if(vars[j].compare("cand_pt") == 0) cand_pt_ = data + j;
            if(vars[j].compare("cand_p") == 0) cand_p_ = data + j;
            if(vars[j].compare("cand_eta") == 0) cand_eta_ = data + j;
            if(vars[j].compare("cand_phi") == 0) cand_phi_ = data + j;
            if(vars[j].compare("cand_m") == 0) cand_m_ = data + j;
            if(vars[j].compare("cand_dRMax") == 0) cand_dRMax_ = data + j;
            if(vars[j].compare("cand_dThetaMin") == 0) cand_dThetaMin_ = data + j;
            if(vars[j].compare("cand_dThetaMax") == 0) cand_dThetaMax_ = data + j;
            if(vars[j].compare("dRPtTop") == 0) dRPtTop_ = data + j;
            if(vars[j].compare("dRPtW") == 0) dRPtW_ = data + j;
            if(vars[j].compare("sd_n2") == 0) sd_n2_ = data + j;

            for(unsigned int i = 0; i < NCONST; ++i)
            {
                int iMin = std::min(i, (i+1)%NCONST);
                int iMax = std::max(i, (i+1)%NCONST);
                int iNNext = (i+2)%NCONST;

                if(vars[j].compare("j" + std::to_string(i + 1) + "_m_lab") == 0)                                  j_m_lab_[i] = data + j;
                if(vars[j].compare("j" + std::to_string(i + 1) + "_CSV_lab") == 0)                                j_CSV_lab_[i] = data + j;
                if(vars[j].compare("j" + std::to_string(i + 1) + "_QGL_lab") == 0)                                j_QGL_lab_[i] = data + j;
                if(vars[j].compare("j" + std::to_string(i + 1) + "_qgMult_lab") == 0)                             j_qgMult_lab_[i] = data + j;
                if(vars[j].compare("j" + std::to_string(i + 1) + "_qgPtD_lab") == 0)                              j_qgPtD_lab_[i] = data + j;
                if(vars[j].compare("j" + std::to_string(i + 1) + "_qgAxis1_lab") == 0)                            j_qgAxis1_lab_[i] = data + j;
                if(vars[j].compare("j" + std::to_string(i + 1) + "_qgAxis2_lab") == 0)                            j_qgAxis2_lab_[i] = data + j;
                if(vars[j].compare("dR" + std::to_string(iMin + 1) + std::to_string(iMax + 1) + "_lab") == 0)     dR12_lab_[i] = data + j;
                if(vars[j].compare("dR" + std::to_string(iNNext + 1) + "_" + std::to_string(iMin + 1) + std::to_string(iMax + 1) + "_lab") == 0) dR12_3_lab_[i] = data + j;
                if(vars[j].compare("j"  + std::to_string(iMin + 1) + std::to_string(iMax + 1) + "_m_lab") == 0)   j12_m_lab_[i] = data + j;
                if(vars[j].compare("j" + std::to_string(i + 1) + "_p") == 0)                                      j_p_[i] = data + j;
                if(vars[j].compare("j" + std::to_string(i + 1) + "_p_top") == 0)                                  j_p_top_[i] = data + j;
                if(vars[j].compare("j" + std::to_string(i + 1) + "_theta_top") == 0)                              j_theta_top_[i] = data + j;
                if(vars[j].compare("j" + std::to_string(i + 1) + "_phi_top") == 0)                                j_phi_top_[i] = data + j;
                if(vars[j].compare("j" + std::to_string(i + 1) + "_phi_lab") == 0)                                j_phi_lab_[i] = data + j;
                if(vars[j].compare("j" + std::to_string(i + 1) + "_eta_lab") == 0)                                j_eta_lab_[i] = data + j;
                if(vars[j].compare("j" + std::to_string(i + 1) + "_pt_lab") == 0)                                 j_pt_lab_[i] = data + j;
                if(vars[j].compare("j" + std::to_string(i + 1) + "_m") == 0)                                      j_m_[i] = data + j;
                if(vars[j].compare("j" + std::to_string(i + 1) + "_CSV") == 0)                                    j_CSV_[i] = data + j;
                if(vars[j].compare("j" + std::to_string(i + 1) + "_QGL") == 0)                                    j_QGL_[i] = data + j;
                if(vars[j].compare("j" + std::to_string(i + 1) + "_recoJetsJecScaleRawToFull") == 0)              j_recoJetsJecScaleRawToFull_[i] = data + j;
                if(vars[j].compare("j" + std::to_string(i + 1) + "_qgLikelihood") == 0)                           j_qgLikelihood_[i] = data + j;
                if(vars[j].compare("j" + std::to_string(i + 1) + "_qgPtD") == 0)                                  j_qgPtD_[i] = data + j;
                if(vars[j].compare("j" + std::to_string(i + 1) + "_qgAxis1") == 0)                                j_qgAxis1_[i] = data + j;
                if(vars[j].compare("j" + std::to_string(i + 1) + "_qgAxis2") == 0)                                j_qgAxis2_[i] = data + j;
                if(vars[j].compare("j" + std::to_string(i + 1) + "_recoJetschargedHadronEnergyFraction") == 0)    j_recoJetschargedHadronEnergyFraction_[i] = data + j;
                if(vars[j].compare("j" + std::to_string(i + 1) + "_recoJetschargedEmEnergyFraction") == 0)        j_recoJetschargedEmEnergyFraction_[i] = data + j;
                if(vars[j].compare("j" + std::to_string(i + 1) + "_recoJetsneutralEmEnergyFraction") == 0)        j_recoJetsneutralEmEnergyFraction_[i] = data + j;
                if(vars[j].compare("j" + std::to_string(i + 1) + "_recoJetsmuonEnergyFraction") == 0)             j_recoJetsmuonEnergyFraction_[i] = data + j;
                if(vars[j].compare("j" + std::to_string(i + 1) + "_recoJetsHFHadronEnergyFraction") == 0)         j_recoJetsHFHadronEnergyFraction_[i] = data + j;
                if(vars[j].compare("j" + std::to_string(i + 1) + "_recoJetsHFEMEnergyFraction") == 0)             j_recoJetsHFEMEnergyFraction_[i] = data + j;
                if(vars[j].compare("j" + std::to_string(i + 1) + "_recoJetsneutralEnergyFraction") == 0)          j_recoJetsneutralEnergyFraction_[i] = data + j;
                if(vars[j].compare("j" + std::to_string(i + 1) + "_PhotonEnergyFraction") == 0)                   j_PhotonEnergyFraction_[i] = data + j;
                if(vars[j].compare("j" + std::to_string(i + 1) + "_ElectronEnergyFraction") == 0)                 j_ElectronEnergyFraction_[i] = data + j;
                if(vars[j].compare("j" + std::to_string(i + 1) + "_ChargedHadronMultiplicity") == 0)              j_ChargedHadronMultiplicity_[i] = data + j;
                if(vars[j].compare("j" + std::to_string(i + 1) + "_NeutralHadronMultiplicity") == 0)              j_NeutralHadronMultiplicity_[i] = data + j;
                if(vars[j].compare("j" + std::to_string(i + 1) + "_PhotonMultiplicity") == 0)                     j_PhotonMultiplicity_[i] = data + j;
                if(vars[j].compare("j" + std::to_string(i + 1) + "_ElectronMultiplicity") == 0)                   j_ElectronMultiplicity_[i] = data + j;
                if(vars[j].compare("j" + std::to_string(i + 1) + "_MuonMultiplicity") == 0)                       j_MuonMultiplicity_[i] = data + j;
                if(vars[j].compare("j" + std::to_string(i + 1) + "_DeepCSVb") == 0)                               j_DeepCSVb_[i] = data + j;
                if(vars[j].compare("j" + std::to_string(i + 1) + "_DeepCSVc") == 0)                               j_DeepCSVc_[i] = data + j;
                if(vars[j].compare("j" + std::to_string(i + 1) + "_DeepCSVl") == 0)                               j_DeepCSVl_[i] = data + j;
                if(vars[j].compare("j" + std::to_string(i + 1) + "_DeepCSVbb") == 0)                              j_DeepCSVbb_[i] = data + j;
                if(vars[j].compare("j" + std::to_string(i + 1) + "_DeepCSVcc") == 0)                              j_DeepCSVcc_[i] = data + j;
                if(vars[j].compare("j" + std::to_string(i + 1) + "_DeepFlavorb") == 0)                            j_DeepFlavorb_[i] = data + j;
                if(vars[j].compare("j" + std::to_string(i + 1) + "_DeepFlavorbb") == 0)                           j_DeepFlavorbb_[i] = data + j;
                if(vars[j].compare("j" + std::to_string(i + 1) + "_DeepFlavorlepb") == 0)                         j_DeepFlavorlepb_[i] = data + j;
                if(vars[j].compare("j" + std::to_string(i + 1) + "_DeepFlavorc") == 0)                            j_DeepFlavorc_[i] = data + j;
                if(vars[j].compare("j" + std::to_string(i + 1) + "_DeepFlavoruds") == 0)                          j_DeepFlavoruds_[i] = data + j;
                if(vars[j].compare("j" + std::to_string(i + 1) + "_DeepFlavorg") == 0)                            j_DeepFlavorg_[i] = data + j;
                if(vars[j].compare("j" + std::to_string(i + 1) + "_CvsL") == 0)                                   j_CvsL_[i] = data + j;
                if(vars[j].compare("j" + std::to_string(i + 1) + "_CvsB") == 0)                                   j_CvsB_[i] = data + j;
                if(vars[j].compare("j" + std::to_string(i + 1) + "_CombinedSvtx") == 0)                           j_CombinedSvtx_[i] = data + j;
                if(vars[j].compare("j" + std::to_string(i + 1) + "_JetProba") == 0)                               j_JetProba_[i] = data + j;
                if(vars[j].compare("j" + std::to_string(i + 1) + "_JetBprob") == 0)                               j_JetBprob_[i] = data + j;
                if(vars[j].compare("j" + std::to_string(i + 1) + "_recoJetsBtag") == 0)                           j_recoJetsBtag_[i] = data + j;
                if(vars[j].compare("j" + std::to_string(i + 1) + "_recoJetsCharge") == 0)                         j_recoJetsCharge_[i] = data + j;
                if(vars[j].compare("j" + std::to_string(i + 1) + "_qgMult") == 0)                                 j_qgMult_[i] = data + j;
                if(vars[j].compare("dTheta" + std::to_string(iMin + 1) + std::to_string(iMax + 1)) == 0)          dTheta_[i] = data + j;
                if(vars[j].compare("j"   + std::to_string(iMin + 1) + std::to_string(iMax + 1) + "_m") == 0)      j12_m_[i] = data + j;
            }
        }
    }
        
    bool TrijetInputCalculator::calculateVars(const TopObject& topCand)
    {
        if(topCand.getNConstituents() == 3)
        {
            std::map<std::string, double> varMap;

            //Get top candidate variables
            if(cand_pt_)        *cand_pt_        = topCand.p().Pt();
            if(cand_p_)         *cand_p_         = topCand.p().P();
            if(cand_eta_)       *cand_eta_       = topCand.p().Eta();
            if(cand_phi_)       *cand_phi_       = topCand.p().Phi();
            if(cand_m_)         *cand_m_         = topCand.p().M();
            if(cand_dRMax_)     *cand_dRMax_     = topCand.getDRmax();
            if(cand_dThetaMin_) *cand_dThetaMin_ = topCand.getDThetaMin();
            if(cand_dThetaMax_) *cand_dThetaMax_ = topCand.getDThetaMax();

            //Get Constituents
            //Get a copy instead of the reference
            std::vector<Constituent const *> top_constituents = topCand.getConstituents();

            //resort by CSV
            std::sort(top_constituents.begin(), top_constituents.end(), [](const Constituent * const c1, const Constituent * const c2){ return c1->getBTagDisc() > c2->getBTagDisc(); });

            //Get constituent variables before deboost
            for(unsigned int i = 0; i < top_constituents.size(); ++i)
            {
                if(j_m_lab_[i])       *j_m_lab_[i]       = top_constituents[i]->p().M();
                if(j_CSV_lab_[i])     *j_CSV_lab_[i]     = top_constituents[i]->getBTagDisc();
                //Here we fake the QGL if it is a b jet
                if(j_QGL_lab_[i])     *j_QGL_lab_[i]     = top_constituents[i]->getQGLikelihood();
                if(j_qgMult_lab_[i])  *j_qgMult_lab_[i]  = top_constituents[i]->getQGMult();
                if(j_qgPtD_lab_[i])   *j_qgPtD_lab_[i]   = top_constituents[i]->getQGPtD();
                if(j_qgAxis1_lab_[i]) *j_qgAxis1_lab_[i] = top_constituents[i]->getQGAxis1();
                if(j_qgAxis2_lab_[i]) *j_qgAxis2_lab_[i] = top_constituents[i]->getQGAxis2();

                //index of next jet (assumes < 4 jets)
                unsigned int iNext = (i + 1) % top_constituents.size();
                unsigned int iNNext = (i + 2) % top_constituents.size();
                unsigned int iMin = std::min(i, iNext);
                unsigned int iMax = std::max(i, iNext);

                //Calculate the angle variables
                if(dR12_lab_[i])   *dR12_lab_[i]   = ROOT::Math::VectorUtil::DeltaR(top_constituents[i]->p(), top_constituents[iNext]->p());
                if(dR12_3_lab_[i]) *dR12_3_lab_[i] = ROOT::Math::VectorUtil::DeltaR(top_constituents[iNNext]->p(), top_constituents[i]->p() + top_constituents[iNext]->p());

                //calculate pair masses
                auto jetPair = top_constituents[i]->p() + top_constituents[iNext]->p();
                if(j12_m_lab_[i]) *j12_m_lab_[i] = jetPair.M();
            }

            if(dRPtTop_) *dRPtTop_ = varMap["dR1_23_lab"] * varMap["cand_pt"];
            if(dRPtW_) *dRPtW_ = varMap["dR23_lab"] * (top_constituents[1]->p() + top_constituents[2]->p()).Pt();
            double var_sd_0 = top_constituents[2]->p().Pt()/(top_constituents[1]->p().Pt()+top_constituents[2]->p().Pt());
            if(sd_n2_) *sd_n2_ = var_sd_0/std::pow(varMap["dR23_lab"], -2);

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
                if(j_p_[i]) *j_p_[i]     = RF_constituents[i].p().P();

                //This is a bit silly
                TLorentzVector p4(RF_constituents[i].p());
                p4.Boost(topCand.p().BoostVector());
                if(j_p_top_[i])     *j_p_top_[i]     = p4.P();
                if(j_theta_top_[i]) *j_theta_top_[i] = topCand.p().Angle(p4.Vect());
                if(j_phi_top_[i])   *j_phi_top_[i]   = ROOT::Math::VectorUtil::DeltaPhi(RF_constituents[i].p(), RF_constituents[0].p());

                if(j_phi_lab_[i]) *j_phi_lab_[i]   = p4.Phi();
                if(j_eta_lab_[i]) *j_eta_lab_[i]   = p4.Eta();
                if(j_pt_lab_[i])  *j_pt_lab_[i]    = p4.Pt();
            
                if(j_m_[i])   *j_m_[i]     = RF_constituents[i].p().M();
                if(j_CSV_[i]) *j_CSV_[i]   = RF_constituents[i].getBTagDisc();
                //Here we fake the QGL if it is a b jet
                if(j_QGL_[i]) *j_QGL_[i]   = RF_constituents[i].getQGLikelihood();

                if(j_recoJetsJecScaleRawToFull_[i])           *j_recoJetsJecScaleRawToFull_[i]           = relu(RF_constituents[i].getExtraVar("recoJetsJecScaleRawToFull"));
                if(j_qgLikelihood_[i])                        *j_qgLikelihood_[i]                        = relu(RF_constituents[i].getExtraVar("qgLikelihood"));
                if(j_qgPtD_[i])                               *j_qgPtD_[i]                               = relu(RF_constituents[i].getExtraVar("qgPtD"));
                if(j_qgAxis1_[i])                             *j_qgAxis1_[i]                             = relu(RF_constituents[i].getExtraVar("qgAxis1"));
                if(j_qgAxis2_[i])                             *j_qgAxis2_[i]                             = relu(RF_constituents[i].getExtraVar("qgAxis2"));
                if(j_recoJetschargedHadronEnergyFraction_[i]) *j_recoJetschargedHadronEnergyFraction_[i] = relu(RF_constituents[i].getExtraVar("recoJetschargedHadronEnergyFraction"));
                if(j_recoJetschargedEmEnergyFraction_[i])     *j_recoJetschargedEmEnergyFraction_[i]     = relu(RF_constituents[i].getExtraVar("recoJetschargedEmEnergyFraction"));
                if(j_recoJetsneutralEmEnergyFraction_[i])     *j_recoJetsneutralEmEnergyFraction_[i]     = relu(RF_constituents[i].getExtraVar("recoJetsneutralEmEnergyFraction"));
                if(j_recoJetsmuonEnergyFraction_[i])          *j_recoJetsmuonEnergyFraction_[i]          = relu(RF_constituents[i].getExtraVar("recoJetsmuonEnergyFraction"));
                if(j_recoJetsHFHadronEnergyFraction_[i])      *j_recoJetsHFHadronEnergyFraction_[i]      = relu(RF_constituents[i].getExtraVar("recoJetsHFHadronEnergyFraction"));
                if(j_recoJetsHFEMEnergyFraction_[i])          *j_recoJetsHFEMEnergyFraction_[i]          = relu(RF_constituents[i].getExtraVar("recoJetsHFEMEnergyFraction"));
                if(j_recoJetsneutralEnergyFraction_[i])       *j_recoJetsneutralEnergyFraction_[i]       = relu(RF_constituents[i].getExtraVar("recoJetsneutralEnergyFraction"));
                if(j_PhotonEnergyFraction_[i])                *j_PhotonEnergyFraction_[i]                = relu(RF_constituents[i].getExtraVar("PhotonEnergyFraction"));
                if(j_ElectronEnergyFraction_[i])              *j_ElectronEnergyFraction_[i]              = relu(RF_constituents[i].getExtraVar("ElectronEnergyFraction"));
                if(j_ChargedHadronMultiplicity_[i])           *j_ChargedHadronMultiplicity_[i]           = relu(RF_constituents[i].getExtraVar("ChargedHadronMultiplicity"));
                if(j_NeutralHadronMultiplicity_[i])           *j_NeutralHadronMultiplicity_[i]           = relu(RF_constituents[i].getExtraVar("NeutralHadronMultiplicity"));
                if(j_PhotonMultiplicity_[i])                  *j_PhotonMultiplicity_[i]                  = relu(RF_constituents[i].getExtraVar("PhotonMultiplicity"));
                if(j_ElectronMultiplicity_[i])                *j_ElectronMultiplicity_[i]                = relu(RF_constituents[i].getExtraVar("ElectronMultiplicity"));
                if(j_MuonMultiplicity_[i])                    *j_MuonMultiplicity_[i]                    = relu(RF_constituents[i].getExtraVar("MuonMultiplicity"));
                if(j_DeepCSVb_[i])                            *j_DeepCSVb_[i]                            = relu(RF_constituents[i].getExtraVar("DeepCSVb"));
                if(j_DeepCSVc_[i])                            *j_DeepCSVc_[i]                            = relu(RF_constituents[i].getExtraVar("DeepCSVc"));
                if(j_DeepCSVl_[i])                            *j_DeepCSVl_[i]                            = relu(RF_constituents[i].getExtraVar("DeepCSVl"));
                if(j_DeepCSVbb_[i])                           *j_DeepCSVbb_[i]                           = relu(RF_constituents[i].getExtraVar("DeepCSVbb"));
                if(j_DeepCSVcc_[i])                           *j_DeepCSVcc_[i]                           = relu(RF_constituents[i].getExtraVar("DeepCSVcc"));
                if(j_DeepFlavorb_[i])                         *j_DeepFlavorb_[i]                         = relu(RF_constituents[i].getExtraVar("DeepFlavorb"));
                if(j_DeepFlavorbb_[i])                        *j_DeepFlavorbb_[i]                        = relu(RF_constituents[i].getExtraVar("DeepFlavorbb"));
                if(j_DeepFlavorlepb_[i])                      *j_DeepFlavorlepb_[i]                      = relu(RF_constituents[i].getExtraVar("DeepFlavorlepb"));
                if(j_DeepFlavorc_[i])                         *j_DeepFlavorc_[i]                         = relu(RF_constituents[i].getExtraVar("DeepFlavorc"));
                if(j_DeepFlavoruds_[i])                       *j_DeepFlavoruds_[i]                       = relu(RF_constituents[i].getExtraVar("DeepFlavoruds"));
                if(j_DeepFlavorg_[i])                         *j_DeepFlavorg_[i]                         = relu(RF_constituents[i].getExtraVar("DeepFlavorg"));
                if(j_CvsL_[i])                                *j_CvsL_[i]                                = relu(RF_constituents[i].getExtraVar("CvsL"));
                if(j_CvsB_[i])                                *j_CvsB_[i]                                = relu(RF_constituents[i].getExtraVar("CvsB"));
                if(j_CombinedSvtx_[i])                        *j_CombinedSvtx_[i]                        = relu(RF_constituents[i].getExtraVar("CombinedSvtx"));
                if(j_JetProba_[i])                            *j_JetProba_[i]                            = relu(RF_constituents[i].getExtraVar("JetProba"));
                if(j_JetBprob_[i])                            *j_JetBprob_[i]                            = relu(RF_constituents[i].getExtraVar("JetBprob"));
                if(j_recoJetsBtag_[i])                        *j_recoJetsBtag_[i]                        = relu(RF_constituents[i].getExtraVar("recoJetsBtag"));
                if(j_recoJetsCharge_[i])                      *j_recoJetsCharge_[i]                      = relu(RF_constituents[i].getExtraVar("recoJetsCharge"), -2);
                if(j_qgMult_[i])                              *j_qgMult_[i]                              = relu(RF_constituents[i].getExtraVar("qgMult"));            

                //index of next jet (assumes < 4 jets)
                unsigned int iNext = (i + 1) % RF_constituents.size();
                unsigned int iMin = std::min(i, iNext);
                unsigned int iMax = std::max(i, iNext);

                //Calculate delta angle variables
                if(dTheta_[i]) *dTheta_[i] = RF_constituents[iMin].p().Angle(RF_constituents[iMax].p().Vect());

                //calculate pair masses
                auto jetPair = RF_constituents[i].p() + RF_constituents[iNext].p();
                if(j12_m_[i]) *j12_m_[i] = jetPair.M();
            }
                
            return true;
        }
            
        return false;
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
