#ifndef TOPTAGGERUTILITIES_H
#define TOPTAGGERUTILITIES_H

/** \file */

#include <vector>
#include <map>

class Constituent;
class TopObject;
class TopTaggerResults;

#include "TopTagger/TopTagger/interface/Constituent.h"
#include "TopTagger/CfgParser/include/TTException.h"

#include "TF1.h"
#include "TFile.h"
#include "TLorentzVector.h"
#include "Math/VectorUtil.h"

//This file is for utility functions which do not fit in other classes 
//DO NOT DEFINE FUNCTIONS IN LINE!!!!!!

namespace ttUtility
{
    /**
     *Class to hold the gen level inputs.
     */
    class ConstGenInputs
    {
    protected:
        const std::vector<TLorentzVector>* hadGenTops_;
        const std::vector<std::vector<const TLorentzVector*>>* hadGenTopDaughters_;

        ///Default constructor
        ConstGenInputs();
        /**
         *Constructs gen inputs from gen level vectors
         *@param hadGenTops Vector of hadronicly decaying gen top TLorentzVectors
         *@param hadGenTopDaughters Vector of direct decay daughters of the top quarks
         */
        ConstGenInputs(const std::vector<TLorentzVector>& hadGenTops, const std::vector<std::vector<const TLorentzVector*>>& hadGenTopDaughters);
    };

    /**
     *Class to gather the information necessary to construct the AK4 jet constituents
     */
    template<typename FLOATTYPE, typename FLOATCONTAINERTYPE = std::vector<FLOATTYPE>, typename INTCONTAINERTYPE = std::vector<int>>
    class ConstAK4Inputs : public ConstGenInputs
    {
    private:
        const std::vector<TLorentzVector>* jetsLVec_;
        const FLOATCONTAINERTYPE* btagFactors_;
        const FLOATCONTAINERTYPE* qgLikelihood_;
        const INTCONTAINERTYPE* qgMult_;
        const FLOATCONTAINERTYPE* qgPtD_;
        const FLOATCONTAINERTYPE* qgAxis1_;
        const FLOATCONTAINERTYPE* qgAxis2_;

        std::map<std::string, const FLOATCONTAINERTYPE*> extraInputVariables_;

    public:
        /**
         *Basic constructor with QGL
         *@param jetsLVec Jet TLorentzVectors for each jet
         *@param btagFactors B-tag discriminators for each jet
         *@param qgLikelihood Quark-gluon likelihoods for each jet
         */
        ConstAK4Inputs(const std::vector<TLorentzVector>& jetsLVec, const FLOATCONTAINERTYPE& btagFactors, const FLOATCONTAINERTYPE& qgLikelihood) : ConstGenInputs(), jetsLVec_(&jetsLVec), btagFactors_(&btagFactors), qgLikelihood_(&qgLikelihood), qgMult_(nullptr), qgPtD_(nullptr), qgAxis1_(nullptr), qgAxis2_(nullptr)    {}
        /**
         *Basic constructor
         *@param jetsLVec Jet TLorentzVectors for each jet
         *@param btagFactors B-tag discriminators for each jet
         */
        ConstAK4Inputs(const std::vector<TLorentzVector>& jetsLVec, const FLOATCONTAINERTYPE& btagFactors) : ConstGenInputs(), jetsLVec_(&jetsLVec), btagFactors_(&btagFactors), qgLikelihood_(nullptr), qgMult_(nullptr), qgPtD_(nullptr), qgAxis1_(nullptr), qgAxis2_(nullptr) {}
        /**
         *Constructor with gen informaion 
         *@param jetsLVec Jet TLorentzVectors for each jet
         *@param btagFactors B-tag discriminators for each jet
         *@param qgLikelihood Quark-gluon likelihoods for each jet
         *@param hadGenTops Vector of hadronicly decaying gen top TLorentzVectors
         *@param hadGenTopDaughters Vector of direct decay daughters of the top quarks
         */
        ConstAK4Inputs(const std::vector<TLorentzVector>& jetsLVec, const FLOATCONTAINERTYPE& btagFactors, const FLOATCONTAINERTYPE& qgLikelihood, const std::vector<TLorentzVector>& hadGenTops, const std::vector<std::vector<const TLorentzVector*>>& hadGenTopDaughters) : ConstGenInputs(hadGenTops, hadGenTopDaughters), jetsLVec_(&jetsLVec), btagFactors_(&btagFactors), qgLikelihood_(&qgLikelihood), qgMult_(nullptr), qgPtD_(nullptr), qgAxis1_(nullptr), qgAxis2_(nullptr) {}
        /**
         *Adds jet shape inputs from the quark-gluon likelihood calculator
         */
        void addQGLVectors(const INTCONTAINERTYPE& qgMult, const FLOATCONTAINERTYPE& qgPtD, const FLOATCONTAINERTYPE& qgAxis1, const FLOATCONTAINERTYPE& qgAxis2)
        {
            qgMult_ = &qgMult;
            qgPtD_ = &qgPtD;
            qgAxis1_ = &qgAxis1;
            qgAxis2_ = &qgAxis2;
        }

        /**
         *Adds a vector holding additional variables which will be inserted into the "extraVars" map of the Constituent
         *@param name The name to use to store the extra variables
         *@param vector the values for the extra variable for each jet
         */
        void addSupplamentalVector(const std::string& name, const FLOATCONTAINERTYPE& vector)
        {
            extraInputVariables_[name] = &vector;
        }

        /**
         *Called to fill the constituents using the information collected in the class. 
         *Not intended to be called directly.
         *@param constituents vector to insert AK4 constituents into
         */
        void packageConstituents(std::vector<Constituent>& constituents)
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
                constituents.emplace_back((*jetsLVec_)[iJet], static_cast<double>((*btagFactors_)[iJet]), static_cast<double>((qgLikelihood_ != nullptr)?((*qgLikelihood_)[iJet]):(0.0)));

                //Add additional QGL info if it is provided 
                if(qgMult_ && qgPtD_ && qgAxis1_ && qgAxis2_) 
                {
                    constituents.back().setQGLVars(static_cast<double>((*qgMult_)[iJet]), static_cast<double>((*qgPtD_)[iJet]), static_cast<double>((*qgAxis1_)[iJet]), static_cast<double>((*qgAxis2_)[iJet]));
                }

                //Add any extra variables that have been added 
                for(const auto& extraVar : extraInputVariables_)
                {
                    if(extraVar.second && iJet < extraVar.second->size()) constituents.back().setExtraVar(extraVar.first, static_cast<double>((*extraVar.second)[iJet]));
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

    };

    typedef ConstAK4Inputs<float> ConstAK4InputsFloat;

    /**
     *Class to gather the information necessary to construct the AK8 jet constituents
     */
    template<typename FLOATTYPE>
    class ConstAK8Inputs : public ConstGenInputs
    {
    private:
        const std::vector<TLorentzVector>* jetsLVec_;
        const std::vector<FLOATTYPE>* tau1_;
        const std::vector<FLOATTYPE>* tau2_;
        const std::vector<FLOATTYPE>* tau3_;
        const std::vector<FLOATTYPE>* deepAK8Top_;
        const std::vector<FLOATTYPE>* deepAK8W_;
        const std::vector<FLOATTYPE>* softDropMass_;
        const std::vector<FLOATTYPE>* subjetsBtag_;
        const std::vector<FLOATTYPE>* subjetsMult_;
        const std::vector<FLOATTYPE>* subjetsPtD_;
        const std::vector<FLOATTYPE>* subjetsAxis1_;
        const std::vector<FLOATTYPE>* subjetsAxis2_;
        const std::vector<TLorentzVector>* subjetsLVec_;
        const std::vector<std::vector<TLorentzVector>>* vecSubjetsLVec_;
        const std::vector<std::vector<FLOATTYPE>>* vecSubjetsBtag_;
        const std::vector<std::vector<FLOATTYPE>>* vecSubjetsMult_;
        const std::vector<std::vector<FLOATTYPE>>* vecSubjetsPtD_;
        const std::vector<std::vector<FLOATTYPE>>* vecSubjetsAxis1_;
        const std::vector<std::vector<FLOATTYPE>>* vecSubjetsAxis2_;
        TF1* puppisd_corrGEN_;
        TF1* puppisd_corrRECO_cen_;
        TF1* puppisd_corrRECO_for_;
        
        double getPUPPIweight(double puppipt, double puppieta ) const
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

    public:
        ConstAK8Inputs(const std::vector<TLorentzVector>& jetsLVec, const std::vector<FLOATTYPE>& tau1, const std::vector<FLOATTYPE>& tau2, const std::vector<FLOATTYPE>& tau3, const std::vector<FLOATTYPE>& softDropMass, const std::vector<TLorentzVector>& subjetsLVec) : ConstGenInputs(), jetsLVec_(&jetsLVec), tau1_(&tau1), tau2_(&tau2), tau3_(&tau3), deepAK8Top_(nullptr), deepAK8W_(nullptr), softDropMass_(&softDropMass), subjetsBtag_(nullptr), subjetsMult_(nullptr), subjetsPtD_(nullptr), subjetsAxis1_(nullptr), subjetsAxis2_(nullptr), subjetsLVec_(&subjetsLVec), vecSubjetsLVec_(nullptr), vecSubjetsBtag_(nullptr), vecSubjetsMult_(nullptr), vecSubjetsPtD_(nullptr), vecSubjetsAxis1_(nullptr), vecSubjetsAxis2_(nullptr), puppisd_corrGEN_(nullptr), puppisd_corrRECO_cen_(nullptr), puppisd_corrRECO_for_(nullptr) { }

        ConstAK8Inputs(const std::vector<TLorentzVector>& jetsLVec, const std::vector<FLOATTYPE>& deepAK8Top, const std::vector<FLOATTYPE>& deepAK8W, const std::vector<FLOATTYPE>& softDropMass, const std::vector<TLorentzVector>& subjetsLVec) : ConstGenInputs(), jetsLVec_(&jetsLVec), tau1_(nullptr), tau2_(nullptr), tau3_(nullptr), deepAK8Top_(&deepAK8Top), deepAK8W_(&deepAK8W), softDropMass_(&softDropMass), subjetsBtag_(nullptr), subjetsMult_(nullptr), subjetsPtD_(nullptr), subjetsAxis1_(nullptr), subjetsAxis2_(nullptr), subjetsLVec_(&subjetsLVec), vecSubjetsLVec_(nullptr), vecSubjetsBtag_(nullptr), vecSubjetsMult_(nullptr), vecSubjetsPtD_(nullptr), vecSubjetsAxis1_(nullptr), vecSubjetsAxis2_(nullptr), puppisd_corrGEN_(nullptr), puppisd_corrRECO_cen_(nullptr), puppisd_corrRECO_for_(nullptr) { }

        ConstAK8Inputs(const std::vector<TLorentzVector>& jetsLVec, const std::vector<FLOATTYPE>& tau1, const std::vector<FLOATTYPE>& tau2, const std::vector<FLOATTYPE>& tau3, const std::vector<FLOATTYPE>& softDropMass, const std::vector<TLorentzVector>& subjetsLVec, const std::vector<FLOATTYPE>& subjetsBtag, const std::vector<FLOATTYPE>& subjetsMult, const std::vector<FLOATTYPE>& subjetsPtD, const std::vector<FLOATTYPE>& subjetsAxis1, const std::vector<FLOATTYPE>& subjetsAxis2) : ConstGenInputs(), jetsLVec_(&jetsLVec), tau1_(&tau1), tau2_(&tau2), tau3_(&tau3), deepAK8Top_(nullptr), deepAK8W_(nullptr), softDropMass_(&softDropMass), subjetsBtag_(&subjetsBtag), subjetsMult_(&subjetsMult), subjetsPtD_(&subjetsPtD), subjetsAxis1_(&subjetsAxis1), subjetsAxis2_(&subjetsAxis2), subjetsLVec_(&subjetsLVec), vecSubjetsLVec_(nullptr), vecSubjetsBtag_(nullptr), vecSubjetsMult_(nullptr), vecSubjetsPtD_(nullptr), vecSubjetsAxis1_(nullptr), vecSubjetsAxis2_(nullptr), puppisd_corrGEN_(nullptr), puppisd_corrRECO_cen_(nullptr), puppisd_corrRECO_for_(nullptr) { }

        ConstAK8Inputs(const std::vector<TLorentzVector>& jetsLVec, const std::vector<FLOATTYPE>& tau1, const std::vector<FLOATTYPE>& tau2, const std::vector<FLOATTYPE>& tau3, const std::vector<FLOATTYPE>& softDropMass, const std::vector<std::vector<TLorentzVector> >& vecSubJetsLVec) : ConstGenInputs(), jetsLVec_(&jetsLVec), tau1_(&tau1), tau2_(&tau2), tau3_(&tau3), deepAK8Top_(nullptr), deepAK8W_(nullptr), softDropMass_(&softDropMass), subjetsBtag_(nullptr), subjetsMult_(nullptr), subjetsPtD_(nullptr), subjetsAxis1_(nullptr), subjetsAxis2_(nullptr), subjetsLVec_(nullptr), vecSubjetsLVec_(&vecSubJetsLVec), vecSubjetsBtag_(nullptr), vecSubjetsMult_(nullptr), vecSubjetsPtD_(nullptr), vecSubjetsAxis1_(nullptr), vecSubjetsAxis2_(nullptr), puppisd_corrGEN_(nullptr), puppisd_corrRECO_cen_(nullptr), puppisd_corrRECO_for_(nullptr) { }

        ConstAK8Inputs(const std::vector<TLorentzVector>& jetsLVec, const std::vector<FLOATTYPE>& deepAK8Top, const std::vector<FLOATTYPE>& deepAK8W, const std::vector<FLOATTYPE>& softDropMass, const std::vector<std::vector<TLorentzVector> >& vecSubJetsLVec) : ConstGenInputs(), jetsLVec_(&jetsLVec), tau1_(nullptr), tau2_(nullptr), tau3_(nullptr), deepAK8Top_(&deepAK8Top), deepAK8W_(&deepAK8W), softDropMass_(&softDropMass), subjetsBtag_(nullptr), subjetsMult_(nullptr), subjetsPtD_(nullptr), subjetsAxis1_(nullptr), subjetsAxis2_(nullptr), subjetsLVec_(nullptr), vecSubjetsLVec_(&vecSubJetsLVec), vecSubjetsBtag_(nullptr), vecSubjetsMult_(nullptr), vecSubjetsPtD_(nullptr), vecSubjetsAxis1_(nullptr), vecSubjetsAxis2_(nullptr), puppisd_corrGEN_(nullptr), puppisd_corrRECO_cen_(nullptr), puppisd_corrRECO_for_(nullptr) { }

        ConstAK8Inputs(const std::vector<TLorentzVector>& jetsLVec, const std::vector<FLOATTYPE>& tau1, const std::vector<FLOATTYPE>& tau2, const std::vector<FLOATTYPE>& tau3, const std::vector<FLOATTYPE>& softDropMass, const std::vector<TLorentzVector>& subjetsLVec, const std::vector<TLorentzVector>& hadGenTops, const std::vector<std::vector<const TLorentzVector*>>& hadGenTopDaughters) : ConstGenInputs(hadGenTops, hadGenTopDaughters), jetsLVec_(&jetsLVec), tau1_(&tau1), tau2_(&tau2), tau3_(&tau3), deepAK8Top_(nullptr), deepAK8W_(nullptr), softDropMass_(&softDropMass), subjetsBtag_(nullptr), subjetsMult_(nullptr), subjetsPtD_(nullptr), subjetsAxis1_(nullptr), subjetsAxis2_(nullptr), subjetsLVec_(&subjetsLVec), vecSubjetsLVec_(nullptr), vecSubjetsBtag_(nullptr), vecSubjetsMult_(nullptr), vecSubjetsPtD_(nullptr), vecSubjetsAxis1_(nullptr), vecSubjetsAxis2_(nullptr), puppisd_corrGEN_(nullptr), puppisd_corrRECO_cen_(nullptr), puppisd_corrRECO_for_(nullptr) { }

        ConstAK8Inputs(const std::vector<TLorentzVector>& jetsLVec, const std::vector<FLOATTYPE>& deepAK8Top, const std::vector<FLOATTYPE>& deepAK8W, const std::vector<FLOATTYPE>& softDropMass, const std::vector<TLorentzVector>& subjetsLVec, const std::vector<TLorentzVector>& hadGenTops, const std::vector<std::vector<const TLorentzVector*>>& hadGenTopDaughters) : ConstGenInputs(hadGenTops, hadGenTopDaughters), jetsLVec_(&jetsLVec), tau1_(nullptr), tau2_(nullptr), tau3_(nullptr), deepAK8Top_(&deepAK8Top), deepAK8W_(&deepAK8W), softDropMass_(&softDropMass), subjetsBtag_(nullptr), subjetsMult_(nullptr), subjetsPtD_(nullptr), subjetsAxis1_(nullptr), subjetsAxis2_(nullptr), subjetsLVec_(&subjetsLVec), vecSubjetsLVec_(nullptr), vecSubjetsBtag_(nullptr), vecSubjetsMult_(nullptr), vecSubjetsPtD_(nullptr), vecSubjetsAxis1_(nullptr), vecSubjetsAxis2_(nullptr), puppisd_corrGEN_(nullptr), puppisd_corrRECO_cen_(nullptr), puppisd_corrRECO_for_(nullptr) { }

        ConstAK8Inputs(const std::vector<TLorentzVector>& jetsLVec, const std::vector<FLOATTYPE>& tau1, const std::vector<FLOATTYPE>& tau2, const std::vector<FLOATTYPE>& tau3, const std::vector<FLOATTYPE>& softDropMass, const std::vector<TLorentzVector>& subjetsLVec, const std::vector<FLOATTYPE>& subjetsBtag, const std::vector<FLOATTYPE>& subjetsMult, const std::vector<FLOATTYPE>& subjetsPtD, const std::vector<FLOATTYPE>& subjetsAxis1, const std::vector<FLOATTYPE>& subjetsAxis2, const std::vector<TLorentzVector>& hadGenTops, const std::vector<std::vector<const TLorentzVector*>>& hadGenTopDaughters) : ConstGenInputs(hadGenTops, hadGenTopDaughters), jetsLVec_(&jetsLVec), tau1_(&tau1), tau2_(&tau2), tau3_(&tau3), deepAK8Top_(nullptr), deepAK8W_(nullptr), softDropMass_(&softDropMass), subjetsBtag_(&subjetsBtag), subjetsMult_(&subjetsMult), subjetsPtD_(&subjetsPtD), subjetsAxis1_(&subjetsAxis1), subjetsAxis2_(&subjetsAxis2), subjetsLVec_(&subjetsLVec), vecSubjetsLVec_(nullptr), vecSubjetsBtag_(nullptr), vecSubjetsMult_(nullptr), vecSubjetsPtD_(nullptr), vecSubjetsAxis1_(nullptr), vecSubjetsAxis2_(nullptr), puppisd_corrGEN_(nullptr), puppisd_corrRECO_cen_(nullptr), puppisd_corrRECO_for_(nullptr) { }

        ConstAK8Inputs(const std::vector<TLorentzVector>& jetsLVec, const std::vector<FLOATTYPE>& tau1, const std::vector<FLOATTYPE>& tau2, const std::vector<FLOATTYPE>& tau3, const std::vector<FLOATTYPE>& softDropMass, const std::vector<std::vector<TLorentzVector> >& vecSubJetsLVec, const std::vector<TLorentzVector>& hadGenTops, const std::vector<std::vector<const TLorentzVector*>>& hadGenTopDaughters) : ConstGenInputs(hadGenTops, hadGenTopDaughters), jetsLVec_(&jetsLVec), tau1_(&tau1), tau2_(&tau2), tau3_(&tau3), deepAK8Top_(nullptr), deepAK8W_(nullptr), softDropMass_(&softDropMass), subjetsBtag_(nullptr), subjetsMult_(nullptr), subjetsPtD_(nullptr), subjetsAxis1_(nullptr), subjetsAxis2_(nullptr), subjetsLVec_(nullptr), vecSubjetsLVec_(&vecSubJetsLVec), vecSubjetsBtag_(nullptr), vecSubjetsMult_(nullptr), vecSubjetsPtD_(nullptr), vecSubjetsAxis1_(nullptr), vecSubjetsAxis2_(nullptr), puppisd_corrGEN_(nullptr), puppisd_corrRECO_cen_(nullptr), puppisd_corrRECO_for_(nullptr) { }

        ConstAK8Inputs(const std::vector<TLorentzVector>& jetsLVec, const std::vector<FLOATTYPE>& deepAK8Top, const std::vector<FLOATTYPE>& deepAK8W, const std::vector<FLOATTYPE>& softDropMass, const std::vector<std::vector<TLorentzVector> >& vecSubJetsLVec, const std::vector<TLorentzVector>& hadGenTops, const std::vector<std::vector<const TLorentzVector*>>& hadGenTopDaughters) : ConstGenInputs(hadGenTops, hadGenTopDaughters), jetsLVec_(&jetsLVec), tau1_(nullptr), tau2_(nullptr), tau3_(nullptr), deepAK8Top_(&deepAK8Top), deepAK8W_(&deepAK8W), softDropMass_(&softDropMass), subjetsBtag_(nullptr), subjetsMult_(nullptr), subjetsPtD_(nullptr), subjetsAxis1_(nullptr), subjetsAxis2_(nullptr), subjetsLVec_(nullptr), vecSubjetsLVec_(&vecSubJetsLVec), vecSubjetsBtag_(nullptr), vecSubjetsMult_(nullptr), vecSubjetsPtD_(nullptr), vecSubjetsAxis1_(nullptr), vecSubjetsAxis2_(nullptr), puppisd_corrGEN_(nullptr), puppisd_corrRECO_cen_(nullptr), puppisd_corrRECO_for_(nullptr) { }

	ConstAK8Inputs(const std::vector<TLorentzVector>& jetsLVec, const std::vector<FLOATTYPE>& tau1, const std::vector<FLOATTYPE>& tau2, const std::vector<FLOATTYPE>& tau3, const std::vector<FLOATTYPE>& softDropMass, const std::vector<std::vector<TLorentzVector> >& vecSubjetsLVec, const std::vector<std::vector<FLOATTYPE> >& vecSubjetsBtag, const std::vector<std::vector<FLOATTYPE> >& vecSubjetsMult, const std::vector<std::vector<FLOATTYPE> >& vecSubjetsPtD, const std::vector<std::vector<FLOATTYPE> >& vecSubjetsAxis1, const std::vector<std::vector<FLOATTYPE> >& vecSubjetsAxis2, const std::vector<TLorentzVector>& hadGenTops, const std::vector<std::vector<const TLorentzVector*> >& hadGenTopDaughters) : ConstGenInputs(hadGenTops, hadGenTopDaughters), jetsLVec_(&jetsLVec), tau1_(&tau1), tau2_(&tau2), tau3_(&tau3), deepAK8Top_(nullptr), deepAK8W_(nullptr), softDropMass_(&softDropMass), subjetsBtag_(nullptr), subjetsMult_(nullptr), subjetsPtD_(nullptr), subjetsAxis1_(nullptr), subjetsAxis2_(nullptr), subjetsLVec_(nullptr), vecSubjetsLVec_(&vecSubjetsLVec), vecSubjetsBtag_(&vecSubjetsBtag), vecSubjetsMult_(&vecSubjetsMult), vecSubjetsPtD_(&vecSubjetsPtD), vecSubjetsAxis1_(&vecSubjetsAxis1), vecSubjetsAxis2_(&vecSubjetsAxis2), puppisd_corrGEN_(nullptr), puppisd_corrRECO_cen_(nullptr), puppisd_corrRECO_for_(nullptr) { }

	ConstAK8Inputs(const std::vector<TLorentzVector>& jetsLVec, const std::vector<FLOATTYPE>& tau1, const std::vector<FLOATTYPE>& tau2, const std::vector<FLOATTYPE>& tau3, const std::vector<FLOATTYPE>& softDropMass, const std::vector<std::vector<TLorentzVector> >& vecSubjetsLVec, const std::vector<std::vector<FLOATTYPE> >& vecSubjetsBtag, const std::vector<std::vector<FLOATTYPE> >& vecSubjetsMult, const std::vector<std::vector<FLOATTYPE> >& vecSubjetsPtD, const std::vector<std::vector<FLOATTYPE> >& vecSubjetsAxis1, const std::vector<std::vector<FLOATTYPE> >& vecSubjetsAxis2) : ConstGenInputs(), jetsLVec_(&jetsLVec), tau1_(&tau1), tau2_(&tau2), tau3_(&tau3), deepAK8Top_(nullptr), deepAK8W_(nullptr), softDropMass_(&softDropMass), subjetsBtag_(nullptr), subjetsMult_(nullptr), subjetsPtD_(nullptr), subjetsAxis1_(nullptr), subjetsAxis2_(nullptr), subjetsLVec_(nullptr), vecSubjetsLVec_(&vecSubjetsLVec), vecSubjetsBtag_(&vecSubjetsBtag), vecSubjetsMult_(&vecSubjetsMult), vecSubjetsPtD_(&vecSubjetsPtD), vecSubjetsAxis1_(&vecSubjetsAxis1), vecSubjetsAxis2_(&vecSubjetsAxis2), puppisd_corrGEN_(nullptr), puppisd_corrRECO_cen_(nullptr), puppisd_corrRECO_for_(nullptr) { }

        void packageConstituents(std::vector<Constituent>& constituents)
        {
            //vector holding constituents to be created
            //std::vector<Constituent> constituents;

            //Safety check that jet and b-tag vectors are the same length
            if(jetsLVec_->size() != softDropMass_->size())
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
                THROW_TTEXCEPTION("Unequal subjet vector size!!!!!!!\n");
            }

            //Construct constituents in place in the vector
            for(unsigned int iJet = 0; iJet < jetsLVec_->size(); ++iJet)
            {
                // For each tagged top/W, find the corresponding subjets
                std::vector<Constituent> subjets;
                if(vecSubjetsLVec_ != nullptr)
                {	        
                    for(unsigned int iSJ = 0; iSJ < (*vecSubjetsLVec_)[iJet].size(); ++iSJ)
                    {
                        subjets.emplace_back((*vecSubjetsLVec_)[iJet][iSJ], AK8SUBJET);
                        if(vecSubjetsBtag_)  subjets.back().setBTag((*vecSubjetsBtag_)[iJet][iSJ]);
                        if(vecSubjetsMult_)  subjets.back().setExtraVar("mult",  (*vecSubjetsMult_) [iJet][iSJ]);
                        if(vecSubjetsPtD_)   subjets.back().setExtraVar("ptD",   (*vecSubjetsPtD_)  [iJet][iSJ]);
                        if(vecSubjetsAxis1_) subjets.back().setExtraVar("axis1", (*vecSubjetsAxis1_)[iJet][iSJ]);
                        if(vecSubjetsAxis2_) subjets.back().setExtraVar("axis2", (*vecSubjetsAxis2_)[iJet][iSJ]);
                    }
                } 
                else if (subjetsLVec_ != nullptr) 
                {
                    // Calculate matching subjets if a single list was given 
                    for(unsigned int iSJ = 0; iSJ < subjetsLVec_->size(); ++iSJ)
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
                if(tau1_ && tau2_ && tau3_)
                {
                    constituents.emplace_back((*jetsLVec_)[iJet], static_cast<double>((*tau1_)[iJet]), static_cast<double>((*tau2_)[iJet]), static_cast<double>((*tau3_)[iJet]), static_cast<double>((*softDropMass_)[iJet]), subjets, getPUPPIweight(static_cast<double>((*jetsLVec_)[iJet].Pt()), static_cast<double>((*jetsLVec_)[iJet].Eta())));
                }
                else
                {
                    constituents.emplace_back((*jetsLVec_)[iJet], 0.0, 0.0, 0.0, static_cast<double>((*softDropMass_)[iJet]), subjets, getPUPPIweight(static_cast<double>((*jetsLVec_)[iJet].Pt()), static_cast<double>((*jetsLVec_)[iJet].Eta())));
                }

                if(deepAK8Top_)
                {
                    constituents.back().setTopDisc((*deepAK8Top_)[iJet]);
                }

                if(deepAK8W_)
                {
                    constituents.back().setWDisc((*deepAK8W_)[iJet]);
                }

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

        std::vector<TLorentzVector> denominator(const double ptCut) const
        {
            std::vector<TLorentzVector> returnVector;
            for(auto& jet : *jetsLVec_)
            {
                if(jet.Pt() > ptCut) returnVector.push_back(jet);
            }
            return returnVector;
        }

        void setWMassCorrHistos(const std::string& fname)
        {
            TF1* puppisd_corrGEN = nullptr;
            TF1* puppisd_corrRECO_cen = nullptr;
            TF1* puppisd_corrRECO_for = nullptr;

            ConstAK8Inputs::prepHistosForWCorrectionFactors(fname, puppisd_corrGEN, puppisd_corrRECO_cen, puppisd_corrRECO_for);
            setWMassCorrHistos(puppisd_corrGEN,puppisd_corrRECO_cen, puppisd_corrRECO_for);
        }

        void setWMassCorrHistos(TF1* puppisd_corrGEN, TF1* puppisd_corrRECO_cen, TF1* puppisd_corrRECO_for)
        {
            puppisd_corrGEN_ = puppisd_corrGEN;
            puppisd_corrRECO_cen_ = puppisd_corrRECO_cen;
            puppisd_corrRECO_for_ = puppisd_corrRECO_for;
        }

        static void prepHistosForWCorrectionFactors(const std::string& fname, TF1* puppisd_corrGEN, TF1* puppisd_corrRECO_cen, TF1* puppisd_corrRECO_for)
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
    };

///////////////////////////////////////////

    /**
     *Class to gather the information necessary to construct the ResolvedTopCand Constituents
     */
    template<typename FLOATTYPE>
    class ConstResolvedCandInputs
    {
    private:
        const std::vector<TLorentzVector>* topCandLVec_;
        const std::vector<FLOATTYPE>* topCandDisc_;
        const std::vector<int>* topCandJ1_;
        const std::vector<int>* topCandJ2_;
        const std::vector<int>* topCandJ3_;

    public:
        /**
         *Basic constructor
         *@param topCandLVec Vector of lorentz vectors of resolved top candidates 
         *@param topCandDisc Vector of discriminator values for resolved top candidates
         *@param topCandJ1 Vector of jet 1 indices for resolved top candidates, referenced with respect to the AK4 constituents 
         *@param topCandJ2 Vector of jet 2 indices for resolved top candidates, referenced with respect to the AK4 constituents 
         *@param topCandJ3 Vector of jet 3 indices for resolved top candidates, referenced with respect to the AK4 constituents 
         */
        ConstResolvedCandInputs(const std::vector<TLorentzVector>& topCandLVec, const std::vector<FLOATTYPE>& topCandDisc, const std::vector<int>& topCandJ1, const std::vector<int>& topCandJ2, const std::vector<int>& topCandJ3) : topCandLVec_(&topCandLVec), topCandDisc_(&topCandDisc), topCandJ1_(&topCandJ1), topCandJ2_(&topCandJ2), topCandJ3_(&topCandJ3) {}

        /**
         *Called to fill the constituents using the information collected in the class. 
         *Not intended to be called directly.
         *@param constituents vector to insert resolved top candidate constituents into
         */
        void packageConstituents(std::vector<Constituent>& constituents)
        {
            //Check that vectors are of equal length
            if(topCandLVec_->size() != topCandDisc_->size() || topCandLVec_->size() != topCandJ1_->size() || topCandLVec_->size() != topCandJ2_->size() || topCandLVec_->size() != topCandJ3_->size())
            {
                THROW_TTEXCEPTION("Vector sizes are unequal!!!");
            }

            //Find the start of the AK4 constituents 
            unsigned int ak4Offset = 0;
            for(const auto& constituent : constituents)
            {
                if(constituent.getType() == AK4JET) break;
                ++ak4Offset;
            }

            //Fill the constituent with the necessary information 
            for(unsigned int iTop = 0; iTop < topCandLVec_->size(); ++iTop)
            {
                constituents.emplace_back((*topCandLVec_)[iTop], RESOLVEDTOPCAND);
                auto& constituent = constituents.back();
                constituent.setTopDisc((*topCandDisc_)[iTop]);
                constituent.addJetIndex(ak4Offset + (*topCandJ1_)[iTop]);
                constituent.addJetIndex(ak4Offset + (*topCandJ2_)[iTop]);
                constituent.addJetIndex(ak4Offset + (*topCandJ3_)[iTop]);
            }
        }
    };

    //Typedef for python
    typedef ConstResolvedCandInputs<float> ConstResolvedCandInputsFloat;

    //template metaprogramming magic 
    ///Resurcive function to assemble constituents from arbitrary list of input classes. Don't call this function!
    template<typename T, typename... Args> void packageConstituentsRecurse(std::vector<Constituent>& constituents, T input, Args... args)
    {
        input.packageConstituents(constituents);

        packageConstituentsRecurse(constituents, args...);
    }

    ///recursion termimnation specialization. Don't call this function!
    template<typename T> void packageConstituentsRecurse(std::vector<Constituent>& constituents, T input)
    {
        input.packageConstituents(constituents);
    }

    ///Function to fill constituent list based upon arbitrary list of input objects
    template<typename... Args> std::vector<Constituent> packageConstituents(Args... args)
    {
        //vector holding constituents to be created
        std::vector<Constituent> constituents;

        //begin the recursion to fill constituents
        packageConstituentsRecurse(constituents, args...);

        return constituents;        
    }

    ///Python compatibility function
    std::vector<Constituent> packageConstituentsAK4(ConstAK4Inputs<float>& inputs);

    ///backwards compatability overload
    std::vector<Constituent> packageConstituents(const std::vector<TLorentzVector>& jetsLVec, const std::vector<double>& btagFactors, const std::vector<double>& qgLikelihood);
    
    ///Tool to calcualte MT2 from tagger results
    double calculateMT2(const TopTaggerResults& ttr, const TLorentzVector& metLVec);

    //MVA helper functions
    /* \fn std::map<std::string, double> ttUtility::createMVAInputs(const TopObject& topCand, const double csvThresh) */
    ///Function to calculate MVA input variables from a TopObject. This function is depricated in favor of the new MVAInputCalculator based approach.
    std::map<std::string, double> createMVAInputs(const TopObject& topCand, const double csvThresh);

    //New MVA variable helper class
    /**
     *Base class for MVA input variable calculator 
     */
    class MVAInputCalculator
    {
    private:
    protected:
        float* basePtr_;
        int len_;
    public:
        /**
         *The job of mapVars is to populate the internal offests for all variables in the input variable list with their memory location in the data array.  To be called only once.
         *@param vars list of variables used for the model
         */
        virtual void mapVars(const std::vector<std::string>&) = 0;
        /**
         *The job of setPtr is to set hte starting place of memory block where the data will be written. To be called only once for the creation of the array pointed to by data.
         *@param data pointer to start of the data array which will be used as input to the MVA
         */
        virtual void setPtr(float* data) {basePtr_ = data;}
        /**
         *Calculate the requested variables and store the values directly in the input array for the MVA
         *@param topCand the top candidate to calculate the input variables for 
         */
        virtual bool calculateVars(const TopObject&, int) = 0;
        /**
         *Check if the TopObject passes basic selection for this category.
         *@param topCand the top candidate to check
         */
        virtual bool checkCand(const TopObject&) = 0;
        /**
         *Base distructor to allow cleanup of derived classes when necessary
         */
        virtual ~MVAInputCalculator() {}
    };

    /**
     *Class to calculate the input variables for the BDT based AK8 top selection
     */
    class BDTMonojetInputCalculator : public MVAInputCalculator
    {
    private:
        int ak8_sdmass_, ak8_tau21_, ak8_tau32_, ak8_ptDR_, ak8_rel_ptdiff_, ak8_csv1_mass_, ak8_csv1_csv_, ak8_csv1_ptD_, ak8_csv1_axis1_, ak8_csv1_mult_, ak8_csv2_mass_, ak8_csv2_ptD_, ak8_csv2_axis1_, ak8_csv2_mult_;
    public:
        BDTMonojetInputCalculator();
        void mapVars(const std::vector<std::string>&);
        bool calculateVars(const TopObject&, int);
        bool checkCand(const TopObject&);
    };

    /**
     *Class to calculate the input variables for the BDT based AK8 W selection
     */
    class BDTDijetInputCalculator : public MVAInputCalculator
    {
    private:
        int var_fj_sdmass_, var_fj_tau21_, var_fj_ptDR_, var_fj_rel_ptdiff_, var_sj1_ptD_, var_sj1_axis1_, var_sj1_mult_, var_sj2_ptD_, var_sj2_axis1_, var_sj2_mult_, var_sjmax_csv_, var_sd_n2_;

    public:
        BDTDijetInputCalculator();
        void mapVars(const std::vector<std::string>&);
        bool calculateVars(const TopObject&, int);
        bool checkCand(const TopObject&);
    };

    /**
     *Class to calculate the input variables for the AK4 based resolved top catagory
     */
    class TrijetInputCalculator : public MVAInputCalculator
    {
    private:
        //Nconstituents
        static constexpr int NCONST = 3;
        //Get top candidate variables
        int cand_pt_;
        int cand_p_;
        int cand_eta_;
        int cand_phi_;
        int cand_m_;
        int cand_dRMax_;
        int cand_dThetaMin_;
        int cand_dThetaMax_;

        int j_m_lab_[NCONST];
        int j_CSV_lab_[NCONST];
        int j_QGL_lab_[NCONST];
        int j_qgMult_lab_[NCONST];
        int j_qgPtD_lab_[NCONST];
        int j_qgAxis1_lab_[NCONST];
        int j_qgAxis2_lab_[NCONST];
        int j_CvsL_lab_[NCONST];
        int dR12_lab_[NCONST];
        int dR12_3_lab_[NCONST];
        int j12_m_lab_[NCONST];

        int dRPtTop_;
        int dRPtW_;
        int sd_n2_;

        int j_p_[NCONST];
        int j_p_top_[NCONST];
        int j_theta_top_[NCONST];
        int j_phi_top_[NCONST];
        int j_phi_lab_[NCONST];
        int j_eta_lab_[NCONST];
        int j_pt_lab_[NCONST];
        int j_m_[NCONST];
        int j_CSV_[NCONST];
        int j_QGL_[NCONST];
        int j_recoJetsJecScaleRawToFull_[NCONST];
        int j_qgLikelihood_[NCONST];
        int j_qgPtD_[NCONST];
        int j_qgAxis1_[NCONST];
        int j_qgAxis2_[NCONST];
        int j_recoJetschargedHadronEnergyFraction_[NCONST];
        int j_recoJetschargedEmEnergyFraction_[NCONST];
        int j_recoJetsneutralEmEnergyFraction_[NCONST];
        int j_recoJetsmuonEnergyFraction_[NCONST];
        int j_recoJetsHFHadronEnergyFraction_[NCONST];
        int j_recoJetsHFEMEnergyFraction_[NCONST];
        int j_recoJetsneutralEnergyFraction_[NCONST];
        int j_PhotonEnergyFraction_[NCONST];
        int j_ElectronEnergyFraction_[NCONST];
        int j_ChargedHadronMultiplicity_[NCONST];
        int j_NeutralHadronMultiplicity_[NCONST];
        int j_PhotonMultiplicity_[NCONST];
        int j_ElectronMultiplicity_[NCONST];
        int j_MuonMultiplicity_[NCONST];
        int j_DeepCSVb_[NCONST];
        int j_DeepCSVc_[NCONST];
        int j_DeepCSVl_[NCONST];
        int j_DeepCSVbb_[NCONST];
        int j_DeepCSVcc_[NCONST];
        int j_DeepFlavorb_[NCONST];
        int j_DeepFlavorbb_[NCONST];
        int j_DeepFlavorlepb_[NCONST];
        int j_DeepFlavorc_[NCONST];
        int j_DeepFlavoruds_[NCONST];
        int j_DeepFlavorg_[NCONST];
        int j_CvsL_[NCONST];
        int j_CvsB_[NCONST];
        int j_CombinedSvtx_[NCONST];
        int j_JetProba_[NCONST];
        int j_JetBprob_[NCONST];
        int j_recoJetsBtag_[NCONST];
        int j_recoJetsCharge_[NCONST];
        int j_qgMult_[NCONST];
        int dTheta_[NCONST];
        int j12_m_[NCONST];

    public:
        TrijetInputCalculator();
        void mapVars(const std::vector<std::string>&);
        bool calculateVars(const TopObject&, int);
        bool checkCand(const TopObject&);
    };

    std::vector<std::string> getMVAVars();

    //Gen matching helper functions 
    ///Helper function to help find the hadronically decaying gen tops 
    std::vector<TLorentzVector> GetHadTopLVec(const std::vector<TLorentzVector>& genDecayLVec, const std::vector<int>& genDecayPdgIdVec, const std::vector<int>& genDecayIdxVec, const std::vector<int>& genDecayMomIdxVec);

    ///Helper function to get the direct decay products of the gen tops
    std::vector<const TLorentzVector*> GetTopdauLVec(const TLorentzVector& top, const std::vector<TLorentzVector>& genDecayLVec, const std::vector<int>& genDecayPdgIdVec, const std::vector<int>& genDecayIdxVec, const std::vector<int>& genDecayMomIdxVec);

}

#endif
