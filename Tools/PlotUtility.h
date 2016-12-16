#ifndef PLOTTAGGERUTILITY_H
#define PLOTTAGGERUTILITY_H

#include <iostream>
#include "TH1.h"
#include "TH2.h"
#include "TLorentzVector.h"
#include "TVector3.h"

namespace pUtility {

  void FillDouble(TH1* hist, const double &a, const double &w);
  void FillInt(TH1* hist, const int &a, const double &w);
  void Fill2D(TH2 *hist, const double &a, const double &b, const double &w);
}

namespace genUtility {
    std::vector<TLorentzVector> GetHadTopLVec(const std::vector<TLorentzVector>& genDecayLVec, const std::vector<int>& genDecayPdgIdVec, const std::vector<int>& genDecayIdxVec, const std::vector<int>& genDecayMomIdxVec);
    std::vector<TLorentzVector> GetTopdauLVec(TLorentzVector top, std::vector<TLorentzVector>genDecayLVec, std::vector<int>genDecayPdgIdVec, std::vector<int>genDecayIdxVec, std::vector<int>genDecayMomIdxVec);
    std::vector<TLorentzVector> GetHadWLVec(const std::vector<TLorentzVector>& genDecayLVec, const std::vector<int>& genDecayPdgIdVec, const std::vector<int>& genDecayIdxVec, const std::vector<int>& genDecayMomIdxVec);
    std::vector<TLorentzVector> GetWdauLVec(TLorentzVector W, std::vector<TLorentzVector>genDecayLVec, std::vector<int>genDecayPdgIdVec, std::vector<int>genDecayIdxVec, std::vector<int>genDecayMomIdxVec);
}
#endif
