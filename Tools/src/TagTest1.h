#include <iostream>
#include <string>
#include <vector>
#include "TH1.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TFile.h"
#include "TLorentzVector.h"
#include "TVector3.h"
#include "TChain.h"
#include "TTree.h"
#include "TString.h"

#include "SusyAnaTools/Tools/NTupleReader.h"
#include "SusyAnaTools/Tools/samples.h"

using namespace std;

static BaselineVessel *ExpBaselineVessel;
void passBaselineFuncExp(NTupleReader& tr)
{
  (*ExpBaselineVessel)(tr);
}
double Lumiscale = 1.0;
double EventWeight = 1.0;

class BaseHistgram
{
 public:
  void BookHistgram(const char *, const int&);
  TFile *oFile;
  TH1D *hNtop;
  TH2D *hNjet_Ntop;
  TH1D *hNtop_old;
  TH1D *hNtopCand;
  TH1D *hMatchedNtop;
  TH1D *hConstMatchedNtop;
  TH1D *hMatchedNtopCand;
  TH1D *hgentopPt_nomatch;
  TH1D *hrecotopPt_nomatch;
  TH1D *hgentopPt_match;
  TH1D *hrecotopPt_match;
  TH1D *hgentopM_nomatch;
  TH1D *hrecotopM_nomatch;
  TH1D *hgentopM_match;
  TH1D *hrecotopM_match;
  TH1D *hgentopdRmax_nomatch;
  TH1D *hrecotopdRmax_nomatch;
  TH1D *hgentopdRmax_match;
  TH1D *hrecotopdRmax_match;

  TH1D *hgentopPt_den;
  TH1D *hgentopPt_num;
  TH1D *hgentopPt_old_den;
  TH1D *hgentopPt_old_num;
  TH1D *hrecotopPt_den;
  TH1D *hrecotopPt_num;
  TH1D *hrecotopEta_den;
  TH1D *hrecotopEta_num;
  TH1D *hrecotopDRmax_den;
  TH1D *hrecotopDRmax_num;
  TH1D *hfakeMET_den;
  TH1D *hfakeMET_num;
  TH1D *hfakeNJET_den;
  TH1D *hfakeNJET_num;
  TH1D *hfakeMET_old_den;
  TH1D *hfakeMET_old_num;
  TH1D *hEffNJET_den;
  TH1D *hEffNJET_num;
  TH1D *hPurNJET_den;
  TH1D *hPurNJET_num;

  TH1D *hgentopPt_2match_den;
  TH1D *hgentopPt_2match_num;
  TH1D *hgentopPt_1match_den;
  TH1D *hgentopPt_1match_num;

  TH1D *hgentopPt_Ineff_den;
  TH1D *hgentopPt_Ineff_num;

};

void BaseHistgram::BookHistgram(const char *outFileName, const int& filerun)
{
  TString filename(outFileName);
  TString index(std::to_string(filerun));
  filename+= "_TagTest1"+index+".root";
  oFile = new TFile(filename, "recreate");
 
  hNtop = new TH1D("hNtop", "No. of top;N_{top};Event", 10, 0, 10);
  hNtop->Sumw2();
  hNjet_Ntop = new TH2D("hNjet_Ntop", "No. of top;N_{jet};N_{top}", 15, 0, 15, 15, 0, 15);
  hNjet_Ntop->Sumw2();
  hNtop_old = new TH1D("hNtop_old", "No. of top;N_{top};Event", 10, 0, 10);
  hNtop_old->Sumw2();
  hNtopCand = new TH1D("hNtopCand", "No.of topcand;N_{top};Event", 10, 0, 10);
  hNtopCand->Sumw2();
  hMatchedNtop = new TH1D("hMatchedNtop", "No.of matched top;N_{top};Event", 10, 0, 10);
  hMatchedNtop->Sumw2();
  hConstMatchedNtop = new TH1D("hConstMatchedNtop", "No.of matched top;N_{top};Event", 10, 0, 10);
  hConstMatchedNtop->Sumw2();
  hMatchedNtopCand = new TH1D("hMatchedNtopCand", "No.of matched topcand;N_{top};Event", 10, 0, 10);
  hMatchedNtopCand->Sumw2();

  hgentopPt_nomatch = new TH1D("hgentopPt_nomatch","Gen.Top P_{T};p_{T}[GeV];Event",25, 0, 1000);
  hgentopPt_nomatch->Sumw2();
  hrecotopPt_nomatch = new TH1D("hrecotopPt_nomatch","Gen.Top P_{T};p_{T}[GeV];Event",25, 0, 1000);
  hrecotopPt_nomatch->Sumw2();
  hgentopPt_match = new TH1D("hgentopPt_match","Gen.Top P_{T};p_{T}[GeV];Event",25, 0, 1000);
  hgentopPt_match->Sumw2();
  hrecotopPt_match = new TH1D("hrecotopPt_match","Gen.Top P_{T};p_{T}[GeV];Event",25, 0, 1000);
  hrecotopPt_match->Sumw2();
  hgentopM_nomatch = new TH1D("hgentopM_nomatch","Gen.Top Mass;Mass[GeV];Event",25, 0, 500);
  hgentopM_nomatch->Sumw2();
  hrecotopM_nomatch = new TH1D("hrecotopM_nomatch","Gen.Top Mass;Mass[GeV];Event",25, 0, 500);
  hrecotopM_nomatch->Sumw2();
  hgentopM_match = new TH1D("hgentopM_match","Gen.Top Mass;Mass[GeV];Event",25, 0, 500);
  hgentopM_match->Sumw2();
  hrecotopM_match = new TH1D("hrecotopM_match","Gen.Top Mass;Mass[GeV];Event",25, 0, 500);
  hrecotopM_match->Sumw2();
  hgentopdRmax_nomatch = new TH1D("hgentopdRmax_nomatch","Gen.Top Max. dR;#Delta R;Event",15, 0, 15);
  hgentopdRmax_nomatch->Sumw2();
  hrecotopdRmax_nomatch = new TH1D("hrecotopdRmax_nomatch","Gen.Top Max. dR;#Delta R;Event",15, 0, 15);
  hrecotopdRmax_nomatch->Sumw2();
  hgentopdRmax_match = new TH1D("hgentopdRmax_match","Gen.Top Max. dR;#Delta R;Event",15, 0, 15);
  hgentopdRmax_match->Sumw2();
  hrecotopdRmax_match = new TH1D("hrecotopdRmax_match","Gen.Top Max. dR;#Delta R;Event",15, 0, 15);
  hrecotopdRmax_match->Sumw2();

  hgentopPt_den = new TH1D("hgentopPt_den","Top P_{T};p_{T}[GeV];Event",25, 0, 1000);
  hgentopPt_den->Sumw2();
  hgentopPt_num = new TH1D("hgentopPt_num","Top P_{T};p_{T}[GeV];Event",25, 0, 1000);
  hgentopPt_num->Sumw2();
  hrecotopPt_den = new TH1D("hrecotopPt_den","Top P_{T};p_{T}[GeV];Event",25, 0, 1000);
  hrecotopPt_den->Sumw2();
  hrecotopPt_num = new TH1D("hrecotopPt_num","Top P_{T};p_{T}[GeV];Event",25, 0, 1000);
  hrecotopPt_num->Sumw2();
  hrecotopEta_den = new TH1D("hrecotopEta_den","Top #eta;#eta;Event",24, -2.4, 2.4);
  hrecotopEta_den->Sumw2();
  hrecotopEta_num = new TH1D("hrecotopEta_num","Top #eta;#eta;Event",24, -2.4, 2.4);
  hrecotopEta_num->Sumw2();
  hrecotopDRmax_den = new TH1D("hrecotopDRmax_den","Top #DeltaRmax;#DeltaRmax;Event",30, 0, 1.5);
  hrecotopDRmax_den->Sumw2();
  hrecotopDRmax_num = new TH1D("hrecotopDRmax_num","Top #DeltaRmax;#DeltaRmax;Event",30, 0, 1.5);
  hrecotopDRmax_num->Sumw2();
  hgentopPt_old_den = new TH1D("hgentopPt_old_den","Top P_{T};p_{T}[GeV];Event",25, 0, 1000);
  hgentopPt_old_den->Sumw2();
  hgentopPt_old_num = new TH1D("hgentopPt_old_num","Top P_{T};p_{T}[GeV];Event",25, 0, 1000);
  hgentopPt_old_num->Sumw2();
  hfakeMET_den = new TH1D("hfakeMET_den","FakeRate in p_{T}^{miss} bin;p_{T}^{miss}[GeV];Event",25, 0, 1000);
  hfakeMET_den->Sumw2();
  hfakeNJET_num = new TH1D("hfakeNJET_num","FakeRate in N_{jet} bin;N_{jet};Event",10, 4, 14);
  hfakeNJET_num->Sumw2();
  hfakeNJET_den = new TH1D("hfakeNJET_den","FakeRate in N_{jet} bin;N_{jet};Event",10, 4, 14);
  hfakeNJET_den->Sumw2();
  hfakeMET_num = new TH1D("hfakeMET_num","FakeRate in p_{T}^{miss} bin;p_{T}^{miss}[GeV];Event",25, 0, 1000);
  hfakeMET_num->Sumw2();
  hfakeMET_old_den = new TH1D("hfakeMET_old_den","FakeRate in p_{T}^{miss} bin;p_{T}^{miss}[GeV];Event",25, 0, 1000);
  hfakeMET_old_den->Sumw2();
  hfakeMET_old_num = new TH1D("hfakeMET_old_num","FakeRate in p_{T}^{miss} bin;p_{T}^{miss}[GeV];Event",25, 0, 1000);
  hfakeMET_old_num->Sumw2();
  hEffNJET_num = new TH1D("hEffNJET_num","Eff. in N_{jet} bin;N_{jet};Event",10, 4, 14);
  hEffNJET_num->Sumw2();
  hEffNJET_den = new TH1D("hEffNJET_den","Eff. in N_{jet} bin;N_{jet};Event",10, 4, 14);
  hEffNJET_den->Sumw2();
  hPurNJET_num = new TH1D("hPurNJET_num","Purity in N_{jet} bin;N_{jet};Event",10, 4, 14);
  hPurNJET_num->Sumw2();
  hPurNJET_den = new TH1D("hPurNJET_den","Purity in N_{jet} bin;N_{jet};Event",10, 4, 14);
  hPurNJET_den->Sumw2();

  hgentopPt_2match_den = new TH1D("hgentopPt_2match_den","Top P_{T};p_{T}[GeV];Event",25, 0, 1000);
  hgentopPt_2match_den->Sumw2();
  hgentopPt_2match_num = new TH1D("hgentopPt_2match_num","Top P_{T};p_{T}[GeV];Event",25, 0, 1000);
  hgentopPt_2match_num->Sumw2();
  hgentopPt_1match_den = new TH1D("hgentopPt_1match_den","Top P_{T};p_{T}[GeV];Event",25, 0, 1000);
  hgentopPt_1match_den->Sumw2();
  hgentopPt_1match_num = new TH1D("hgentopPt_1match_num","Top P_{T};p_{T}[GeV];Event",25, 0, 1000);
  hgentopPt_1match_num->Sumw2();

  hgentopPt_Ineff_den = new TH1D("hgentopPt_Ineff_den", "Top P_{T};p_{T}[GeV];Event",25, 0, 1000);
  hgentopPt_Ineff_den->Sumw2();
  hgentopPt_Ineff_num = new TH1D("hgentopPt_Ineff_num", "Top P_{T};p_{T}[GeV];Event",25, 0, 1000);
  hgentopPt_Ineff_num->Sumw2();
}
bool FillChain(TChain* &chain, const char *subsample, const string condorSpec, const int& startfile, const int& filerun){
  AnaSamples::SampleSet        allSamples = condorSpec.empty()? AnaSamples::SampleSet():AnaSamples::SampleSet(condorSpec);
  AnaSamples::SampleCollection allCollections(allSamples);
  bool find = false;  
  TString subsamplename(subsample);
  chain = new TChain(allSamples[subsample].treePath.c_str());
  if(allSamples[subsample] != allSamples.null())
    {
      allSamples[subsample].addFilesToChain(chain, startfile, filerun);
      find = true;
      Lumiscale = allSamples[subsample].getWeight();  
    }
  return find;
}
