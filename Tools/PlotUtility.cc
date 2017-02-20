#include "PlotUtility.h"

using namespace std;

namespace pUtility {

  void FillDouble(TH1* hist, const double &a, const double &w){
    int nbin = hist->GetNbinsX();
    double low = hist->GetBinLowEdge(nbin);
    double high = hist->GetBinLowEdge(nbin + 1);
    double copy = a;
    if(copy >= high) copy = low;
    hist->Fill(copy, w);
  }

  void FillInt(TH1* hist, const int &a, const double &w){
    int nbin = hist->GetNbinsX();
    int low = hist->GetBinLowEdge(nbin);
    int high = hist->GetBinLowEdge(nbin + 1);
    int copy = a;
    if(copy >= high) copy = low;
    hist->Fill(copy, w);
  }

  void Fill2D(TH2 *hist, const double &a, const double &b, const double &w){
    int nbinx = hist->GetNbinsX();
    int nbiny = hist->GetNbinsY();
    double lowx = hist->GetXaxis()->GetBinLowEdge(nbinx);
    double highx = hist->GetXaxis()->GetBinLowEdge(nbinx + 1);
    double lowy = hist->GetYaxis()->GetBinLowEdge(nbiny);
    double highy = hist->GetYaxis()->GetBinLowEdge(nbiny + 1);
    double copyx = a;
    if(copyx >= highx) copyx = lowx;
    double copyy = b;
    if(copyy >= highy) copyy = lowy;
    hist->Fill(copyx, copyy, w);
  }
}

namespace genUtility {
  
vector<TLorentzVector> GetHadTopLVec(const vector<TLorentzVector>& genDecayLVec, const vector<int>& genDecayPdgIdVec, const vector<int>& genDecayIdxVec, const vector<int>& genDecayMomIdxVec)
{
    vector<TLorentzVector> tLVec;
    for(unsigned it=0; it<genDecayLVec.size(); it++)
    {
        int pdgId = genDecayPdgIdVec.at(it);
        if(abs(pdgId)==6)
        {
            for(unsigned ig=0; ig<genDecayLVec.size(); ig++)
            {
                if( genDecayMomIdxVec.at(ig) == genDecayIdxVec.at(it) )
                {
                    int pdgId = genDecayPdgIdVec.at(ig);
                    if(abs(pdgId)==24)
                    {
                        int flag = 0;
                        for(unsigned iq=0; iq<genDecayLVec.size(); iq++)
                        {
                            if( genDecayMomIdxVec.at(iq) == genDecayIdxVec.at(ig) ) 
                            {
                                int pdgid = genDecayPdgIdVec.at(iq);
                                if(abs(pdgid)== 11 || abs(pdgid)== 13 || abs(pdgid)== 15) flag++;
                            }
                        }
                        if(!flag) tLVec.push_back(genDecayLVec.at(it));
                    }
                }
            }//dau. loop
        }//top cond
    }//genloop
    return tLVec;
  }

    vector<TLorentzVector> GetTopdauLVec(TLorentzVector top, vector<TLorentzVector>genDecayLVec, vector<int>genDecayPdgIdVec, vector<int>genDecayIdxVec, vector<int>genDecayMomIdxVec)
    {
        vector<TLorentzVector>topdauLVec;
        for(unsigned it=0; it<genDecayLVec.size(); it++)
        {
            if(genDecayLVec[it]==top){
                for(unsigned ig=0; ig<genDecayLVec.size(); ig++)
                {
                    if( genDecayMomIdxVec.at(ig) == genDecayIdxVec.at(it) )
                    {
                        int pdgId = genDecayPdgIdVec.at(ig);
                        if(abs(pdgId)==5)topdauLVec.push_back(genDecayLVec[ig]);
                        if(abs(pdgId)==24)
                        {
			  //topdauLVec.push_back(genDecayLVec[ig]);	 
                            for(unsigned iq=0; iq<genDecayLVec.size(); iq++)
                            {
                                if( genDecayMomIdxVec.at(iq) == genDecayIdxVec.at(ig) ) 
                                {
                                    int pdgid = genDecayPdgIdVec.at(iq);
                                    if(abs(pdgid)!= 11 && abs(pdgid)!= 13 && abs(pdgid)!= 15) topdauLVec.push_back(genDecayLVec[iq]);
                                }
                            }
                        }
                    }
                }//dau. loop
            }//top cand.
        }//gen loop
        return topdauLVec;
    }



vector<TLorentzVector> GetHadWLVec(const vector<TLorentzVector>& genDecayLVec, const vector<int>& genDecayPdgIdVec, const vector<int>& genDecayIdxVec, const vector<int>& genDecayMomIdxVec)
{
    vector<TLorentzVector> tLVec;
    for(unsigned it=0; it<genDecayLVec.size(); it++)
    {
        int pdgId = genDecayPdgIdVec.at(it);
        if(abs(pdgId)==24)
        {
            int flag = 0;                    
            for(unsigned ig=0; ig<genDecayLVec.size(); ig++)
            {
                if( genDecayMomIdxVec.at(ig) == genDecayIdxVec.at(it) )
                {
                    int pdgid = genDecayPdgIdVec.at(ig);
                    if(abs(pdgid)== 11 || abs(pdgid)== 13 || abs(pdgid)== 15) flag++;
                }
            }//dau. loop
            if(!flag) tLVec.push_back(genDecayLVec.at(it));

        }//W cond
    }//genloop
    return tLVec;
  }

    vector<TLorentzVector> GetWdauLVec(TLorentzVector W, vector<TLorentzVector>genDecayLVec, vector<int>genDecayPdgIdVec, vector<int>genDecayIdxVec, vector<int>genDecayMomIdxVec)
    {
        vector<TLorentzVector>WdauLVec;
        for(unsigned it=0; it<genDecayLVec.size(); it++)
        {
            if(genDecayLVec[it]==W){
                for(unsigned ig=0; ig<genDecayLVec.size(); ig++)
                {
                    if( genDecayMomIdxVec.at(ig) == genDecayIdxVec.at(it) )
                    {
                        int pdgid = genDecayPdgIdVec.at(ig);
                        if(abs(pdgid)!= 11 && abs(pdgid)!= 13 && abs(pdgid)!= 15) WdauLVec.push_back(genDecayLVec[ig]);
                    }
                }//dau. loop
            }//W cand.
        }//gen loop
        return WdauLVec;
    }




}
