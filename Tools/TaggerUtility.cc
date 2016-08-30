#include "TaggerTest/include/TaggerUtility.h"

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
bool TopCat::GetMatchedTop(vector<TopObject*> Top, vector<TopObject*> &MachedTop, vector<TLorentzVector>Gentop, vector<TLorentzVector> &MGentop){
  bool match = false; 
  if(Gentop.size()==0) return match;
  double DeltaR = 0.4;
  for(unsigned nt=0; nt<Top.size();nt++){
    double deltaRMin = 100000.;
    unsigned tid = -1;
    for(unsigned gent = 0; gent < Gentop.size(); gent++) { // Loop over objects
      const double dr = Top[nt]->p().DeltaR(Gentop.at(gent));
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

bool TopCat::GetMatchedTop(vector<TopObject> TopCand, vector<TopObject> &MachedTopCand, vector<TLorentzVector>Gentop, vector<TLorentzVector> &MGentop){
  bool match = false; 
  if(Gentop.size()==0) return match;
  double DeltaR = 0.4;
  for(unsigned nt=0; nt<TopCand.size();nt++){
    double deltaRMin = 100000.;
    unsigned tid = -1;
    for(unsigned gent = 0; gent < Gentop.size(); gent++) { // Loop over objects
      const double dr = TopCand[nt].p().DeltaR(Gentop.at(gent));
      if( dr < deltaRMin ) {deltaRMin = dr; tid = gent;}
    }
    if(deltaRMin < DeltaR){
      MachedTopCand.push_back(TopCand[nt]);
      MGentop.push_back(Gentop[tid]);
      match = true;
    }
  }
  return match;
}
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

int TopCat::GetMatchedTopConst(vector<Constituent const *> topconst, vector<TLorentzVector>gentopdau){
  int match=0;
  if(gentopdau.size()==0) return match;
  double DeltaR = 0.4;
  for(unsigned nt=0; nt<topconst.size();nt++){
    double deltaRMin = 100000.;
    for(unsigned gent = 0; gent < gentopdau.size(); gent++) { // Loop over objects                                                                                           
      const double dr = topconst[nt]->p().DeltaR(gentopdau.at(gent));
      if( dr < deltaRMin ) deltaRMin = dr;
    }
    if(deltaRMin < DeltaR) match++;
  }
  return match;

}
