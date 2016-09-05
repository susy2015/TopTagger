#include "SusyAnaTools/Tools/samples.h"
#include "SusyAnaTools/Tools/NTupleReader.h"
#include "SusyAnaTools/Tools/MiniTupleMaker.h"
#include "SusyAnaTools/Tools/customize.h"
#include "SusyAnaTools/Tools/SATException.h"

#include "TopTagger/TopTagger/include/TopTagger.h"
#include "TopTagger/TopTagger/include/TopTaggerUtilities.h"
#include "TopTagger/TopTagger/include/TopTaggerResults.h"
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
    std::set<std::string> allowedVarsD_, allowedVarsI_;

    void prepVariables(NTupleReader& tr)
    {
        const std::vector<TLorentzVector>& jetsLVec  = tr.getVec<TLorentzVector>("jetsLVec");
        const std::vector<double>& recoJetsBtag      = tr.getVec<double>("recoJetsBtag_0");

        const std::vector<TLorentzVector>& genDecayLVec = tr.getVec<TLorentzVector>("genDecayLVec");
        const std::vector<int>& genDecayPdgIdVec        = tr.getVec<int>("genDecayPdgIdVec");
        const std::vector<int>& genDecayIdxVec          = tr.getVec<int>("genDecayIdxVec");
        const std::vector<int>& genDecayMomIdxVec       = tr.getVec<int>("genDecayMomIdxVec");

        std::vector<TLorentzVector> jetsLVec_forTagger;
        std::vector<double> recoJetsBtag_forTagger;
        AnaFunctions::prepareJetsForTagger(jetsLVec, recoJetsBtag, jetsLVec_forTagger, recoJetsBtag_forTagger);

        //New Tagger starts here
        //prep input object (constituent) vector
        std::vector<Constituent> constituents = ttUtility::packageConstituents(jetsLVec_forTagger, recoJetsBtag_forTagger);

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
            const std::vector<Constituent const *>& constituents = topCand.getConstituents();

            //Get constituent variables
            //std::cout << "\tconst vec size: " << constituents.size() << std::endl;
            for(int i = 0; i < constituents.size(); ++i)
            {
                vh.add("j" + std::to_string(i + 1) + "_pt",    constituents[i]->p().Pt()      );
                vh.add("j" + std::to_string(i + 1) + "_eta",   constituents[i]->p().Eta()     );
                vh.add("j" + std::to_string(i + 1) + "_phi",   constituents[i]->p().Phi()     );
                vh.add("j" + std::to_string(i + 1) + "_m",     constituents[i]->p().M()       );
                vh.add("j" + std::to_string(i + 1) + "_CSV",   constituents[i]->getBTagDisc() );

                //index of next jet (assumes < 4 jets)
                int iNext = (i + 1) % constituents.size();
                int iMin = std::min(i, iNext);
                int iMax = std::max(i, iNext);

                //Calculate delta angle variables
                double dR   = ROOT::Math::VectorUtil::DeltaR(constituents[iMin]->p(), constituents[iMax]->p());
                double dPhi = ROOT::Math::VectorUtil::DeltaPhi(constituents[iMin]->p(), constituents[iMax]->p());
                double dEta = constituents[iMin]->p().Eta() - constituents[iMax]->p().Eta();
                vh.add("dR"   + std::to_string(iMin + 1) + std::to_string(iMax + 1), dR);
                vh.add("dPhi" + std::to_string(iMin + 1) + std::to_string(iMax + 1), dPhi);
                vh.add("dEta" + std::to_string(iMin + 1) + std::to_string(iMax + 1), dEta);

                //calculate pair masses
                vh.add("j"   + std::to_string(iMin + 1) + std::to_string(iMax + 1) + "_m", (constituents[i]->p() + constituents[iNext]->p()).M());
            }
        }

        vh.registerFunctions();

        //register matching vectors
        tr.registerDerivedVec("genTopMatchesVec",        genMatches.first);
        tr.registerDerivedVec("genConstiuentMatchesVec", genMatches.second.first);
        tr.registerDerivedVec("genConstMatchGenPtVec", genMatches.second.second);

        //Generate basic MVA selection 
        //for now I just require that there is at least 1 top candidate 
        bool passMVABaseline = (topCands.size() >= 1) || genMatches.second.second->size() >= 1;
        tr.registerDerivedVar("passMVABaseline", passMVABaseline);
    }

public:
    PrepVariables()
    {
        topTagger_ = new TopTagger();
        topTagger_->setCfgFile("TopTaggerClusterOnly.cfg");

        //double variables list here
        allowedVarsD_ = {"cand_pt", "cand_eta", "cand_phi", "cand_m", "cand_dRMax", "j1_pt", "j1_eta", "j1_phi", "j1_m", "j1_CSV", "j2_pt", "j2_eta", "j2_phi", "j2_m", "j2_CSV", "j3_pt", "j3_eta", "j3_phi", "j3_m",  "j3_CSV", "dR12", "dEta12", "dPhi12", "dR13", "dEta13", "dPhi13", "dR23", "dEta23", "dPhi23", "j12_m", "j13_m", "j23_m", "genTopPt"};
        //integer valuse list here
        allowedVarsI_ = {"genTopMatchesVec", "genConstiuentMatchesVec", "genConstMatchGenPtVec"};
    }

    std::set<std::string> getVarSet()
    {
        //this is dumb
        std::set<std::string> allowedVars = allowedVarsD_;
        for(const auto& var : allowedVarsI_) allowedVars.insert(var);
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
        {"condor",           no_argument, 0, 'c'},
        {"dataSets",   required_argument, 0, 'D'},
        {"numFiles",   required_argument, 0, 'N'},
        {"startFile",  required_argument, 0, 'M'},
        {"numEvts",    required_argument, 0, 'E'},
        {"ratio"  ,    required_argument, 0, 'R'},
    };

    bool runOnCondor = false;
    string dataSets = "", sampleloc = AnaSamples::fileDir, outFile = "trainingTuple", sampleRatios = "1:1";
    int nFiles = -1, startFile = 0, nEvts = -1, printInterval = 10000;

    while((opt = getopt_long(argc, argv, "cD:N:M:E:R:", long_options, &option_index)) != -1)
    {
        switch(opt)
        {
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
        string ofname = outFile + "_division_" + to_string(iter) + "_" + dataSets + ".root";
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

                        //fill mini tuple
                        if(passMVABaseline)
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
                catch(const SATException e)
                {
                    cout << e << endl;
                    throw;
                }
                catch(const string e)
                {
                    cout << e << endl;
                    throw;
                }
            }
        }
    }

    for(auto& mtm : mtmVec) delete mtm.first;
}
