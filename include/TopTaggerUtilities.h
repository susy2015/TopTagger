#ifndef TOPTAGGERUTILITIES_H
#define TOPTAGGERUTILITIES_H

#include <vector>

class Constituent;

#include "TLorentzVector.h"

//This file is for utility functions which do not fit in other classes 
//DO NOT DEFINE FUNCTIONS IN LINE!!!!!!

namespace ttUtility
{
    const std::vector<Constituent> packageCandidates(const std::vector<TLorentzVector>& jetsLVec, const std::vector<double>& btagFactors);
}

#endif
