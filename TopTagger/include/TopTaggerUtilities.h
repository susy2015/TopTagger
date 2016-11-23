#ifndef TOPTAGGERUTILITIES_H
#define TOPTAGGERUTILITIES_H

#include <vector>
#include <map>

class Constituent;
class TopObject;
class TopTaggerResults;

#include "TLorentzVector.h"

//This file is for utility functions which do not fit in other classes 
//DO NOT DEFINE FUNCTIONS IN LINE!!!!!!

namespace ttUtility
{
    class ConstAK4Inputs
    {
    private:
        const std::vector<TLorentzVector>* jetsLVec_;
        const std::vector<double>* btagFactors_;
        const std::vector<double>* qgLikelihood_;

    public:
        ConstAK4Inputs(const std::vector<TLorentzVector>& jetsLVec, const std::vector<double>& btagFactors, const std::vector<double>& qgLikelihood);
        void packageConstituents(std::vector<Constituent>& constituents);
    };

    class ConstAK8Inputs
    {
    private:
        const std::vector<TLorentzVector>* jetsLVec_;
        const std::vector<double>* tau1_;
        const std::vector<double>* tau2_;
        const std::vector<double>* tau3_;
        const std::vector<double>* softDropMass_;
        const std::vector<TLorentzVector>* subjetsLVec_;
        
    public:
        ConstAK8Inputs(const std::vector<TLorentzVector>& jetsLVec, const std::vector<double>& tau1, const std::vector<double>& tau2, const std::vector<double>& tau3, const std::vector<double>& softDropMass, const std::vector<TLorentzVector>& subJetsLVec);
        void packageConstituents(std::vector<Constituent>& constituents);
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
    
    double calculateMT2(const TopTaggerResults& ttr);

    std::map<std::string, double> createMVAInputs(const TopObject& topCand);

    std::vector<std::string> getMVAVars();
}

#endif
