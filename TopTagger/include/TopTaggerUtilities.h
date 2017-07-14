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

        const std::vector<double>* recoJetschargedHadronEnergyFraction_;
        const std::vector<double>* recoJetschargedEmEnergyFraction_;
        const std::vector<double>* recoJetsneutralEmEnergyFraction_;
        const std::vector<double>* recoJetsmuonEnergyFraction_;
        const std::vector<double>* PhotonEnergyFraction_;
        const std::vector<double>* ElectronEnergyFraction_;
        const std::vector<double>* ChargedHadronMultiplicity_;
        const std::vector<double>* NeutralHadronMultiplicity_;
        const std::vector<double>* PhotonMultiplicity_;
        const std::vector<double>* ElectronMultiplicity_;
        const std::vector<double>* MuonMultiplicity_;
        const std::vector<double>* recoJetsCharge_0_;

    public:
        ConstAK4Inputs(const std::vector<TLorentzVector>& jetsLVec, const std::vector<double>& btagFactors, const std::vector<double>& qgLikelihood);
        ConstAK4Inputs(const std::vector<TLorentzVector>& jetsLVec, const std::vector<double>& btagFactors);
        ConstAK4Inputs(const std::vector<TLorentzVector>& jetsLVec, const std::vector<double>& btagFactors, const std::vector<double>& qgLikelihood, const std::vector<TLorentzVector>& hadGenTops, const std::vector<std::vector<const TLorentzVector*>>& hadGenTopDaughters);
        void addQGLVectors(const std::vector<int>& qgMult, const std::vector<double>& qgPtD, const std::vector<double>& qgAxis1, const std::vector<double>& qgAxis2);
        void addSupplamentalVectors(const std::vector<double>& recoJetschargedHadronEnergyFraction,
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
                                    const std::vector<double>& recoJetsCharge_0);
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
        const std::vector<TLorentzVector>* subjetsLVec_;
        TF1* puppisd_corrGEN_;
        TF1* puppisd_corrRECO_cen_;
        TF1* puppisd_corrRECO_for_;
        
        double getPUPPIweight(double puppipt, double puppieta ) const;

    public:
        ConstAK8Inputs(const std::vector<TLorentzVector>& jetsLVec, const std::vector<double>& tau1, const std::vector<double>& tau2, const std::vector<double>& tau3, const std::vector<double>& softDropMass, const std::vector<TLorentzVector>& subJetsLVec);
        ConstAK8Inputs(const std::vector<TLorentzVector>& jetsLVec, const std::vector<double>& tau1, const std::vector<double>& tau2, const std::vector<double>& tau3, const std::vector<double>& softDropMass, const std::vector<TLorentzVector>& subJetsLVec, const std::vector<TLorentzVector>& hadGenTops, const std::vector<std::vector<const TLorentzVector*>>& hadGenTopDaughters);
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
    double calculateMT2(const TopTaggerResults& ttr);

    //MVA helper functions
    std::map<std::string, double> createMVAInputs(const TopObject& topCand, const double csvThresh);

    std::vector<std::string> getMVAVars();

    //Gen matching helper functions 
    std::vector<TLorentzVector> GetHadTopLVec(const std::vector<TLorentzVector>& genDecayLVec, const std::vector<int>& genDecayPdgIdVec, const std::vector<int>& genDecayIdxVec, const std::vector<int>& genDecayMomIdxVec);

    std::vector<const TLorentzVector*> GetTopdauLVec(const TLorentzVector& top, const std::vector<TLorentzVector>& genDecayLVec, const std::vector<int>& genDecayPdgIdVec, const std::vector<int>& genDecayIdxVec, const std::vector<int>& genDecayMomIdxVec);

}

#endif
