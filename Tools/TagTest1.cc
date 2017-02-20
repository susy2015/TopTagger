#include "SusyAnaTools/Tools/customize.h"
#include "SusyAnaTools/Tools/baselineDef.h"
#include "SusyAnaTools/Tools/NTupleReader.h"
#include "TopTagger/TopTagger/include/TopTagger.h"
#include "TopTagger/TopTagger/include/TopTaggerResults.h"
#include "TopTagger/TopTagger/include/TopTaggerUtilities.h"

#include "TaggerUtility.h"
#include "PlotUtility.h"
#include "TagTest1.h"
#include <map>
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
  std::string spec = "TagTest";
  ExpBaselineVessel = new BaselineVessel(*static_cast<NTupleReader*>(nullptr), spec);
  AnaFunctions::prepareForNtupleReader();
  //AnaFunctions::prepareTopTagger();
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
  // Loop over the events (tree entries)
  while(tr->getNextEvent()){
    if(maxevent>=0 && tr->getEvtNum() > maxevent ) break;
    // Add print out of the progress of looping
    if( tr->getEvtNum()-1 == 0 || tr->getEvtNum() == entries || (tr->getEvtNum()-1)%(entries/10) == 0 ) std::cout<<"\n   Processing the "<<tr->getEvtNum()-1<<"th event ..."<<std::endl;
    ev++;
    // std::cout<<"Event: "<<ev<<std::endl;
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
    const vector<TLorentzVector> &jetsForTopLVec = tr->getVec<TLorentzVector>("jetsLVec_forTagger"+spec);
    const vector<double> &recoJetsBtag_ForTop = tr->getVec<double>("recoJetsBtag_forTagger"+spec);
    //cut    
    //if(!(passMET && passNJET && passBJET))continue;
    if(!(passNJET && passMET))continue;
    vector<TLorentzVector> hadtopLVec = genUtility::GetHadTopLVec(genDecayLVec, genDecayPdgIdVec, genDecayIdxVec, genDecayMomIdxVec);

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
    vector<TopObject> NtopCand = ttr.getTopCandidates();
    //std::cout<<"Ntop: "<<Ntop.size()<<" NtopCand: "<<NtopCand.size()<<std::endl;
    //matching
    bool topmatch = false;
    bool topmatchCand = false;
    bool topmatch_old = false;
    vector<TopObject> MatchNtop;
    vector<TopObject> MatchNtopCand;
    vector<TLorentzVector> MatchGentop; 
    vector<TLorentzVector> MatchGentopWidCand; 
     
    if(topcat.GetMatchedTop(Ntop, MatchNtop, hadtopLVec, MatchGentop)) topmatch = true;//final top match
    if(topcat.GetMatchedTop(NtopCand, MatchNtopCand, hadtopLVec, MatchGentopWidCand)) topmatchCand = true;//topcand match
    
    //constituent matching
    vector<bool> monojetmatch(Ntop.size(),false);
    vector<bool> dijet2match(Ntop.size(),false);
    vector<bool> trijet3match(Ntop.size(),false);
    vector<bool> trijet2match(Ntop.size(),false);
    vector<bool> trijet1match(Ntop.size(),false);

    std::pair<std::vector<int>, std::pair<std::vector<int>, std::vector<TLorentzVector>>> genMatches = topcat.TopConst(Ntop, genDecayLVec,genDecayPdgIdVec, genDecayIdxVec, genDecayMomIdxVec);
    std::vector<int> *constmatch = new std::vector<int>(genMatches.second.first);
    std::vector<TLorentzVector> *ConstmatchGentop = new std::vector<TLorentzVector>(genMatches.second.second);

      for(unsigned tc = 0; tc<Ntop.size(); tc++){
	std::vector<Constituent const*> topconst = Ntop[tc]->getConstituents();
	if(topconst.size()==1 && (*constmatch)[tc]==1) monojetmatch[tc]=true;
	if(topconst.size()==2){
	  if((*constmatch)[tc]==2)dijet2match[tc] = true;
	}
	if(topconst.size()==3){
	  if((*constmatch)[tc]==3)trijet3match[tc] = true;
	  if((*constmatch)[tc]==2)trijet2match[tc] = true;
	  if((*constmatch)[tc]==1)trijet1match[tc] = true;
	}
      }
    
 vector<TopObject*> ConstMatchNtop;
 if(MatchNtop.size()){
   for(unsigned cm = 0; cm<Ntop.size(); cm++){
     if(trijet3match[cm] || dijet2match[cm] || monojetmatch[cm])ConstMatchNtop.push_back(&MatchNtop[cm]);
   }
 }
    pUtility::FillInt(myBaseHistgram.hNtop,Ntop.size(),Lumiscale);
    pUtility::Fill2D(myBaseHistgram.hNjet_Ntop,double(nJets), double(Ntop.size()),Lumiscale);
    pUtility::FillInt(myBaseHistgram.hNtopCand,NtopCand.size(),Lumiscale);   
    pUtility::FillInt(myBaseHistgram.hMatchedNtop,MatchNtop.size(),Lumiscale);
     if(Ntop.size()){         
       pUtility::FillInt(myBaseHistgram.hMatchedNtopCand,MatchNtop.size(),Lumiscale);
       pUtility::FillInt(myBaseHistgram.hConstMatchedNtop,ConstMatchNtop.size(),Lumiscale);    
     }       
   

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
      for(unsigned je = 0; je<ConstmatchGentop->size(); je++){
	if(hadtopLVec[ie]!=(*ConstmatchGentop)[je])continue;
	if(trijet3match[je] || dijet2match[je] || monojetmatch[je]){
	fMatch = true; break;
	}
      }
      if(fMatch){
	pUtility::FillDouble(myBaseHistgram.hgentopPt_num,hadtopLVec[ie].Pt(),Lumiscale);
	pUtility::FillInt(myBaseHistgram.hEffNJET_num, nJets, Lumiscale); 
      }
    }

    //trijet2match
    for(unsigned ie = 0; ie<hadtopLVec.size(); ie++){
      pUtility::FillDouble(myBaseHistgram.hgentopPt_2match_den,hadtopLVec[ie].Pt(),Lumiscale);
      bool fMatch = false;
      for(unsigned je = 0; je<ConstmatchGentop->size(); je++){
        if(hadtopLVec[ie]!=(*ConstmatchGentop)[je])continue;
	if(trijet2match[je]){
	  fMatch = true; break;
        }
      }
      if(fMatch){
	pUtility::FillDouble(myBaseHistgram.hgentopPt_2match_num,hadtopLVec[ie].Pt(),Lumiscale);
      }
    }
    //trijet1match
    for(unsigned ie = 0; ie<hadtopLVec.size(); ie++){
      pUtility::FillDouble(myBaseHistgram.hgentopPt_1match_den,hadtopLVec[ie].Pt(),Lumiscale);
      bool fMatch = false;
      for(unsigned je = 0; je<ConstmatchGentop->size(); je++){
        if(hadtopLVec[ie]!=(*ConstmatchGentop)[je])continue;
	if(trijet1match[je]){
	  fMatch = true; break;
        }
      }
      if(fMatch){
	pUtility::FillDouble(myBaseHistgram.hgentopPt_1match_num,hadtopLVec[ie].Pt(),Lumiscale);
      }
    }
    

    //Purity
    for(unsigned ip = 0; ip<Ntop.size(); ip++){
      pUtility::FillDouble(myBaseHistgram.hrecotopPt_den,Ntop[ip]->p().Pt(),Lumiscale);  
      pUtility::FillDouble(myBaseHistgram.hrecotopEta_den,Ntop[ip]->p().Eta(),Lumiscale);
      pUtility::FillDouble(myBaseHistgram.hrecotopDRmax_den,Ntop[ip]->getDRmax(),Lumiscale);    
      pUtility::FillInt(myBaseHistgram.hPurNJET_den, nJets, Lumiscale); 
      bool fMatch = false;
      if(trijet3match[ip] || dijet2match[ip] || monojetmatch[ip]){fMatch = true; break;}
      if(fMatch){
	pUtility::FillDouble(myBaseHistgram.hrecotopPt_num,Ntop[ip]->p().Pt(),Lumiscale);
	pUtility::FillDouble(myBaseHistgram.hrecotopEta_num,Ntop[ip]->p().Eta(),Lumiscale);
	pUtility::FillDouble(myBaseHistgram.hrecotopDRmax_num,Ntop[ip]->getDRmax(),Lumiscale);
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
    
    //Intrinsic Ineff
    for(unsigned ie = 0; ie<hadtopLVec.size(); ie++){
      pUtility::FillDouble(myBaseHistgram.hgentopPt_Ineff_den,hadtopLVec[ie].Pt(),Lumiscale);
      vector<TLorentzVector>genhadtopdauLVec = genUtility::GetTopdauLVec(hadtopLVec[ie], genDecayLVec, genDecayPdgIdVec, genDecayIdxVec, genDecayMomIdxVec);
      double DeltaR = 0.4;
      int Ineff = 0;
      for(unsigned id = 0; id<genhadtopdauLVec.size(); id++){
	bool match = false;
	double deltaRMin = 100000.;
	unsigned tid = -1;
	for(unsigned ij = 0; ij<jetsLVec.size(); ij++){
	  const double dr = genhadtopdauLVec[id].DeltaR(jetsLVec.at(ij));
	  if( dr < deltaRMin ){ 
	    deltaRMin = dr;
	    tid = ij;
	  }
	}
	if(deltaRMin < DeltaR) {
	  match = true;
	  if(jetsLVec[tid].Pt()<30) Ineff++;
	}
      }
      if(Ineff)pUtility::FillDouble(myBaseHistgram.hgentopPt_Ineff_num,hadtopLVec[ie].Pt(),Lumiscale);
    }

 }//event loop
  (myBaseHistgram.oFile)->Write();
  return 0;
}
