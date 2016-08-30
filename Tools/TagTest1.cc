#include "SusyAnaTools/Tools/customize.h"
#include "SusyAnaTools/Tools/baselineDef.h"
#include "SusyAnaTools/Tools/NTupleReader.h"
#include "TopTagger/TopTagger/include/TopTagger.h"
#include "TopTagger/TopTagger/include/TopTaggerResults.h"
#include "TopTagger/TopTagger/include/TopTaggerUtilities.h"

#include "TaggerTest/include/TaggerUtility.h"
#include "TaggerTest/include/PlotUtility.h"
#include "TagTest1.h"

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
  TChain *fChain = 0;
  BaseHistgram myBaseHistgram;
  myBaseHistgram.BookHistgram(subsamplename, startfile);
  const string condorSpec = argc==6 ? argv[5]: "";  
  if(!FillChain(fChain, subsamplename, condorSpec, startfile, filerun))
    {
      std::cerr << "Cannot get the tree " << std::endl;
    }

  //Use BaselineVessel class for baseline variables and selections
  std::string spec = "TagTest";
  ExpBaselineVessel = new BaselineVessel(spec);
  AnaFunctions::prepareForNtupleReader();
  AnaFunctions::prepareTopTagger();
  NTupleReader *tr =0;
  tr = new NTupleReader(fChain);
  tr->registerFunction(&passBaselineFuncExp);

  //configure top tagger
  TopTagger tt;
  tt.setCfgFile("file.cfg");


  TString sample(subsamplename);
  TopCat topcat;
  TopVar topvar;
  // --- Analyse events --------------------------------------------
  std::cout<<"First loop begin: "<<std::endl;
  int entries = tr->getNEntries();
  std::cout<<"\nentries : "<<entries<<"\t MC Scale: "<<Lumiscale<<std::endl; 
  cout<<"maxevent: "<<maxevent<<endl;

  // Loop over the events (tree entries)
  while(tr->getNextEvent()){
    if(maxevent>=0 && tr->getEvtNum() > maxevent ) break;
    // Add print out of the progress of looping
    if( tr->getEvtNum()-1 == 0 || tr->getEvtNum() == entries || (tr->getEvtNum()-1)%(entries/10) == 0 ) std::cout<<"\n   Processing the "<<tr->getEvtNum()-1<<"th event ..."<<std::endl;

    const vector<TLorentzVector> &genDecayLVec = tr->getVec<TLorentzVector>("genDecayLVec");
    const vector<int> &genDecayIdxVec = tr->getVec<int>("genDecayIdxVec");
    const vector<int> &genDecayPdgIdVec = tr->getVec<int>("genDecayPdgIdVec");
    const vector<int> &genDecayMomIdxVec = tr->getVec<int>("genDecayMomIdxVec");
    const vector<TLorentzVector> &jetsLVec = tr->getVec<TLorentzVector>("jetsLVec");
    const vector<double> &recoJetsBtag_0 = tr->getVec<double>("recoJetsBtag_0");

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
    bool passTagger_old = tr->getVar<bool>("passTagger"+spec); 
    const int nTops_old = tr->getVar<int>("nTopCandSortedCnt"+spec);
    const vector<TLorentzVector> &nTopsLVec_old = tr->getVec<TLorentzVector>("vTops"+spec);
    const vector<TLorentzVector> &jetsForTopLVec = tr->getVec<TLorentzVector>("jetsLVec_forTagger"+spec);
    const vector<double> &recoJetsBtag_ForTop = tr->getVec<double>("recoJetsBtag_forTagger"+spec);
    //cut    
    if(!(passMET && passNJET && passBJET))continue;
    
    vector<TLorentzVector> hadtopLVec = genUtility::GetHadTopLVec(genDecayLVec, genDecayPdgIdVec, genDecayIdxVec, genDecayMomIdxVec);
   
    // top tagger
    //construct vector of constituents 
    vector<Constituent> constituents = ttUtility::packageConstituents(jetsForTopLVec, recoJetsBtag_ForTop);
    //run tagger
    tt.runTagger(constituents);
    //get output of tagger
    const TopTaggerResults& ttr = tt.getResults();
    //Use result for top var
    vector<TopObject*> Ntop = ttr.getTops();
    vector<TopObject> NtopCand = ttr.getTopCandidates();

    //matching
    bool topmatch = false;
    bool topmatchCand = false;
    bool topmatch_old = false;
    vector<TopObject*> MatchNtop;
    vector<TopObject> MatchNtopCand;
    vector<TLorentzVector> MatchGentop; 
    vector<TLorentzVector> MatchGentopWidCand; 
    vector<TLorentzVector> MatchGentop_old;
    vector<TLorentzVector> MatchNtop_old;
 
    if(topcat.GetMatchedTop(Ntop, MatchNtop, hadtopLVec, MatchGentop)) topmatch = true;//final top match
    if(topcat.GetMatchedTop(NtopCand, MatchNtopCand, hadtopLVec, MatchGentopWidCand)) topmatchCand = true;//topcand match
    if(topcat.GetMatchedTop(nTopsLVec_old, MatchNtop_old, hadtopLVec, MatchGentop_old)) topmatch_old = true;//final top match

    //constituent matching
    vector<bool> monojetmatch(MatchNtopCand.size(),false);
    vector<bool> dijet2match(MatchNtopCand.size(),false);
    vector<bool> trijet3match(MatchNtopCand.size(),false);
    if(MatchNtopCand.size()){
      for(unsigned tc = 0; tc<MatchNtopCand.size(); tc++){
	vector<Constituent const*> topconst = MatchNtopCand[tc].getConstituents();
	if(topconst.size()!=3)cout<<"const: "<<topconst.size()<<endl;
	vector<TLorentzVector>gentopdauLVec = genUtility::GetTopdauLVec(MatchGentopWidCand[tc], genDecayLVec, genDecayPdgIdVec, genDecayIdxVec, genDecayMomIdxVec);
	if(topconst.size()==1) monojetmatch[tc]=true;
	if(topconst.size()==2){
	  int dimatch = topcat.GetMatchedTopConst(topconst, gentopdauLVec);
	  if(dimatch==2)dijet2match[tc] = true;
	}
	if(topconst.size()==3){
	  int trimatch = topcat.GetMatchedTopConst(topconst, gentopdauLVec);
	  if(trimatch==3)trijet3match[tc] = true;
	}
      }
    }
 vector<TopObject> ConstMatchNtop;
 if(MatchNtopCand.size()){
   for(unsigned cm = 0; cm<MatchNtopCand.size(); cm++){
     if(trijet3match[cm] || dijet2match[cm] || monojetmatch[cm])ConstMatchNtop.push_back(MatchNtopCand[cm]);
   }
 }
    pUtility::FillInt(myBaseHistgram.hNtop,Ntop.size(),Lumiscale);
    pUtility::FillInt(myBaseHistgram.hNtopCand,NtopCand.size(),Lumiscale);   
    pUtility::FillInt(myBaseHistgram.hMatchedNtop,MatchNtop.size(),Lumiscale);
     if(NtopCand.size()){         
       pUtility::FillInt(myBaseHistgram.hMatchedNtopCand,MatchNtopCand.size(),Lumiscale);
       pUtility::FillInt(myBaseHistgram.hConstMatchedNtop,ConstMatchNtop.size(),Lumiscale);    
     }       
     pUtility::FillInt(myBaseHistgram.hNtop_old, nTops_old,Lumiscale);//from old tagger
   

    if(!MatchNtop.size()){
      for(auto& g:hadtopLVec){
	pUtility::FillDouble(myBaseHistgram.hgentopPt_nomatch,g.Pt(), Lumiscale);
      	pUtility::FillDouble(myBaseHistgram.hgentopM_nomatch,g.M(), Lumiscale);
      }
      for(auto& r:Ntop){
	pUtility::FillDouble(myBaseHistgram.hrecotopPt_nomatch, r->p().Pt(), Lumiscale);
	pUtility::FillDouble(myBaseHistgram.hrecotopM_nomatch, r->p().M(), Lumiscale);
      }    
    }
    if(MatchNtop.size()){
      for(auto& g:hadtopLVec){
	pUtility::FillDouble(myBaseHistgram.hgentopPt_match,g.Pt(), Lumiscale);
	pUtility::FillDouble(myBaseHistgram.hgentopM_match,g.M(), Lumiscale);
      }
      for(auto& r:Ntop){
	pUtility::FillDouble(myBaseHistgram.hrecotopPt_match, r->p().Pt(), Lumiscale);
     	pUtility::FillDouble(myBaseHistgram.hrecotopM_match, r->p().M(), Lumiscale);
      }    
    }

    //Efficency
    for(unsigned ie = 0; ie<hadtopLVec.size(); ie++){
     pUtility::FillDouble(myBaseHistgram.hgentopPt_den,hadtopLVec[ie].Pt(),Lumiscale);
     pUtility::FillInt(myBaseHistgram.hEffNJET_den, nJets, Lumiscale); 
     bool fMatch = false;
      for(unsigned je = 0; je<MatchGentopWidCand.size(); je++){
	if(hadtopLVec[ie]!=MatchGentopWidCand[je])continue;
	if(trijet3match[je] || dijet2match[je] || monojetmatch[je]){fMatch = true; break;}
      }
      if(fMatch){
	pUtility::FillDouble(myBaseHistgram.hgentopPt_num,hadtopLVec[ie].Pt(),Lumiscale);
       pUtility::FillInt(myBaseHistgram.hEffNJET_num, nJets, Lumiscale); 
      }
    }
    //from old tagger
    for(unsigned ie = 0; ie<hadtopLVec.size(); ie++){
     pUtility::FillDouble(myBaseHistgram.hgentopPt_old_den,hadtopLVec[ie].Pt(),Lumiscale);
      bool fMatch = false;
      for(unsigned je = 0; je<MatchGentop_old.size(); je++){
	if(hadtopLVec[ie]!=MatchGentop_old[je])continue;
	fMatch = true; break;
      }
      if(fMatch)pUtility::FillDouble(myBaseHistgram.hgentopPt_old_num,hadtopLVec[ie].Pt(),Lumiscale);
    }
    //Purity
    for(unsigned ip = 0; ip<NtopCand.size(); ip++){
      pUtility::FillDouble(myBaseHistgram.hrecotopPt_den,NtopCand[ip].p().Pt(),Lumiscale);  
      pUtility::FillInt(myBaseHistgram.hPurNJET_den, nJets, Lumiscale); 
      bool fMatch = false;
      for(unsigned jp = 0; jp<MatchNtopCand.size(); jp++){
	if(NtopCand[ip].p()!=MatchNtopCand[jp].p())continue;
	if(trijet3match[jp] || dijet2match[jp] || monojetmatch[jp]){fMatch = true; break;}
      }
      if(fMatch){
	pUtility::FillDouble(myBaseHistgram.hrecotopPt_num,NtopCand[ip].p().Pt(),Lumiscale);
	pUtility::FillInt(myBaseHistgram.hPurNJET_num, nJets, Lumiscale); 
      }
    }
    //FakeRate
    if(sample.Contains("ZJetsToNuNu")){
      pUtility::FillDouble(myBaseHistgram.hfakeMET_den, met, Lumiscale);
      pUtility::FillInt(myBaseHistgram.hfakeNJET_den, nJets, Lumiscale);
      if(Ntop.size()) {
	pUtility::FillDouble(myBaseHistgram.hfakeMET_num, met, Lumiscale);
    	pUtility::FillInt(myBaseHistgram.hfakeNJET_num, nJets, Lumiscale);
      }
    }
    //from old tagger
    if(sample.Contains("ZJetsToNuNu")){
      pUtility::FillDouble(myBaseHistgram.hfakeMET_old_den, met, Lumiscale);
      if(nTopsLVec_old.size()) pUtility::FillDouble(myBaseHistgram.hfakeMET_old_num, met, Lumiscale);
    }

 }//event loop
  (myBaseHistgram.oFile)->Write();
  return 0;
}
