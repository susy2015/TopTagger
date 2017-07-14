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
  TH1D *hNtopCand;
  TH1D *hgentopPt;
  TH1D *hdisc;
  TH1D *hdiscMatch;
  TH1D *hdiscNoMatch;
  TH1D *hmaxdiscNoMatch;
  TH1D *hcheck;
  TH1D *hcheckHep;
  TH1D *hcheckHepcand;
};

void BaseHistgram::BookHistgram(const char *outFileName, const int& filerun)
{
  TString filename(outFileName);
  TString index(std::to_string(filerun));
  filename+= "_MVAstuff"+index+".root";
  oFile = new TFile(filename, "recreate");
 
  hNtopCand = new TH1D("hNtopCand", "No.of topcand;N_{top};Event", 15, 0, 15);
  hNtopCand->Sumw2();

  hgentopPt = new TH1D("hgentopPt", "Had gen tau p-{T}", 25, 0, 1000);
  hgentopPt->Sumw2();
  hdisc = new TH1D("hdisc", "MVA disc", 100, 0, 1);
  hdisc->Sumw2();
  hdiscMatch = new TH1D("hdiscMatch", "MVA disc", 100, 0, 1);
  hdiscMatch->Sumw2();
  hdiscNoMatch = new TH1D("hdiscNoMatch", "MVA disc", 100, 0, 1);
  hdiscNoMatch->Sumw2();
  hmaxdiscNoMatch = new TH1D("hmaxdiscNoMatch", "MVA disc", 100, 0, 1);
  hmaxdiscNoMatch->Sumw2();
  hcheck = new TH1D("hcheck" ,"Event stat", 4, 0, 4);
  hcheck->Sumw2();
  hcheckHep = new TH1D("hcheckHep" ,"Event stat", 5, 0, 5);
  hcheckHep->Sumw2();
  hcheckHepcand = new TH1D("hcheckHepcand" ,"Event stat", 6, 0, 6);
  hcheckHepcand->Sumw2();
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
