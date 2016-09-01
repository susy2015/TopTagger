#ifndef TAGGERUTILITY_H
#define TAGGERUTILITY_H

#include <iostream>
#include "TLorentzVector.h"
#include "TopTagger/TopTagger/include/TopTagger.h"
#include "TopTagger/TopTagger/include/TopTaggerResults.h"
#include "TopTagger/TopTagger/include/TopTaggerUtilities.h"
#include "PlotUtility.h"
class TopVar
{
 private:
  TopObject* Top_;
  TopObject TopCand_;
  std::vector<Constituent const*> con;

 public:

  double m12, m23, m13, m123;

  TLorentzVector GetTopLVec(TopObject* Top){return Top->p();};
  TLorentzVector GetTopLVec(TopObject TopCand){return TopCand.p();};
  double GetTopPt(TopObject* Top){return Top->p().Pt();};
  double GetTopPt(TopObject TopCand){return TopCand.p().Pt();};
  double GetTopMass(TopObject* Top){return Top->p().M();};
  double GetTopMass(TopObject TopCand){return TopCand.p().M();};
  
  double GetTopdRmax(TopObject* Top){return Top->getDRmax();};
  double GetTopdRmax(TopObject TopCand){return TopCand.getDRmax();};
  double GetTopdRmin(TopObject* Top);
  double GetTopdRmin(TopObject TopCand);
  double GetArea(TopObject* Top);
  double GetArea(TopObject TopCand);
  void CalCombmass(TopObject* Top);
  void CalCombmass(TopObject TopCand);
};

class TopCat
{
 public:
  bool GetMatchedTop(std::vector<TopObject*> Top, std::vector<TopObject*> &MachedTop, std::vector<TLorentzVector>Gentop, std::vector<TLorentzVector> &MGentop); 
  bool GetMatchedTop(std::vector<TopObject> TopCand, std::vector<TopObject> &MachedTopCand, std::vector<TLorentzVector>Gentop, std::vector<TLorentzVector> &MGentop); 
  bool GetMatchedTop(std::vector<TLorentzVector> TopCand, std::vector<TLorentzVector> &MachedTopCand, std::vector<TLorentzVector>Gentop, std::vector<TLorentzVector> &MGentop); 
  int GetMatchedTopConst(std::vector<Constituent const *> topconst, std::vector<TLorentzVector>gentopdau);

  std::pair<std::vector<int>*, std::vector<int>*> TopConst(std::vector<TopObject> tops, std::vector<TLorentzVector>genDecayLVec, std::vector<int>genDecayPdgIdVec, std::vector<int>genDecayIdxVec, std::vector<int>genDecayMomIdxVec);
};


#endif
