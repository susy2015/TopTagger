#include "SusyAnaTools/Tools/customize.h"
#include "SusyAnaTools/Tools/baselineDef.h"
#include "SusyAnaTools/Tools/NTupleReader.h"
#include "TopTagger/TopTagger/include/TopTagger.h"
#include "TopTagger/TopTagger/include/TopTaggerResults.h"
#include "TopTagger/TopTagger/include/TopTaggerUtilities.h"

#include "TaggerUtility.h"
#include "PlotUtility.h"
#include "MVAstuff.h"
#include <map>
#include <iostream>

// === Main Function ===================================================
int main(int argc, char* argv[]) {
  if (argc < 5)
    {
      std::cerr <<"Please give 4 arguments "<<"SubsampleName"<<" MaxEvent"<<" Startfile"<<" No. of Files to run"<<std::endl;
      std::cerr <<" Valid configurations are " << std::endl;
      std::cerr <<" ./TagTest1 TTbarInc 1000 0 1" << std::endl;
      return -1;
    }
  const char *subsamplename = argv[1]; 
  const char *Maxevent = argv[2];
  const  char *Stratfile = argv[3];
  const  char *Filerun = argv[4];
  const  int startfile = std::atoi(Stratfile);
  const int filerun = std::atoi(Filerun);  
  const int maxevent = std::atoi(Maxevent);
  BaseHistgram myBaseHistgram;
  myBaseHistgram.BookHistgram(subsamplename, startfile);
  const string condorSpec = argc==6 ? argv[5]: "";  
  TChain *fChain = 0;
  if(!FillChain(fChain, subsamplename, condorSpec, startfile, filerun))
    {
      std::cerr << "Cannot get the tree " << std::endl;
    }
  
  /*  TChain *fChain = new TChain("stopTreeMaker/AUX");
  for(int i=1; i<4; i++){
    fChain->Add(Form("/eos/uscms/store/user/lpcsusyhad/Stop_production/Spring16_80X_Oct_2016_Ntp_v10X_AK8_QGL/SMS-T1tttt_mGluino-1500_mLSP-100_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/Spring16_80X_Oct_2016_Ntp_v10p0_AK8_QGL_SMS-T1tttt_mGluino-1500_mLSP-100/161021_184102/0000/stopFlatNtuples_%d.root",i));
    }*/

  //Use BaselineVessel class for baseline variables and selections
  std::string spec = "MVAstuff";
  ExpBaselineVessel = new BaselineVessel(spec);
  AnaFunctions::prepareForNtupleReader();
  AnaFunctions::prepareTopTagger();
  NTupleReader *tr =0;
  tr = new NTupleReader(fChain);
  tr->registerFunction(&passBaselineFuncExp);

  //configure top tagger
  TopTagger tt;
  tt.setCfgFile("MyFile.cfg");


  TString sample(subsamplename);
  TopCat topcat;
  TopVar topvar;
  // --- Analyse events --------------------------------------------
  std::cout<<"First loop begin: "<<std::endl;
  int entries = tr->getNEntries();
  std::cout<<"\nentries : "<<entries<<"\t MC Scale: "<<Lumiscale<<std::endl; 
  cout<<"maxevent: "<<maxevent<<endl;
  int ev = 0;
  double totevt = 0;
  double totcand = 0;
  double totmatchcand = 0;
  double totnomatchcand = 0;
  double heptop = 0;
  double hepmatchtop = 0;
  double hepnomatchtop = 0;
  double hepFakenum = 0;
  double cand = 0;
  double matchcand = 0;
  double hepmatchcand = 0;
  double nomatchcand = 0;
  double hepnomatchcand = 0;
  // Loop over the events (tree entries)
  while(tr->getNextEvent()){
    if(maxevent>=0 && tr->getEvtNum() > maxevent ) break;
    // Add print out of the progress of looping
    if( tr->getEvtNum()-1 == 0 || tr->getEvtNum() == entries || (tr->getEvtNum()-1)%(entries/10) == 0 ) std::cout<<"\n   Processing the "<<tr->getEvtNum()-1<<"th event ..."<<std::endl;
    ev++;
    //std::cout<<"Event: "<<ev<<std::endl;
    const vector<TLorentzVector> &genDecayLVec = tr->getVec<TLorentzVector>("genDecayLVec");
    const vector<int> &genDecayIdxVec = tr->getVec<int>("genDecayIdxVec");
    const vector<int> &genDecayPdgIdVec = tr->getVec<int>("genDecayPdgIdVec");
    const vector<int> &genDecayMomIdxVec = tr->getVec<int>("genDecayMomIdxVec");
    const vector<TLorentzVector> &jetsLVec = tr->getVec<TLorentzVector>("jetsLVec");
    const vector<double> &recoJetsBtag_0 = tr->getVec<double>("recoJetsBtag_0");
    const std::vector<double>& qgLikelihood      = tr->getVec<double>("qgLikelihood");
    const std::vector<double>& recoJetsCharge      = tr->getVec<double>("recoJetsCharge_0");

    double met=tr->getVar<double>("met");
    double metphi=tr->getVar<double>("metphi");
    double EvtWt = tr->getVar<double>("evtWeight");
    int nJets = tr->getVar<int>("cntNJetsPt30Eta24"+spec);
    TLorentzVector metLVec; metLVec.SetPtEtaPhiM(met, 0, metphi, 0);
    EventWeight = EvtWt;
    Lumiscale = Lumiscale * EventWeight;

    bool passNJET = tr->getVar<bool>("passnJets"+spec);
    bool passdPhis = tr->getVar<bool>("passdPhis"+spec);
    bool passMET =  tr->getVar<bool>("passMET"+spec);
    bool passBJET = tr->getVar<bool>("passBJets"+spec);
    bool passHT = tr->getVar<bool>("passHT"+spec);
    
    //cut    
    //if(!(passMET && passNJET && passBJET))continue;
    if(!(passNJET && passMET))continue;

    //Total event
    totevt += 1*Lumiscale;
    
    vector<TLorentzVector> hadtopLVec = genUtility::GetHadTopLVec(genDecayLVec, genDecayPdgIdVec, genDecayIdxVec, genDecayMomIdxVec);
    for(unsigned ig =0; ig<hadtopLVec.size();ig++)
      {
	pUtility::FillDouble(myBaseHistgram.hgentopPt,hadtopLVec[ig].Pt(),Lumiscale);
      }
    std::vector<TLorentzVector> jetsLVec_forTagger;
    std::vector<double> recoJetsBtag_forTagger;
    std::vector<double> qgLikelihood_forTagger;
    AnaFunctions::prepareJetsForTagger(jetsLVec, recoJetsBtag_0, jetsLVec_forTagger, recoJetsBtag_forTagger, qgLikelihood, qgLikelihood_forTagger);
    
    // top tagger
    //construct vector of constituents 
    vector<Constituent> constituents = ttUtility::packageConstituents(jetsLVec_forTagger, recoJetsBtag_forTagger, qgLikelihood_forTagger);
    //run tagger
    tt.runTagger(constituents);
    //get output of tagger
    const TopTaggerResults& ttr = tt.getResults();
    //Use result for top var
    vector<TopObject*> Ntop = ttr.getTops();
    const vector<TopObject> &NtopCand = ttr.getTopCandidates();
    //std::cout<<"Ntop: "<<Ntop.size()<<" NtopCand: "<<NtopCand.size()<<std::endl;
    //Disc Plot    
    for(unsigned ci = 0; ci<NtopCand.size(); ci++)
      {
	totcand += 1*Lumiscale;
	pUtility::FillDouble(myBaseHistgram.hdisc,NtopCand[ci].getDiscriminator(), Lumiscale);
      }

    //matching
    bool topmatchCand = false;
    vector<TopObject> MatchNtopCand;
    vector<TLorentzVector> MatchGentopWidCand; 
    if(topcat.GetMatchedTop(NtopCand, MatchNtopCand, hadtopLVec, MatchGentopWidCand)) topmatchCand = true;//topcand match
    
    //constituent matching
    std::pair<std::vector<int>, std::pair<std::vector<int>, std::vector<TLorentzVector>>> genMatches = topcat.TopConst(NtopCand, genDecayLVec, genDecayPdgIdVec, genDecayIdxVec, genDecayMomIdxVec);
  
    std::vector<int> *trijetmatch = new std::vector<int>(genMatches.second.first);
    
    //std::cout<<"NtopCand.size(): "<<NtopCand.size()<<" trijetmatch->size(): "<<trijetmatch->size()<<std::endl;
    
    double maxNomatchDisc = 0.0;
    for(unsigned ci = 0; ci<NtopCand.size(); ci++)
      {
	if((*trijetmatch)[ci]==3)
	  {
	    totmatchcand += 1*Lumiscale;
	    pUtility::FillDouble(myBaseHistgram.hdiscMatch,NtopCand[ci].getDiscriminator(), Lumiscale);
	  }	
	else {
	  totnomatchcand +=1*Lumiscale;
	  pUtility::FillDouble(myBaseHistgram.hdiscNoMatch,NtopCand[ci].getDiscriminator(), Lumiscale);
	  if(NtopCand[ci].getDiscriminator()>maxNomatchDisc)maxNomatchDisc = NtopCand[ci].getDiscriminator();
	}
      }    
    if(sample.Contains("ZJetsToNuNu"))
      {     
	pUtility::FillDouble(myBaseHistgram.hmaxdiscNoMatch,maxNomatchDisc, Lumiscale);
      }
    
    //HEP
    /*for(unsigned ci = 0; ci<Ntop.size(); ci++)
      {
	heptop += 1*Lumiscale;
        if((*trijetmatch)[ci]==3)
          {
	    hepmatchtop += 1*Lumiscale;
	  }
	else hepnomatchtop += 1*Lumiscale;
      }
    if(sample.Contains("ZJetsToNuNu") && Ntop.size())
      {
	hepFakenum += 1*Lumiscale;
	}
    //for TPR and FPR
    std::pair<std::vector<int>, std::pair<std::vector<int>, std::vector<TLorentzVector>>> genMatchescand = topcat.TopConst(NtopCand, genDecayLVec,genDecayPdgIdVec, genDecayIdxVec, genDecayMomIdxVec);
    std::vector<int> *trijetmatchcand = new std::vector<int>(genMatchescand.second.first);
    for(unsigned ci = 0; ci<NtopCand.size(); ci++)
      {
	cand += 1*Lumiscale;
	if((*trijetmatchcand)[ci]==3)
	  {
	    matchcand += 1*Lumiscale;
	    if(Ntop.size()){
	      for(unsigned ti = 0; ti<Ntop.size(); ti++)
		{
		  if(NtopCand[ci].p()==Ntop[ti]->p() && (*trijetmatch)[ci]==3) hepmatchcand += 1*Lumiscale;
		}
	    }
	  }
	else
	  {
	    nomatchcand += 1*Lumiscale;
	    if(Ntop.size())
	      {
		for(unsigned ti = 0; ti<Ntop.size(); ti++)
		  {
		    if(NtopCand[ci].p()==Ntop[ti]->p() && (*trijetmatch)[ci]!=3) hepnomatchcand += 1*Lumiscale;
		  }
	      }
	  }
	  }*/
    
  }//event loop
  //Event stat histo
  myBaseHistgram.hcheck->SetBinContent(1, totevt);
  myBaseHistgram.hcheck->SetBinContent(2, totcand);
  myBaseHistgram.hcheck->SetBinContent(3, totmatchcand);
  myBaseHistgram.hcheck->SetBinContent(4, totnomatchcand);
  myBaseHistgram.hcheckHep->SetBinContent(1, totevt);
  myBaseHistgram.hcheckHep->SetBinContent(2, heptop);
  myBaseHistgram.hcheckHep->SetBinContent(3, hepmatchtop);
  myBaseHistgram.hcheckHep->SetBinContent(4, hepnomatchtop);
  myBaseHistgram.hcheckHep->SetBinContent(5, hepFakenum);
  myBaseHistgram.hcheckHepcand->SetBinContent(1, totevt);
  myBaseHistgram.hcheckHepcand->SetBinContent(2, cand);
  myBaseHistgram.hcheckHepcand->SetBinContent(3, matchcand);
  myBaseHistgram.hcheckHepcand->SetBinContent(4, hepmatchcand);
  myBaseHistgram.hcheckHepcand->SetBinContent(5, nomatchcand);
  myBaseHistgram.hcheckHepcand->SetBinContent(6, hepnomatchcand);
  (myBaseHistgram.oFile)->Write();
  return 0;
}
