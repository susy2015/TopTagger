#ifndef TOPTAGGERUTILITIES_H
#define TOPTAGGERUTILITIES_H

#include <vector>
#include <map>

class Constituent;
class TopObject;
class TopTaggerResults;
class TF1;

#include "TLorentzVector.h"

//This file is for utility functions which do not fit in other classes 
//DO NOT DEFINE FUNCTIONS IN LINE!!!!!!

namespace ttUtility
{
    class ConstGenInputs
    {
    protected:
        const std::vector<TLorentzVector>* hadGenTops_;
        const std::vector<std::vector<const TLorentzVector*>>* hadGenTopDaughters_;

        ConstGenInputs();
        ConstGenInputs(const std::vector<TLorentzVector>& hadGenTops, const std::vector<std::vector<const TLorentzVector*>>& hadGenTopDaughters);
    };

    class ConstAK4Inputs : public ConstGenInputs
    {
    private:
        const std::vector<TLorentzVector>* jetsLVec_;
        const std::vector<double>* btagFactors_;
        const std::vector<double>* qgLikelihood_;
        const std::vector<int>* qgMult_;
        const std::vector<double>* qgPtD_;
        const std::vector<double>* qgAxis1_;
        const std::vector<double>* qgAxis2_;

        std::map<std::string, const std::vector<double>*> extraInputVariables_;

    public:
        ConstAK4Inputs(const std::vector<TLorentzVector>& jetsLVec, const std::vector<double>& btagFactors, const std::vector<double>& qgLikelihood);
        ConstAK4Inputs(const std::vector<TLorentzVector>& jetsLVec, const std::vector<double>& btagFactors);
        ConstAK4Inputs(const std::vector<TLorentzVector>& jetsLVec, const std::vector<double>& btagFactors, const std::vector<double>& qgLikelihood, const std::vector<TLorentzVector>& hadGenTops, const std::vector<std::vector<const TLorentzVector*>>& hadGenTopDaughters);
        void addQGLVectors(const std::vector<int>& qgMult, const std::vector<double>& qgPtD, const std::vector<double>& qgAxis1, const std::vector<double>& qgAxis2);
        void addSupplamentalVector(const std::string& name, const std::vector<double>& vector);
        void packageConstituents(std::vector<Constituent>& constituents);
    };

    class ConstAK8Inputs : public ConstGenInputs
    {
    private:
        const std::vector<TLorentzVector>* jetsLVec_;
        const std::vector<double>* tau1_;
        const std::vector<double>* tau2_;
        const std::vector<double>* tau3_;
        const std::vector<double>* softDropMass_;
        const std::vector<double>* subjetsBtag_;
        const std::vector<double>* subjetsMult_;
        const std::vector<double>* subjetsPtD_;
        const std::vector<double>* subjetsAxis1_;
        const std::vector<double>* subjetsAxis2_;
        const std::vector<TLorentzVector>* subjetsLVec_;
        const std::vector<std::vector<TLorentzVector>>* vecSubjetsLVec_;
        TF1* puppisd_corrGEN_;
        TF1* puppisd_corrRECO_cen_;
        TF1* puppisd_corrRECO_for_;
        
        double getPUPPIweight(double puppipt, double puppieta ) const;

    public:
        ConstAK8Inputs(const std::vector<TLorentzVector>& jetsLVec, const std::vector<double>& tau1, const std::vector<double>& tau2, const std::vector<double>& tau3, const std::vector<double>& softDropMass, const std::vector<TLorentzVector>& subjetsLVec);
        ConstAK8Inputs(const std::vector<TLorentzVector>& jetsLVec, const std::vector<double>& tau1, const std::vector<double>& tau2, const std::vector<double>& tau3, const std::vector<double>& softDropMass, const std::vector<TLorentzVector>& subjetsLVec, const std::vector<double>& subjetsBtag, const std::vector<double>& subjetsMult, const std::vector<double>& subjetsPtD, const std::vector<double>& subjetsAxis1, const std::vector<double>& subjetsAxis2);
        ConstAK8Inputs(const std::vector<TLorentzVector>& jetsLVec, const std::vector<double>& tau1, const std::vector<double>& tau2, const std::vector<double>& tau3, const std::vector<double>& softDropMass, const std::vector<std::vector<TLorentzVector> >& vecSubJetsLVec);
        ConstAK8Inputs(const std::vector<TLorentzVector>& jetsLVec, const std::vector<double>& tau1, const std::vector<double>& tau2, const std::vector<double>& tau3, const std::vector<double>& softDropMass, const std::vector<TLorentzVector>& subjetsLVec, const std::vector<TLorentzVector>& hadGenTops, const std::vector<std::vector<const TLorentzVector*>>& hadGenTopDaughters);
        ConstAK8Inputs(const std::vector<TLorentzVector>& jetsLVec, const std::vector<double>& tau1, const std::vector<double>& tau2, const std::vector<double>& tau3, const std::vector<double>& softDropMass, const std::vector<TLorentzVector>& subjetsLVec, const std::vector<double>& subjetsBtag, const std::vector<double>& subjetsMult, const std::vector<double>& subjetsPtD, const std::vector<double>& subjetsAxis1, const std::vector<double>& subjetsAxis2, const std::vector<TLorentzVector>& hadGenTops, const std::vector<std::vector<const TLorentzVector*>>& hadGenTopDaughters);
        ConstAK8Inputs(const std::vector<TLorentzVector>& jetsLVec, const std::vector<double>& tau1, const std::vector<double>& tau2, const std::vector<double>& tau3, const std::vector<double>& softDropMass, const std::vector<std::vector<TLorentzVector> >& vecSubJetsLVec, const std::vector<TLorentzVector>& hadGenTops, const std::vector<std::vector<const TLorentzVector*>>& hadGenTopDaughters);
        void packageConstituents(std::vector<Constituent>& constituents);
        std::vector<TLorentzVector> denominator(const double ptCut) const;
        void setWMassCorrHistos(const std::string& fname);
        void setWMassCorrHistos(TF1* puppisd_corrGEN, TF1* puppisd_corrRECO_cen, TF1* puppisd_corrRECO_for);

        static void prepHistosForWCorrectionFactors(const std::string& fname, TF1* puppisd_corrGEN, TF1* puppisd_corrRECO_cen, TF1* puppisd_corrRECO_for);
    };

    //template metaprogramming magic 
    template<typename T, typename... Args> void packageConstituentsRecurse(std::vector<Constituent>& constituents, T input, Args... args)
    {
        input.packageConstituents(constituents);

        packageConstituentsRecurse(constituents, args...);
    }

    //recursion termimnation specialization
    template<typename T> void packageConstituentsRecurse(std::vector<Constituent>& constituents, T input)
    {
        input.packageConstituents(constituents);
    }

    //Function to fill constituent list based upon arbitrary list of input onjects
    template<typename... Args> std::vector<Constituent> packageConstituents(Args... args)
    {
        //vector holding constituents to be created
        std::vector<Constituent> constituents;

        //begin the recursion to fill constituents
        packageConstituentsRecurse(constituents, args...);

        return constituents;        
    }

    //backwards compatability overload
    std::vector<Constituent> packageConstituents(const std::vector<TLorentzVector>& jetsLVec, const std::vector<double>& btagFactors, const std::vector<double>& qgLikelihood);
    
    //Tool to calcualte MT2 from tagger results
    double calculateMT2(const TopTaggerResults& ttr, const TLorentzVector& metLVec);

    //MVA helper functions
    std::map<std::string, double> createMVAInputs(const TopObject& topCand, const double csvThresh);

    //New MVA variable helper class
    class MVAInputCalculator
    {
    private:
    public:
        virtual void mapVars(const std::vector<std::string>&, float*) = 0;
        virtual bool calculateVars(const TopObject&) = 0;
    };

    class BDTMonojetInputCalculator : public MVAInputCalculator
    {
    private:
        float *ak8_sdmass_, *ak8_tau21_, *ak8_tau32_, *ak8_ptDR_, *ak8_rel_ptdiff_, *ak8_csv1_mass_, *ak8_csv1_csv_, *ak8_csv1_ptD_, *ak8_csv1_axis1_, *ak8_csv1_mult_, *ak8_csv2_mass_, *ak8_csv2_ptD_, *ak8_csv2_axis1_, *ak8_csv2_mult_;
    public:
        BDTMonojetInputCalculator();
        void mapVars(const std::vector<std::string>&, float *);
        bool calculateVars(const TopObject&);
    };

    class BDTDijetInputCalculator : public MVAInputCalculator
    {
    private:
        float *var_fj_sdmass_, *var_fj_tau21_, *var_fj_ptDR_, *var_fj_rel_ptdiff_, *var_sj1_ptD_, *var_sj1_axis1_, *var_sj1_mult_, *var_sj2_ptD_, *var_sj2_axis1_, *var_sj2_mult_, *var_sjmax_csv_, *var_sd_n2_;

    public:
        BDTDijetInputCalculator();
        void mapVars(const std::vector<std::string>&, float *);
        bool calculateVars(const TopObject&);
    };

    class TrijetInputCalculator : public MVAInputCalculator
    {
    private:
        //Nconstituents
        static constexpr int NCONST = 3;
        //Get top candidate variables
        float* cand_pt_;
        float* cand_p_;
        float* cand_eta_;
        float* cand_phi_;
        float* cand_m_;
        float* cand_dRMax_;
        float* cand_dThetaMin_;
        float* cand_dThetaMax_;

        float* j_m_lab_[NCONST];
        float* j_CSV_lab_[NCONST];
        float* j_QGL_lab_[NCONST];
        float* j_qgMult_lab_[NCONST];
        float* j_qgPtD_lab_[NCONST];
        float* j_qgAxis1_lab_[NCONST];
        float* j_qgAxis2_lab_[NCONST];
        float* dR12_lab_[NCONST];
        float* dR12_3_lab_[NCONST];
        float* j12_m_lab_[NCONST];

        float* dRPtTop_;
        float* dRPtW_;
        float* sd_n2_;

        float* j_p_[NCONST];
        float* j_p_top_[NCONST];
        float* j_theta_top_[NCONST];
        float* j_phi_top_[NCONST];
        float* j_phi_lab_[NCONST];
        float* j_eta_lab_[NCONST];
        float* j_pt_lab_[NCONST];
        float* j_m_[NCONST];
        float* j_CSV_[NCONST];
        float* j_QGL_[NCONST];
        float* j_recoJetsJecScaleRawToFull_[NCONST];
        float* j_qgLikelihood_[NCONST];
        float* j_qgPtD_[NCONST];
        float* j_qgAxis1_[NCONST];
        float* j_qgAxis2_[NCONST];
        float* j_recoJetschargedHadronEnergyFraction_[NCONST];
        float* j_recoJetschargedEmEnergyFraction_[NCONST];
        float* j_recoJetsneutralEmEnergyFraction_[NCONST];
        float* j_recoJetsmuonEnergyFraction_[NCONST];
        float* j_recoJetsHFHadronEnergyFraction_[NCONST];
        float* j_recoJetsHFEMEnergyFraction_[NCONST];
        float* j_recoJetsneutralEnergyFraction_[NCONST];
        float* j_PhotonEnergyFraction_[NCONST];
        float* j_ElectronEnergyFraction_[NCONST];
        float* j_ChargedHadronMultiplicity_[NCONST];
        float* j_NeutralHadronMultiplicity_[NCONST];
        float* j_PhotonMultiplicity_[NCONST];
        float* j_ElectronMultiplicity_[NCONST];
        float* j_MuonMultiplicity_[NCONST];
        float* j_DeepCSVb_[NCONST];
        float* j_DeepCSVc_[NCONST];
        float* j_DeepCSVl_[NCONST];
        float* j_DeepCSVbb_[NCONST];
        float* j_DeepCSVcc_[NCONST];
        float* j_DeepFlavorb_[NCONST];
        float* j_DeepFlavorbb_[NCONST];
        float* j_DeepFlavorlepb_[NCONST];
        float* j_DeepFlavorc_[NCONST];
        float* j_DeepFlavoruds_[NCONST];
        float* j_DeepFlavorg_[NCONST];
        float* j_CvsL_[NCONST];
        float* j_CvsB_[NCONST];
        float* j_CombinedSvtx_[NCONST];
        float* j_JetProba_[NCONST];
        float* j_JetBprob_[NCONST];
        float* j_recoJetsBtag_[NCONST];
        float* j_recoJetsCharge_[NCONST];
        float* j_qgMult_[NCONST];
        float* dTheta_[NCONST];
        float* j12_m_[NCONST];

    public:
        TrijetInputCalculator();
        void mapVars(const std::vector<std::string>&, float *);
        bool calculateVars(const TopObject&);
    };

    std::vector<std::string> getMVAVars();

    //Gen matching helper functions 
    std::vector<TLorentzVector> GetHadTopLVec(const std::vector<TLorentzVector>& genDecayLVec, const std::vector<int>& genDecayPdgIdVec, const std::vector<int>& genDecayIdxVec, const std::vector<int>& genDecayMomIdxVec);

    std::vector<const TLorentzVector*> GetTopdauLVec(const TLorentzVector& top, const std::vector<TLorentzVector>& genDecayLVec, const std::vector<int>& genDecayPdgIdVec, const std::vector<int>& genDecayIdxVec, const std::vector<int>& genDecayMomIdxVec);

}

#endif
