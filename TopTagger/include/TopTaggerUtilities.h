#ifndef TOPTAGGERUTILITIES_H
#define TOPTAGGERUTILITIES_H

#include <vector>

class Constituent;
class TopTaggerResults;

#include "TLorentzVector.h"

//This file is for utility functions which do not fit in other classes 
//DO NOT DEFINE FUNCTIONS IN LINE!!!!!!

namespace ttUtility
{
  std::vector<Constituent> packageConstituents(const std::vector<TLorentzVector>& jetsLVec, const std::vector<double>& btagFactors, const std::vector<double>& qgLikelihood, const std::vector<double>& jetChrg);

    double calculateMT2(const TopTaggerResults& ttr);
}

#endif
