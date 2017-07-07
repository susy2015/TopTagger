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
#include <math.h>
#include <memory>

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

        const std::vector<int>& qgMult  = tr.getVec<int>("qgMult");
        const std::vector<double>& qgPtD   = tr.getVec<double>("qgPtD");
        const std::vector<double>& qgAxis1 = tr.getVec<double>("qgAxis2");
        const std::vector<double>& qgAxis2 = tr.getVec<double>("qgAxis2");

        const std::vector<double>& recoJetschargedHadronEnergyFraction = tr.getVec<double>("recoJetschargedHadronEnergyFraction");
        const std::vector<double>& recoJetschargedEmEnergyFraction     = tr.getVec<double>("recoJetschargedEmEnergyFraction");
        const std::vector<double>& recoJetsneutralEmEnergyFraction     = tr.getVec<double>("recoJetsneutralEmEnergyFraction");
        const std::vector<double>& recoJetsmuonEnergyFraction          = tr.getVec<double>("recoJetsmuonEnergyFraction");
        const std::vector<double>& PhotonEnergyFraction                = tr.getVec<double>("PhotonEnergyFraction");
        const std::vector<double>& ElectronEnergyFraction              = tr.getVec<double>("ElectronEnergyFraction");
        const std::vector<double>& ChargedHadronMultiplicity           = tr.getVec<double>("ChargedHadronMultiplicity");
        const std::vector<double>& NeutralHadronMultiplicity           = tr.getVec<double>("NeutralHadronMultiplicity");
        const std::vector<double>& PhotonMultiplicity                  = tr.getVec<double>("PhotonMultiplicity");
        const std::vector<double>& ElectronMultiplicity                = tr.getVec<double>("ElectronMultiplicity");
        const std::vector<double>& MuonMultiplicity                    = tr.getVec<double>("MuonMultiplicity");
        //const std::vector<double>& JetProba_0                          = tr.getVec<double>("JetProba_0");
        //const std::vector<double>& JetProbaN_0                         = tr.getVec<double>("JetProbaN_0");
        //const std::vector<double>& JetProbaP_0                         = tr.getVec<double>("JetProbaP_0");
        //const std::vector<double>& JetBprob                            = tr.getVec<double>("JetBprob");
        //const std::vector<double>& JetBprobN                           = tr.getVec<double>("JetBprobN");
        //const std::vector<double>& JetBprobP                           = tr.getVec<double>("JetBprobP");
        const std::vector<double>& recoJetsCharge_0                    = tr.getVec<double>("recoJetsCharge_0");

        const std::vector<TLorentzVector>& genDecayLVec = tr.getVec<TLorentzVector>("genDecayLVec");
        const std::vector<int>& genDecayPdgIdVec        = tr.getVec<int>("genDecayPdgIdVec");
        const std::vector<int>& genDecayIdxVec          = tr.getVec<int>("genDecayIdxVec");
        const std::vector<int>& genDecayMomIdxVec       = tr.getVec<int>("genDecayMomIdxVec");

	//std::vector<TLorentzVector> jetsLVec_forTagger;
        //std::vector<double> recoJetsBtag_forTagger;
        //std::vector<double> qgLikelihood_forTagger;
        //std::vector<double> recoJetsCharge_forTagger;
        //AnaFunctions::prepareJetsForTagger(jetsLVec, recoJetsBtag, jetsLVec_forTagger, recoJetsBtag_forTagger, qgLikelihood, qgLikelihood_forTagger, AnaConsts::pt20Arr);

	const double met=tr.getVar<double>("met");
	const double metphi=tr.getVar<double>("metphi");
	TLorentzVector metLVec; metLVec.SetPtEtaPhiM(met, 0, metphi, 0);
        int cntNJetsPt50Eta24 = AnaFunctions::countJets(jetsLVec, AnaConsts::pt50Eta24Arr);
        int cntNJetsPt30Eta24 = AnaFunctions::countJets(jetsLVec, AnaConsts::pt30Eta24Arr);
        int cntNJetsPt30      = AnaFunctions::countJets(jetsLVec, AnaConsts::pt30Arr);
	int cntCSVS = AnaFunctions::countCSVS(jetsLVec, recoJetsBtag, AnaConsts::cutCSVS, AnaConsts::bTagArr);
        int cntCSVL = AnaFunctions::countCSVS(jetsLVec, recoJetsBtag, AnaConsts::cutCSVL, AnaConsts::bTagArr);
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
        std::vector<TLorentzVector> hadGenTops = ttUtility::GetHadTopLVec(genDecayLVec, genDecayPdgIdVec, genDecayIdxVec, genDecayMomIdxVec);
        std::vector<std::vector<const TLorentzVector*>> hadGenTopDaughters;
        for(const auto& top : hadGenTops)
        {
            hadGenTopDaughters.push_back(ttUtility::GetTopdauLVec(top, genDecayLVec, genDecayPdgIdVec, genDecayIdxVec, genDecayMomIdxVec));
        }
        ttUtility::ConstAK4Inputs myConstAK4Inputs = ttUtility::ConstAK4Inputs(jetsLVec, recoJetsBtag, qgLikelihood, hadGenTops, hadGenTopDaughters);
        myConstAK4Inputs.addQGLVectors(qgMult, qgPtD, qgAxis1, qgAxis2);
        myConstAK4Inputs.addSupplamentalVectors(recoJetschargedHadronEnergyFraction,
                                                recoJetschargedEmEnergyFraction,
                                                recoJetsneutralEmEnergyFraction,
                                                recoJetsmuonEnergyFraction,
                                                PhotonEnergyFraction,
                                                ElectronEnergyFraction,
                                                ChargedHadronMultiplicity,
                                                NeutralHadronMultiplicity,
                                                PhotonMultiplicity,
                                                ElectronMultiplicity,
                                                MuonMultiplicity,
                                                recoJetsCharge_0);

        std::vector<Constituent> constituents = ttUtility::packageConstituents(myConstAK4Inputs);

        //run tagger
        topTagger_->runTagger(constituents);

        //retrieve results
        const TopTaggerResults& ttr = topTagger_->getResults();
        const std::vector<TopObject>& topCands = ttr.getTopCandidates();

        //Get gen matching results

        std::vector<TLorentzVector> genTops = genUtility::GetHadTopLVec(genDecayLVec, genDecayPdgIdVec, genDecayIdxVec, genDecayMomIdxVec);

        std::pair<std::vector<int>, std::pair<std::vector<int>, std::vector<TLorentzVector>>> genMatches = topMatcher_.TopConst(topCands, genDecayLVec, genDecayPdgIdVec, genDecayIdxVec, genDecayMomIdxVec);

        std::vector<int> *genMatchdR = new std::vector<int>();//genMatches.first);
        std::vector<int> *genMatchConst = new std::vector<int>();//genMatches.second.first);
        std::vector<double> *genMatchVec = new std::vector<double>();
        //for(const auto& vec : genMatches.second.second)
        //{
        //    genMatchVec->push_back(vec.Pt());
        //}

        //Class which holds and registers vectors of variables
        //Annoyingly this list of variables to expect is necessary
        VariableHolder<double> vh(tr, allowedVarsD_);

        //prepare a vector of get top pt
        for(auto& genTop : genTops) vh.add("genTopPt", genTop.Pt());

        //prepare reco top quantities
        for(const TopObject& topCand : topCands)
        {
            const auto* bestMatch = topCand.getBestGenTopMatch();
            genMatchdR->push_back(bestMatch !=  nullptr);
            genMatchVec->push_back(bestMatch?(bestMatch->Pt()):(-999.9));

            int NConstMatches = 0;
            for(const auto* constituent : topCand.getConstituents())
            {
                auto iter = constituent->getGenMatches().find(bestMatch);
                if(iter != constituent->getGenMatches().end())
                {
                    ++NConstMatches;
                }
            }
            genMatchConst->push_back(NConstMatches);

            std::map<std::string, double> varMap = ttUtility::createMVAInputs(topCand, AnaConsts::cutCSVS);

            for(auto& var : allowedVarsD_)
            {
                vh.add(var, varMap[var]);
            }
        }

        vh.registerFunctions();

        //register matching vectors
        tr.registerDerivedVec("genTopMatchesVec",        genMatchdR);
        tr.registerDerivedVec("genConstiuentMatchesVec", genMatchConst);
        tr.registerDerivedVec("genConstMatchGenPtVec",   genMatchVec);

        tr.registerDerivedVar("nConstituents", static_cast<int>(constituents.size()));

        //Generate basic MVA selection 
        bool passMVABaseline = true;//met > 100 && cntNJetsPt30 >= 5 && cntCSVS >= 1 && cntCSVL >= 2;//true;//(topCands.size() >= 1) || genMatches.second.second->size() >= 1;
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
        allowedVarsD_ = {"cand_pt", "cand_eta", "cand_phi", "cand_m", "cand_dRMax", "j1_p", "j1_m", "j1_CSV", "j2_p",  "j2_m", "j2_CSV", "j3_p", "j3_m",  "j3_CSV", "dTheta12", "dTheta13", "dTheta23", "j12_m", "j13_m", "j23_m", "j12_m_lab", "j13_m_lab", "j23_m_lab", "genTopPt", "j1_QGL", "j2_QGL", "j3_QGL", "j1_pt_lab", "j1_eta_lab", "j1_phi_lab", "j1_CSV_lab", "j1_QGL_lab", "j2_pt_lab", "j2_eta_lab", "j2_phi_lab", "j2_CSV_lab", "j2_QGL_lab", "j3_pt_lab", "j3_eta_lab", "j3_phi_lab", "j3_CSV_lab", "j3_QGL_lab", "MET", "N_CSV", "dR12_lab", "dR13_lab", "dR23_lab", "dR3_12_lab", "dR2_13_lab", "dR1_23_lab", "j1_qgMult_lab", "j1_qgPtD_lab", "j1_qgAxis1_lab", "j1_qgAxis2_lab", "j2_qgMult_lab", "j2_qgPtD_lab", "j2_qgAxis1_lab", "j2_qgAxs2_lab", "j3_qgMult_lab", "j3_qgPtD_lab", "j3_qgAxis1_lab", "j3_qgAxis2_lab", "dRPtTop", "dRPtW", "sd_n2", "j1_m_lab", "j2_m_lab", "j3_m_lab", "j1_chargedHadEFrac", "j2_chargedHadEFrac", "j3_chargedHadEFrac","j1_chargedEmEFrac", "j2_chargedEmEFrac", "j3_chargedEmEFrac","j1_neutralEmEFrac", "j2_neutralEmEFrac", "j3_neutralEmEFrac","j1_muonEFrac", "j2_muonEFrac", "j3_muonEFrac","j1_photonEFrac", "j2_photonEFrac", "j3_photonEFrac","j1_elecEFrac", "j2_elecEFrac", "j3_elecEFrac","j1_chargedHadMult", "j2_chargedHadMult", "j3_chargedHadMult","j1_neutralHadMult", "j2_neutralHadMult", "j3_neutralHadMult","j1_photonMult", "j2_photonMult", "j3_photonMult","j1_elecMult", "j2_elecMult", "j3_elecMult","j1_muonMult", "j2_muonMult", "j3_muonMult","j1_jetCharge" "j2_jetCharge" "j3_jetCharge"};

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
    vector<pair<std::unique_ptr<MiniTupleMaker>, int>> mtmVec;
    int sumRatio = 0;
    for(size_t pos = 0, iter = 0; pos != string::npos; pos = sampleRatios.find(":", pos + 1), ++iter)
    {
        int splitNum = stoi(sampleRatios.substr((pos)?(pos + 1):(0)));
        sumRatio += splitNum;
        string ofname;
        if(iter == 0)      ofname = outFile + "_division_" + to_string(iter) + "_" + dataSets + "_training" + ".root";
        else if(iter == 1) ofname = outFile + "_division_" + to_string(iter) + "_" + dataSets + "_validation" + ".root";
        else if(iter == 2) ofname = outFile + "_division_" + to_string(iter) + "_" + dataSets + "_test" + ".root";
        else               ofname = outFile + "_division_" + to_string(iter) + "_" + dataSets + ".root";
        mtmVec.emplace_back(std::unique_ptr<MiniTupleMaker>(new MiniTupleMaker(ofname, "slimmedTuple")), splitNum);
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
                        //Get sample lumi weight and correct for the actual number of events 
                        //This needs to happen before we ad sampleWgt to the mini tuple variables to save
                        double weight = file.getWeight() * (double(file.nEvts) / nEvts) * (double(sumRatio) / mtmVec[mtmIndex].second);
                        tr.registerDerivedVar("sampleWgt", weight);

                        //Things to run only on first event
                        if(tr.isFirstEvent())
                        {
                            //Initialize the mini tuple branches, needs to be done after first call of tr.getNextEvent()
                            for(auto& mtm : mtmVec)
                            {
                                auto varSet = prepVars.getVarSet();
                                varSet.insert("sampleWgt");
                                mtm.first->setTupleVars(varSet);
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
			bool passbaseline = passMVABaseline;
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
                            ++NEvtsTotal;
                        }

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

    for(auto& mtm : mtmVec) mtm.first.reset();
}
