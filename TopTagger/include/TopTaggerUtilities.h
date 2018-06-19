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
        const std::vector<float>* btagFactors_;
        const std::vector<float>* qgLikelihood_;

    public:
        ConstAK4Inputs(const std::vector<TLorentzVector>& jetsLVec, const std::vector<float>& btagFactors, const std::vector<float>& qgLikelihood);
        ConstAK4Inputs(const std::vector<TLorentzVector>& jetsLVec, const std::vector<float>& btagFactors, const std::vector<float>& qgLikelihood, const std::vector<TLorentzVector>& hadGenTops, const std::vector<std::vector<const TLorentzVector*>>& hadGenTopDaughters);
        void packageConstituents(std::vector<Constituent>& constituents);
    };

    class ConstAK8Inputs : public ConstGenInputs
    {
    private:
        const std::vector<TLorentzVector>* jetsLVec_;
        const std::vector<float>* tau1_;
        const std::vector<float>* tau2_;
        const std::vector<float>* tau3_;
        const std::vector<float>* softDropMass_;
        const std::vector<TLorentzVector>* subjetsLVec_;
        TF1* puppisd_corrGEN_;
        TF1* puppisd_corrRECO_cen_;
        TF1* puppisd_corrRECO_for_;
        
        float getPUPPIweight(float puppipt, float puppieta ) const;

    public:
        ConstAK8Inputs(const std::vector<TLorentzVector>& jetsLVec, const std::vector<float>& tau1, const std::vector<float>& tau2, const std::vector<float>& tau3, const std::vector<float>& softDropMass, const std::vector<TLorentzVector>& subJetsLVec);
        ConstAK8Inputs(const std::vector<TLorentzVector>& jetsLVec, const std::vector<float>& tau1, const std::vector<float>& tau2, const std::vector<float>& tau3, const std::vector<float>& softDropMass, const std::vector<TLorentzVector>& subJetsLVec, const std::vector<TLorentzVector>& hadGenTops, const std::vector<std::vector<const TLorentzVector*>>& hadGenTopDaughters);
        void packageConstituents(std::vector<Constituent>& constituents);
        std::vector<TLorentzVector> denominator(const float ptCut) const;
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
    std::vector<Constituent> packageConstituents(const std::vector<TLorentzVector>& jetsLVec, const std::vector<float>& btagFactors, const std::vector<float>& qgLikelihood);
    
    //Tool to calcualte MT2 from tagger results
    float calculateMT2(const TopTaggerResults& ttr);

    //MVA helper functions
    std::map<std::string, float> createMVAInputs(const TopObject& topCand, const float csvThresh);

    std::vector<std::string> getMVAVars();

    //Gen matching helper functions 
    std::vector<TLorentzVector> GetHadTopLVec(const std::vector<TLorentzVector>& genDecayLVec, const std::vector<int>& genDecayPdgIdVec, const std::vector<int>& genDecayIdxVec, const std::vector<int>& genDecayMomIdxVec);

    std::vector<const TLorentzVector*> GetTopdauLVec(const TLorentzVector& top, const std::vector<TLorentzVector>& genDecayLVec, const std::vector<int>& genDecayPdgIdVec, const std::vector<int>& genDecayIdxVec, const std::vector<int>& genDecayMomIdxVec);

}

#endif
