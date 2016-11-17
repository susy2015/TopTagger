#ifndef TAGGERUTILITY_H
#define TAGGERUTILITY_H

#include "TLorentzVector.h"
#include "TopTagger/TopTagger/include/TopTagger.h"
#include "TopTagger/TopTagger/include/TopTaggerResults.h"
#include "TopTagger/TopTagger/include/TopTaggerUtilities.h"
#include "PlotUtility.h"

#include <utility>

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
private:

    template<typename T> inline const T pointerDeref(T obj) const
    {
        return obj;
    }

    template<typename T> inline const T& pointerDeref(T* const obj) const
    {
        return *obj;
    }

public:
    //bool GetMatchedTop(std::vector<TopObject*> Top, std::vector<TopObject*> &MachedTop, std::vector<TLorentzVector>Gentop, std::vector<TLorentzVector> &MGentop); 
    template<typename T> bool GetMatchedTop(const std::vector<T>& TopCand, std::vector<TopObject>& MachedTopCand, const std::vector<TLorentzVector>& Gentop, std::vector<TLorentzVector>& MGentop)
    {
        bool match = false; 
        if(Gentop.size()==0) return match;
        double DeltaR = 0.4;
        for(unsigned nt=0; nt<TopCand.size(); nt++)
        {
            double deltaRMin = 100000.;
            unsigned tid = -1;
            for(unsigned gent = 0; gent < Gentop.size(); gent++)
            { // Loop over objects
                const double dr = pointerDeref(TopCand[nt]).p().DeltaR(Gentop.at(gent));
                if( dr < deltaRMin ) 
                {
                    deltaRMin = dr;
                    tid = gent;
                }
            }
            if(deltaRMin < DeltaR)
            {
                MachedTopCand.push_back(pointerDeref(TopCand[nt]));
                MGentop.push_back(Gentop[tid]);
                match = true;
            }
        }
        return match;
    }

    bool GetMatchedTop(std::vector<TLorentzVector> TopCand, std::vector<TLorentzVector> &MachedTopCand, std::vector<TLorentzVector>Gentop, std::vector<TLorentzVector> &MGentop); 
    std::pair<int, int> GetMatchedTopConst(const std::vector<Constituent const *>& topconst, const std::vector<TLorentzVector>& gentopdau);

    template<typename T> std::pair<std::vector<int>, std::pair<std::vector<int>, std::vector<TLorentzVector>>> TopConst(const std::vector<T>& tops, const std::vector<TLorentzVector>& genDecayLVec, const std::vector<int>& genDecayPdgIdVec, const std::vector<int>& genDecayIdxVec, const std::vector<int>& genDecayMomIdxVec)
    {
        //Check matching between top candidates and gen top
        std::vector<int> topMatch;// = new std::vector<int>();
        std::vector<TLorentzVector> hadtopLVec = genUtility::GetHadTopLVec(genDecayLVec, genDecayPdgIdVec, genDecayIdxVec, genDecayMomIdxVec);
        std::vector<TLorentzVector> MatchGentop;
        std::vector<TopObject> matchTops;
        bool topmatch = GetMatchedTop(tops, matchTops, hadtopLVec, MatchGentop);//final top match 

        //check matching between reco top constituents and top gen decay daughters 
        std::vector<int> topConstMatch;// = new std::vector<int>();
        std::vector<TLorentzVector> constMatchGen;// = new std::vector<double>();
        int iMatch = 0;
        for(const auto& protoTop : tops)
        {
            const auto& top = pointerDeref(protoTop);
            const std::vector<Constituent const*>& topConst = top.getConstituents();

            //wrong horrible terrible bad inefficient hack to set reco top to gen top matching vector, please replace me!
            if(topConst.size() && iMatch < matchTops.size() && matchTops[iMatch].getConstituents().size() && (topConst[0] == (matchTops[iMatch].getConstituents())[0]))
            {
                topMatch.push_back(1);
                ++iMatch;
            }
            else topMatch.push_back(0);

            int bestMatches = 0;
            const TLorentzVector* genTopPtr = nullptr;
            for(const auto& genTop : hadtopLVec)
            {
                std::vector<TLorentzVector> gentopdauLVec = genUtility::GetTopdauLVec(genTop, genDecayLVec, genDecayPdgIdVec, genDecayIdxVec, genDecayMomIdxVec);
                auto matches = GetMatchedTopConst(topConst, gentopdauLVec);
                if(topConst.size() >= matches.second && matches.first == matches.second)
                {
                    if(matches.first > bestMatches)
                    {
                        bestMatches = matches.first;
                        genTopPtr = &genTop;
                    }
                }
            }
            topConstMatch.push_back(bestMatches);
            if(genTopPtr) constMatchGen.push_back(*genTopPtr);
            else          constMatchGen.push_back(TLorentzVector());
        }
        return std::make_pair(topMatch, std::make_pair(topConstMatch, constMatchGen));
    }

};


#endif
