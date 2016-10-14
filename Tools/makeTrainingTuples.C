#include "SusyAnaTools/Tools/samples.h"
#include "SusyAnaTools/Tools/NTupleReader.h"
#include "SusyAnaTools/Tools/MiniTupleMaker.h"
#include "SusyAnaTools/Tools/customize.h"
#include "SusyAnaTools/Tools/SATException.h"

#include "TopTagger/TopTagger/include/TopTagger.h"
#include "TopTagger/TopTagger/include/TopTaggerUtilities.h"
#include "TopTagger/TopTagger/include/TopTaggerResults.h"
#include "TopTagger/CfgParser/include/TTException.h"
#include "TaggerUtility.h"
#include "PlotUtility.h"

#include "TTree.h"
#include "TFile.h"
#include "Math/VectorUtil.h"

#include <getopt.h>
#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <map>
#include <set>

class PrepVariables
{
private:
    template<typename T>
    class VariableHolder
    {
    public:
        NTupleReader* tr_;
        std::map<std::string, std::vector<T>*> variables_;
        std::set<std::string> allowedVars_;

        VariableHolder(NTupleReader& tr, const std::set<std::string>& vars) : tr_(&tr), allowedVars_(vars)
        {
            for(const auto& var : allowedVars_) variables_[var] = nullptr;
        }

        void add(const std::string& key, const T& var)
        {
            if(variables_.find(key) == variables_.end() || variables_[key] == nullptr)
            {
                variables_[key] = new std::vector<T>();
            }

            variables_[key]->push_back(var);
        }

        std::set<std::string> getKeys()
        {
            return allowedVars_;
        }

        void registerFunctions()
        {
            for(auto& entry : variables_)
            {
                if(allowedVars_.count(entry.first))
                {
                    if(entry.second == nullptr) entry.second = new std::vector<T>();
                    tr_->registerDerivedVec(entry.first, entry.second);
                }
                else
                {
                    THROW_SATEXCEPTION("You must add variable \"" + entry.first + "\" to allowedVars_");
                }
            }
        }
    };
  
  TopTagger* topTagger_;
  TopCat topMatcher_;
  std::set<std::string> allowedVarsD_, allowedVarsI_, allowedVarsB_;

    void prepVariables(NTupleReader& tr)
    {
        const std::vector<TLorentzVector>& jetsLVec  = tr.getVec<TLorentzVector>("jetsLVec");
        const std::vector<double>& recoJetsBtag      = tr.getVec<double>("recoJetsBtag_0");
        const std::vector<double>& qgLikelihood      = tr.getVec<double>("qgLikelihood");
        const std::vector<double>& recoJetsCharge      = tr.getVec<double>("recoJetsCharge_0");

        const std::vector<TLorentzVector>& genDecayLVec = tr.getVec<TLorentzVector>("genDecayLVec");
        const std::vector<int>& genDecayPdgIdVec        = tr.getVec<int>("genDecayPdgIdVec");
        const std::vector<int>& genDecayIdxVec          = tr.getVec<int>("genDecayIdxVec");
        const std::vector<int>& genDecayMomIdxVec       = tr.getVec<int>("genDecayMomIdxVec");

	std::vector<TLorentzVector> jetsLVec_forTagger;
        std::vector<double> recoJetsBtag_forTagger;
        std::vector<double> qgLikelihood_forTagger;
        std::vector<double> recoJetsCharge_forTagger;
        AnaFunctions::prepareJetsForTagger(jetsLVec, recoJetsBtag, jetsLVec_forTagger, recoJetsBtag_forTagger, qgLikelihood, qgLikelihood_forTagger, recoJetsCharge, recoJetsCharge_forTagger);

	const double met=tr.getVar<double>("met");
	const double metphi=tr.getVar<double>("metphi");
	TLorentzVector metLVec; metLVec.SetPtEtaPhiM(met, 0, metphi, 0);
        int cntNJetsPt50Eta24 = AnaFunctions::countJets(jetsLVec, AnaConsts::pt50Eta24Arr);
        int cntNJetsPt30Eta24 = AnaFunctions::countJets(jetsLVec, AnaConsts::pt30Eta24Arr);
        int cntNJetsPt30      = AnaFunctions::countJets(jetsLVec, AnaConsts::pt30Arr);
	int cntCSVS = AnaFunctions::countCSVS(jetsLVec, recoJetsBtag, AnaConsts::cutCSVS, AnaConsts::bTagArr);
	std::vector<double> * dPhiVec = new std::vector<double>();
        (*dPhiVec) = AnaFunctions::calcDPhi(jetsLVec, metLVec.Phi(), 3, AnaConsts::dphiArr);
	bool passnJets = true;
        if( cntNJetsPt50Eta24 < AnaConsts::nJetsSelPt50Eta24 ) passnJets = false;
        if( cntNJetsPt30Eta24 < AnaConsts::nJetsSelPt30Eta24 ) passnJets = false;
        bool passdPhis = (dPhiVec->at(0) >= AnaConsts::dPhi0_CUT && dPhiVec->at(1) >= AnaConsts::dPhi1_CUT && dPhiVec->at(2) >= AnaConsts::dPhi2_CUT);
	bool passBJets = true;
        if( !( (AnaConsts::low_nJetsSelBtagged == -1 || cntCSVS >= AnaConsts::low_nJetsSelBtagged) && (AnaConsts::high_nJetsSelBtagged == -1 || cntCSVS < AnaConsts::high_nJetsSelBtagged ) ) )passBJets = false;
	bool passMET = (metLVec.Pt() >= AnaConsts::defaultMETcut);

        //New Tagger starts here
        //prep input object (constituent) vector
        std::vector<Constituent> constituents = ttUtility::packageConstituents(jetsLVec_forTagger, recoJetsBtag_forTagger, qgLikelihood_forTagger, recoJetsCharge_forTagger);

        //run tagger
        topTagger_->runTagger(constituents);

        //retrieve results
        const TopTaggerResults& ttr = topTagger_->getResults();
        const std::vector<TopObject>& topCands = ttr.getTopCandidates();

        //Get gen matching results

        std::vector<TLorentzVector> genTops = genUtility::GetHadTopLVec(genDecayLVec, genDecayPdgIdVec, genDecayIdxVec, genDecayMomIdxVec);

        std::pair<std::vector<int>*, std::pair<std::vector<int>*, std::vector<double>*>> genMatches = topMatcher_.TopConst(topCands, genDecayLVec, genDecayPdgIdVec, genDecayIdxVec, genDecayMomIdxVec);

        //Class which holds and registers vectors of variables
        //Annoyingly this list of variables to expect is necessary
        VariableHolder<double> vh(tr, allowedVarsD_);

        //prepare a vector of get top pt
        for(auto& genTop : genTops) vh.add("genTopPt", genTop.Pt());

        //prepare reco top quantities
        for(const TopObject& topCand : topCands)
        {
            //Get top candidate variables
            vh.add("cand_pt",    topCand.p().Pt()   );
            vh.add("cand_eta",   topCand.p().Eta()  );
            vh.add("cand_phi",   topCand.p().Phi()  );
            vh.add("cand_m",     topCand.p().M()    );
            vh.add("cand_dRMax", topCand.getDRmax() );

            //Get Constituents
            const std::vector<Constituent const *>& top_constituents = topCand.getConstituents();
            
	    //Get constituent variables before deboost
	    for(int i = 0; i < top_constituents.size(); ++i)
	      {
                vh.add("j" + std::to_string(i + 1) + "_phi_lab",   top_constituents[i]->p().Phi()         );
                vh.add("j" + std::to_string(i + 1) + "_eta_lab",   top_constituents[i]->p().Eta()         );
                vh.add("j" + std::to_string(i + 1) + "_pt_lab",    top_constituents[i]->p().Pt()         );
	      }

            std::vector<Constituent> RF_constituents;

            for(const auto& constitutent : top_constituents)
            {
                TLorentzVector p4(constitutent->p());
                p4.Boost(-topCand.p().BoostVector());
                RF_constituents.emplace_back(p4, constitutent->getBTagDisc(), constitutent->getQGLikelihood(), constitutent->getJetCharge());
            }
            
            //re-sort constituents by p after deboosting
            std::sort(RF_constituents.begin(), RF_constituents.end(), [](const Constituent& c1, const Constituent& c2){ return c1.p().P() > c2.p().P(); });

            //Get constituent variables
            //std::cout << "\tconst vec size: " << constituents.size() << std::endl;
            for(int i = 0; i < RF_constituents.size(); ++i)
            {
                vh.add("j" + std::to_string(i + 1) + "_p",     RF_constituents[i].p().P()           );
                vh.add("j" + std::to_string(i + 1) + "_theta", RF_constituents[i].p().Angle(topCand.p().Vect())         );
                vh.add("j" + std::to_string(i + 1) + "_phi",   RF_constituents[i].p().Phi()         );
		vh.add("j" + std::to_string(i + 1) + "_eta",   RF_constituents[i].p().Eta()         );
		vh.add("j" + std::to_string(i + 1) + "_pt",    RF_constituents[i].p().Pt()         );
		vh.add("j" + std::to_string(i + 1) + "_m",     RF_constituents[i].p().M()           );
                vh.add("j" + std::to_string(i + 1) + "_CSV",   RF_constituents[i].getBTagDisc()     );
                vh.add("j" + std::to_string(i + 1) + "_QGL",   RF_constituents[i].getQGLikelihood() );
                vh.add("j" + std::to_string(i + 1) + "_Chrg",  RF_constituents[i].getJetCharge() );

                //index of next jet (assumes < 4 jets)
                int iNext = (i + 1) % RF_constituents.size();
                int iMin = std::min(i, iNext);
                int iMax = std::max(i, iNext);

                //Calculate delta angle variables
                double dR   = ROOT::Math::VectorUtil::DeltaR(RF_constituents[iMin].p(), RF_constituents[iMax].p());
                double dPhi = ROOT::Math::VectorUtil::DeltaPhi(RF_constituents[iMin].p(), RF_constituents[iMax].p());
                double dEta = RF_constituents[iMin].p().Eta() - RF_constituents[iMax].p().Eta();
                //vh.add("dR"   + std::to_string(iMin + 1) + std::to_string(iMax + 1), dR);
                //vh.add("dPhi" + std::to_string(iMin + 1) + std::to_string(iMax + 1), dPhi);
                vh.add("dTheta" + std::to_string(iMin + 1) + std::to_string(iMax + 1), RF_constituents[iMin].p().Angle(RF_constituents[iMax].p().Vect()));
                //vh.add("dEta" + std::to_string(iMin + 1) + std::to_string(iMax + 1), dEta);

                //calculate pair masses
                //int iNNext = (iNext + 1) % RF_constituents.size();
                auto jetPair = RF_constituents[i].p() + RF_constituents[iNext].p();
                vh.add("j"   + std::to_string(iMin + 1) + std::to_string(iMax + 1) + "_m", jetPair.M());

                TLorentzVector j1 = RF_constituents[i].p();
                j1.Boost(-jetPair.BoostVector());
                TLorentzVector j2 = RF_constituents[iNext].p();
                j2.Boost(-jetPair.BoostVector());
                vh.add("j"   + std::to_string(iMin + 1) + std::to_string(iMax + 1) + "_dTheta", jetPair.Angle(j1.Vect()));
                
                //vh.add("j"   + std::to_string(iMin + 1) + std::to_string(iMax + 1) + "_pt", jetPair.Pt());
                //vh.add("j"   + std::to_string(iMin + 1) + std::to_string(iMax + 1) + "j" + std::to_string(iNNext + 1) +  "_dR", ROOT::Math::VectorUtil::DeltaR(jetPair, RF_constituents[iNNext].p()));
                //vh.add("j"   + std::to_string(iMin + 1) + std::to_string(iMax + 1) + "j" + std::to_string(iNNext + 1) +  "_dR", jetPair.Angle(RF_constituents[iNNext].p().Vect()));
            }
        }

        vh.registerFunctions();

        //register matching vectors
        tr.registerDerivedVec("genTopMatchesVec",        genMatches.first);
        tr.registerDerivedVec("genConstiuentMatchesVec", genMatches.second.first);
        tr.registerDerivedVec("genConstMatchGenPtVec", genMatches.second.second);

        tr.registerDerivedVar("nConstituents", static_cast<int>(constituents.size()));

        //Generate basic MVA selection 
        bool passMVABaseline = true;//(topCands.size() >= 1) || genMatches.second.second->size() >= 1;
        tr.registerDerivedVar("passMVABaseline", passMVABaseline);
	const bool passValidationBaseline = cntNJetsPt30>=AnaConsts::nJetsSelPt30Eta24 && met>=AnaConsts::defaultMETcut && cntCSVS>=AnaConsts::low_nJetsSelBtagged;
	tr.registerDerivedVar("passValidationBaseline",passValidationBaseline);
	tr.registerDerivedVar("MET", met);
	tr.registerDerivedVar("Njet",cntNJetsPt30);
	tr.registerDerivedVar("Bjet", cntCSVS);
	tr.registerDerivedVar("passnJets", passnJets);
	tr.registerDerivedVar("passMET", passMET);
	tr.registerDerivedVar("passdPhis", passdPhis);
	tr.registerDerivedVar("passBJets", passBJets);
	
    }

public:
    PrepVariables()
    {
        topTagger_ = new TopTagger();
        topTagger_->setCfgFile("TopTaggerClusterOnly.cfg");

        //double variables list here
        //allowedVarsD_ = {"cand_pt", "cand_eta", "cand_phi", "cand_m", "cand_dRMax", "j1_pt", "j1_eta", "j1_phi", "j1_m", "j1_CSV", "j2_pt", "j2_eta", "j2_phi", "j2_m", "j2_CSV", "j3_pt", "j3_eta", "j3_phi", "j3_m",  "j3_CSV", "dR12", "dEta12", "dPhi12", "dR13", "dEta13", "dPhi13", "dR23", "dEta23", "dPhi23", "j12_m", "j13_m", "j23_m", "j12_pt", "j13_pt", "j23_pt", "j12j3_dR", "j13j2_dR", "j23j1_dR", "genTopPt", "j1_QGL", "j2_QGL", "j3_QGL" , "MET"};
        allowedVarsD_ = {"cand_pt", "cand_eta", "cand_phi", "cand_m", "cand_dRMax", "j1_p", "j1_theta", "j1_pt", "j1_eta", "j1_phi", "j1_m", "j1_CSV", "j2_p", "j2_theta",  "j2_pt", "j2_eta", "j2_phi", "j2_m", "j2_CSV", "j3_p", "j3_theta", "j3_pt", "j3_eta", "j3_phi", "j3_m",  "j3_CSV", "dTheta12", "dTheta13", "dTheta23", "j12_m", "j13_m", "j23_m", "j12_dTheta", "j23_dTheta", "j13_dTheta", "genTopPt", "j1_QGL", "j2_QGL", "j3_QGL", "j1_Chrg", "j2_Chrg", "j3_Chrg", "j1_pt_lab", "j1_eta_lab", "j1_phi_lab","j2_pt_lab", "j2_eta_lab", "j2_phi_lab", "j3_pt_lab", "j3_eta_lab", "j3_phi_lab","MET"};
        //integer values list here
        allowedVarsI_ = {"genTopMatchesVec", "genConstiuentMatchesVec", "genConstMatchGenPtVec", "Njet", "Bjet"};
	//boolean values list here    
	allowedVarsB_ = {"passnJets", "passMET", "passdPhis", "passBJets"};
    }

    std::set<std::string> getVarSet()
    {
        //this is dumb
        std::set<std::string> allowedVars = allowedVarsD_;
        for(const auto& var : allowedVarsI_) allowedVars.insert(var);
        for(const auto& var : allowedVarsB_) allowedVars.insert(var);
        return allowedVars;
    }

    void operator()(NTupleReader& tr)
    {
        prepVariables(tr);
    }
};

int main(int argc, char* argv[])
{
    using namespace std;

    int opt;
    int option_index = 0;
    static struct option long_options[] = {
        {"fakerate",           no_argument, 0, 'f'},
	{"condor",           no_argument, 0, 'c'},
        {"dataSets",   required_argument, 0, 'D'},
        {"numFiles",   required_argument, 0, 'N'},
        {"startFile",  required_argument, 0, 'M'},
        {"numEvts",    required_argument, 0, 'E'},
        {"ratio"  ,    required_argument, 0, 'R'},
    };
    bool forFakeRate = false;
    bool runOnCondor = false;
    string dataSets = "", sampleloc = AnaSamples::fileDir, outFile = "trainingTuple", sampleRatios = "1:1";
    int nFiles = -1, startFile = 0, nEvts = -1, printInterval = 10000;

    while((opt = getopt_long(argc, argv, "fcD:N:M:E:R:", long_options, &option_index)) != -1)
    {
        switch(opt)
        {
        case 'f':
            forFakeRate = true;
            break;

        case 'c':
            runOnCondor = true;
            break;

        case 'D':
            dataSets = optarg;
            break;

        case 'N':
            nFiles = int(atoi(optarg));
            break;

        case 'M':
            startFile = int(atoi(optarg));
            break;

        case 'E':
            nEvts = int(atoi(optarg)) - 1;
            break;

        case 'R':
            sampleRatios = optarg;
            break;
        }
    }

    //if running on condor override all optional settings
    if(runOnCondor)
    {
        char thistFile[128];
        sprintf(thistFile, "trainingTuple_%s_%d", dataSets.c_str(), startFile);
        outFile = thistFile;
        sampleloc = "condor";
    }

    AnaSamples::SampleSet        ss(sampleloc);
    AnaSamples::SampleCollection sc(ss);

    map<string, vector<AnaSamples::FileSummary>> fileMap;

    //Select approperiate datasets here
    if(dataSets.compare("TEST") == 0)
    {
        fileMap["DYJetsToLL"]  = {ss["DYJetsToLL_HT_600toInf"]};
        fileMap["ZJetsToNuNu"] = {ss["ZJetsToNuNu_HT_2500toInf"]};
        fileMap["DYJetsToLL_HT_600toInf"] = {ss["DYJetsToLL_HT_600toInf"]};
        fileMap["ZJetsToNuNu_HT_2500toInf"] = {ss["ZJetsToNuNu_HT_2500toInf"]};
        fileMap["TTbarDiLep"] = {ss["TTbarDiLep"]};
        fileMap["TTbarNoHad"] = {ss["TTbarDiLep"]};
    }
    else
    {
        if(ss[dataSets] != ss.null())
        {
            fileMap[dataSets] = {ss[dataSets]};
            //for(const auto& colls : ss[dataSets].getCollections())
            //{
            //    fileMap[colls] = {ss[dataSets]};
            //}
        }
        else if(sc[dataSets] != sc.null())
        {
            fileMap[dataSets] = {sc[dataSets]};
        }
    }

    //parse sample splitting and set up minituples
    vector<pair<MiniTupleMaker *, int>> mtmVec;
    for(size_t pos = 0, iter = 0; pos != string::npos; pos = sampleRatios.find(":", pos + 1), ++iter)
    {
        int splitNum = stoi(sampleRatios.substr((pos)?(pos + 1):(0)));
        string ofname;
        if(iter == 0)      ofname = outFile + "_division_" + to_string(iter) + "_" + dataSets + "_training" + ".root";
        else if(iter == 1) ofname = outFile + "_division_" + to_string(iter) + "_" + dataSets + "_validation" + ".root";
        else               ofname = outFile + "_division_" + to_string(iter) + "_" + dataSets + ".root";
        mtmVec.emplace_back(new MiniTupleMaker(ofname, "slimmedTuple"), splitNum);
    }

    for(auto& fileVec : fileMap)
    {
        for(auto& file : fileVec.second)
        {
            int startCount = 0, fileCount = 0, NEvtsTotal = 0;

            file.readFileList();

            for(const std::string& fname : file.filelist_)
            {
                if(startCount++ < startFile) continue;
                if(nFiles > 0 && fileCount++ >= nFiles) break;

                if(nFiles > 0) NEvtsTotal = 0;
                else if(nEvts >= 0 && NEvtsTotal > nEvts) break;

                //open input file and tree
                TFile *f = TFile::Open(fname.c_str());

                if(!f)
                {
                    std::cout << "File \"" << fname << "\" not found!!!!!!" << std::endl;
                    continue;
                }
                TTree *t = (TTree*)f->Get(file.treePath.c_str());

                if(!t)
                {
                    std::cout << "Tree \"" << file.treePath << "\" not found in file \"" << fname << "\"!!!!!!" << std::endl;
                    continue;
                }
                std::cout << "\t" << fname << std::endl;

                try
                {
                    //Don't bother with activateBranches, take advantage of new on-the-fly branch allocation
                    NTupleReader tr(t);

                    //register variable prep class with NTupleReader
                    PrepVariables prepVars;
                    tr.registerFunction(prepVars);

                    int splitCounter = 0, mtmIndex = 0;

                    while(tr.getNextEvent())
                    {
                        //Things to run only on first event
                        if(tr.isFirstEvent())
                        {
                            //Initialize the mini tuple branches, needs to be done after first call of tr.getNextEvent()
                            for(auto& mtm : mtmVec)
                            {
                                mtm.first->setTupleVars(prepVars.getVarSet());
                                mtm.first->initBranches(tr);
                            }
                        }

                        //If nEvts is set, stop after so many events
                        if(nEvts > 0 && NEvtsTotal > nEvts) break;
                        if(tr.getEvtNum() % printInterval == 0) std::cout << "Event #: " << tr.getEvtNum() << std::endl;

                        //Get cut variable 
                        const bool& passMVABaseline = tr.getVar<bool>("passMVABaseline");
			const bool& passValidationBaseline = tr.getVar<bool>("passValidationBaseline");
			//fill mini tuple
			bool passbaseline = forFakeRate? passValidationBaseline : passMVABaseline;
			// if(passMVABaseline)
			if(passbaseline)
                        {
                            mtmVec[mtmIndex].first->fill();
                            ++splitCounter;
                            if(splitCounter == mtmVec[mtmIndex].second)
                            {
                                splitCounter = 0;
                                mtmIndex = (mtmIndex + 1)%mtmVec.size();
                            }
                        }

                        ++NEvtsTotal;
                    }
                }
                catch(const SATException& e)
                {
                    cout << e << endl;
                    throw;
                }
                catch(const TTException& e)
                {
                    cout << e << endl;
                    throw;
                }
                catch(const string& e)
                {
                    cout << e << endl;
                    throw;
                }
            }
        }
    }

    for(auto& mtm : mtmVec) delete mtm.first;
}
