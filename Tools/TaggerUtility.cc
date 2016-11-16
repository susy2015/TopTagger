#include "TaggerUtility.h"

#include <set>

using namespace std;

double TopVar::GetTopdRmin(TopObject* Top){
  con = Top->getConstituents();
  double dRmin = 999;
  for(unsigned dr =0; dr<con.size(); dr++){
    double dR = con[dr]->p().DeltaR(Top->p());
    if(dR<dRmin) dRmin = dR;
  }
  return dRmin;
}
double TopVar::GetTopdRmin(TopObject Top){
  con = Top.getConstituents();
  double dRmin = 999;
  for(unsigned dr =0; dr<con.size(); dr++){
    double dR = con[dr]->p().DeltaR(Top.p());
    if(dR<dRmin) dRmin = dR;
  }
  return dRmin;
}
double TopVar::GetArea(TopObject* Top){
  con = Top->getConstituents();
  vector<double>dR(con.size(),0);
  double area = 0;
  if(con.size()<3)return area;
  dR[0] = con[0]->p().DeltaR(con[1]->p());
  dR[1] = con[0]->p().DeltaR(con[2]->p());
  dR[2] = con[1]->p().DeltaR(con[2]->p());
  double s = (dR[0]+dR[1]+dR[2])/2;
  area = sqrt(s*(s-dR[0])*(s-dR[2])*(s-dR[2]));
  return area;
}
double TopVar::GetArea(TopObject Top){
  con = Top.getConstituents();
  vector<double>dR(con.size(),0);
  double area = 0;
  if(con.size()<3)return area;
  dR[0] = con[0]->p().DeltaR(con[1]->p());
  dR[1] = con[0]->p().DeltaR(con[2]->p());
  dR[2] = con[1]->p().DeltaR(con[2]->p());
  double s = (dR[0]+dR[1]+dR[2])/2;
  area = sqrt(s*(s-dR[0])*(s-dR[2])*(s-dR[2]));
  return area;
}
void TopVar::CalCombmass(TopObject* Top){
  con = Top->getConstituents();
  if(con.size()==1){
    m12=0;
    m23=0;
    m13=0;
    m123=con[0]->p().M();
  }
  if(con.size()==2){
    m12=0;
    m23=0;
    m13=0;
  m123 =(con[0]->p() + con[1]->p()).M();
  }
  if(con.size()==3){
    m12=(con[0]->p() + con[1]->p()).M();
    m23=(con[1]->p() + con[2]->p()).M();
    m13=(con[0]->p() + con[2]->p()).M();
    m123 =(con[0]->p() + con[1]->p() + con[2]->p()).M();
  }
}
void TopVar::CalCombmass(TopObject Top){
  con = Top.getConstituents();
  if(con.size()==1){
    m12=0;
    m23=0;
    m13=0;
    m123=con[0]->p().M();
  }
  if(con.size()==2){
    m12=0;
    m23=0;
    m13=0;
    m123 =(con[0]->p() + con[1]->p()).M();
  }
  if(con.size()==3){
    m12=(con[0]->p() + con[1]->p()).M();
    m23=(con[1]->p() + con[2]->p()).M();
    m13=(con[0]->p() + con[2]->p()).M();
    m123 =(con[0]->p() + con[1]->p() + con[2]->p()).M();
  }
}
//bool TopCat::GetMatchedTop(vector<TopObject*> Top, vector<TopObject*> &MachedTop, vector<TLorentzVector>Gentop, vector<TLorentzVector> &MGentop){
//  bool match = false;
//  if(Gentop.size()==0) return match;
//  double DeltaR = 0.4;
//  for(unsigned nt=0; nt<Top.size();nt++){
//    double deltaRMin = 100000.;
//    unsigned tid = -1;
//    for(unsigned gent = 0; gent < Gentop.size(); gent++) { // Loop over objects
//      const double dr = Top[nt]->p().DeltaR(Gentop.at(gent));
//      if( dr < deltaRMin ) {deltaRMin = dr; tid = gent;}
//    }
//    if(deltaRMin < DeltaR){
//      MachedTop.push_back(Top[nt]);
//      MGentop.push_back(Gentop[tid]);
//      match = true;
//    }
//  }
//  return match;
//}
//
//bool TopCat::GetMatchedTop(vector<TopObject> TopCand, vector<TopObject> &MachedTopCand, vector<TLorentzVector>Gentop, vector<TLorentzVector> &MGentop){
//  bool match = false;
//  if(Gentop.size()==0) return match;
//  double DeltaR = 0.4;
//  for(unsigned nt=0; nt<TopCand.size();nt++){
//    double deltaRMin = 100000.;
//    unsigned tid = -1;
//    for(unsigned gent = 0; gent < Gentop.size(); gent++) { // Loop over objects
//      const double dr = TopCand[nt].p().DeltaR(Gentop.at(gent));
//      if( dr < deltaRMin ) {deltaRMin = dr; tid = gent;}
//    }
//    if(deltaRMin < DeltaR){
//      MachedTopCand.push_back(TopCand[nt]);
//      MGentop.push_back(Gentop[tid]);
//      match = true;
//    }
//  }
//  return match;
//}
bool TopCat::GetMatchedTop(vector<TLorentzVector> Top, vector<TLorentzVector> &MachedTop, vector<TLorentzVector>Gentop, vector<TLorentzVector> &MGentop){
  bool match = false;
  if(Gentop.size()==0) return match;
  double DeltaR = 0.4;
  for(unsigned nt=0; nt<Top.size();nt++){
    double deltaRMin = 100000.;
    unsigned tid = -1;
    for(unsigned gent = 0; gent < Gentop.size(); gent++) { // Loop over objects
      const double dr = Top[nt].DeltaR(Gentop.at(gent));
      if( dr < deltaRMin ) {deltaRMin = dr; tid = gent;}
    }
    if(deltaRMin < DeltaR){
      MachedTop.push_back(Top[nt]);
      MGentop.push_back(Gentop[tid]);
      match = true;
    }
  }
  return match;
}

std::pair<int, int> TopCat::GetMatchedTopConst(const vector<Constituent const *>& topconst, const vector<TLorentzVector>& gentopdau)
{
    int match=0;
    if(gentopdau.size()==0) return std::make_pair(0, 0);
    double DeltaR = 0.4;
    std::set<Constituent const *> matchedTopConsts;
    for(unsigned gent = 0; gent < gentopdau.size(); gent++) 
    {
        double deltaRMin = 100000.;
        Constituent const *bestMatch;
        for(unsigned nt=0; nt<topconst.size();nt++)
        { 
            const double dr = topconst[nt]->p().DeltaR(gentopdau.at(gent));
            if( dr < deltaRMin ) 
            {
                deltaRMin = dr;
                bestMatch = topconst[nt];
            }
        }
        if(deltaRMin < DeltaR)
        {
            //If multiple gentops match the same jet, the set will only contain unique matched jets 
            matchedTopConsts.insert(bestMatch);
            match++;
        }
    }
    return std::make_pair(match, int(matchedTopConsts.size()));
}

//std::pair<std::vector<int>*, std::pair<std::vector<int>*, std::vector<double>*>> TopCat::TopConst(const std::vector<TopObject>& tops, const std::vector<TLorentzVector>& genDecayLVec, const std::vector<int>& genDecayPdgIdVec, const std::vector<int>& genDecayIdxVec, const std::vector<int>& genDecayMomIdxVec)
//{
//    //Check matching between top candidates and gen top
//    vector<int>* topMatch = new vector<int>();
//    vector<TLorentzVector> hadtopLVec = genUtility::GetHadTopLVec(genDecayLVec, genDecayPdgIdVec, genDecayIdxVec, genDecayMomIdxVec);
//    vector<TLorentzVector> MatchGentop;
//    vector<TopObject> matchTops;
//    bool topmatch = GetMatchedTop(tops, matchTops, hadtopLVec, MatchGentop);//final top match
//
//    //check matching between reco top constituents and top gen decay daughters
//    vector<int>* topConstMatch = new vector<int>();
//    vector<double>* constMatchGenPt = new vector<double>();
//    int iMatch = 0;
//    for(const auto& top : tops)
//    {
//        const vector<Constituent const*>& topConst = top.getConstituents();
//
//        //wrong horrible terrible bad inefficient hack to set reco top to gen top matching vector, please replace me!
//        if(topConst.size() && iMatch < matchTops.size() && matchTops[iMatch].getConstituents().size() && (topConst[0] == (matchTops[iMatch].getConstituents())[0]))
//        {
//            topMatch->push_back(1);
//            ++iMatch;
//        }
//        else topMatch->push_back(0);
//
//        int matches = 0;
//        double genPt = 0;
//        for(const auto& genTop : hadtopLVec)
//        {
//            vector<TLorentzVector> gentopdauLVec = genUtility::GetTopdauLVec(genTop, genDecayLVec, genDecayPdgIdVec, genDecayIdxVec, genDecayMomIdxVec);
//            if(topConst.size()==1)
//            {
//                matches = std::max(1, matches);
//                genPt = genTop.Pt();
//            }
//            else if(topConst.size()==2)
//            {
//                int dimatch = GetMatchedTopConst(topConst, gentopdauLVec);
//                if(dimatch > matches)
//                {
//                    matches = dimatch;
//                    genPt = genTop.Pt();
//                }
//            }
//            else if(topConst.size()==3)
//            {
//                int trimatch = GetMatchedTopConst(topConst, gentopdauLVec);
//                if(trimatch > matches)
//                {
//                    matches = trimatch;
//                    genPt = genTop.Pt();
//                }
//            }
//        }
//        topConstMatch->push_back(matches);
//        constMatchGenPt->push_back(genPt);
//    }
//    return std::make_pair(topMatch, std::make_pair(topConstMatch, constMatchGenPt));
//}
