#include "SusyAnaTools/Tools/samples.h"
#include "SusyAnaTools/Tools/NTupleReader.h"
#include "SusyAnaTools/Tools/MiniTupleMaker.h"
#include "SusyAnaTools/Tools/customize.h"
#include "SusyAnaTools/Tools/SATException.h"

#include "Framework/Framework/include/MakeMVAVariables.h"
#include "Framework/Framework/include/Jet.h"
#include "Framework/Framework/include/Muon.h"
#include "Framework/Framework/include/Electron.h"
#include "Framework/Framework/include/BJet.h"
#include "Framework/Framework/include/CommonVariables.h"
#include "Framework/Framework/include/Baseline.h"

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
#include <limits>

#include "hdf5.h"

class HDF5Writer
{
private:
    int nEvtsPerFile_, nEvts_, nFile_;
    std::string ofname_;
    std::map<std::string, std::vector<std::string>> variables_;
    std::map<std::string, std::vector<const char *>> variablesPtr_;
    std::map<std::string, std::vector<std::pair<bool,const  void*>>> pointers_;
    std::map<std::string, std::vector<float>> data_;

public:
    HDF5Writer(const std::map<std::string, std::vector<std::string>>& variables, int eventsPerFile, const std::string& ofname) : variables_(variables), nEvtsPerFile_(eventsPerFile), nEvts_(0), nFile_(0), ofname_(ofname)
    {
        for(const auto& varVec : variables_)
        {
            auto& ptrVec = variablesPtr_[varVec.first];
            for(const auto& str : varVec.second)
            {
                ptrVec.push_back(str.c_str());
            }
        }
    }

    void setTupleVars(const std::set<std::string>&) {}

    void initBranches(const NTupleReader& tr)
    {
        for(const auto& dataset : variables_)
        {
            auto& ptrPair = pointers_[dataset.first];
            ptrPair.clear();
            for(const auto& var : dataset.second)
            {
                std::string type;
                tr.getType(var, type);
                if(type.find("vector") != std::string::npos) 
                {
                    ptrPair.push_back(std::make_pair(true, tr.getVecPtr(var)));
                }
                else
                {
                    ptrPair.push_back(std::make_pair(false, tr.getPtr(var)));
                }
            }
        }
    }

    void saveHDF5File()
    {
        herr_t      status;
        std::vector<char*> attr_data;

        std::string fileName(ofname_, 0, ofname_.find("."));
        fileName += "_" + std::to_string(nFile_) + ".h5";
        ++nFile_;

        /* Open an existing file. */
        hid_t file_id = H5Fcreate(fileName.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

        for(const auto& data : data_)
        {
            const float* dset_data = data.second.data();
            /* Create the data space for the dataset. */
            hsize_t dims[2];
            dims[1] = variables_[data.first].size();
            dims[0] = data.second.size()/dims[1];
            hid_t dataspace_id = H5Screate_simple(2, dims, NULL);

            /* Create the dataset. */
            hid_t dataset_id = H5Dcreate2(file_id, data.first.c_str(), H5T_NATIVE_FLOAT, dataspace_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

            /* Write the dataset. */
            status = H5Dwrite(dataset_id, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT, dset_data);

            //create type
            hid_t vls_type_c_id = H5Tcopy(H5T_C_S1);
            status = H5Tset_size(vls_type_c_id, H5T_VARIABLE);

            /* Create a dataset attribute. */
            dims[0] = variables_[data.first].size();
            hid_t dataspace2_id = H5Screate_simple(1, dims, NULL);

            hid_t attribute_id = H5Acreate2 (dataset_id, "column_headers", vls_type_c_id, dataspace2_id, H5P_DEFAULT, H5P_DEFAULT);

            /* Write the attribute data. */
            status = H5Awrite(attribute_id, vls_type_c_id, variablesPtr_[data.first].data());

            /* Close the attribute. */
            status = H5Aclose(attribute_id);
            status = H5Sclose(dataspace2_id);

            /* End access to the dataset and release resources used by it. */
            status = H5Dclose(dataset_id);

            /* Terminate access to the data space. */ 
            status = H5Sclose(dataspace_id);
        }

        /* Close the file. */
        status = H5Fclose(file_id);
    }

    void fill()
    {
        ++nEvts_;
        size_t maxSize = 0;
        for(const auto& dataset : variables_)
        {
            auto& ptrPair = pointers_[dataset.first];
            auto& varVec = variables_[dataset.first];
            auto& dataVec = data_[dataset.first];
            int nCand = 1;
            //get ncand
            for(const auto& pp : ptrPair)
            {
                //Look for the first vector
                if(pp.first)
                {
                    nCand = (*static_cast<const std::vector<double> * const * const>(pp.second))->size();
                    break;
                }
            }
            for(int i = 0; i < nCand; ++i)
            {
                int index = 0;
                for(const auto& pp : ptrPair)
                {
                    //Check if this is a vector or a pointer 
                    if(pp.first) dataVec.push_back(nCand?((**static_cast<const std::vector<double> * const * const>(pp.second))[i]):0.0);
                    else         dataVec.push_back(*static_cast<const double * const>(pp.second));
                    index++;
                }
            }
            maxSize = std::max(maxSize, dataVec.size()/varVec.size());
        }

        if(maxSize >= nEvtsPerFile_)
        {
            //if we reached the max event per file lets write the file
            saveHDF5File();

            //reset the data structure 
            nEvts_ = 0;
            for(auto& data : data_) data.second.clear();
        }
    }

    ~HDF5Writer()
    {
        //Write the last events no matter what we have                                                                                                                                                                        
        saveHDF5File();
    }
    
};

int main(int argc, char* argv[])
{
    using namespace std;

    int opt;
    int option_index = 0;
    static struct option long_options[] = {
        {"fakerate",         no_argument, 0, 'f'},
	{"condor",           no_argument, 0, 'c'},
        {"dataSets",   required_argument, 0, 'D'},
        {"numFiles",   required_argument, 0, 'N'},
        {"startFile",  required_argument, 0, 'M'},
        {"numEvts",    required_argument, 0, 'E'},
        {"ratio"  ,    required_argument, 0, 'R'},
    };
    bool forFakeRate = false;
    bool runOnCondor = false;
    string dataSets = "", /*sampleloc = AnaSamples::fileDir,*/ outFile = "trainingTuple", sampleRatios = "1:1";
    int nFiles = -1, startFile = 0, nEvts = -1, printInterval = 1000;

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
        sprintf(thistFile, "trainingTuple_%d", startFile);
        outFile = thistFile;
        //sampleloc = "condor";
    }

    AnaSamples::SampleSet        ss("sampleSets.cfg", runOnCondor);
    AnaSamples::SampleCollection sc("sampleCollections.cfg", ss);

    map<string, vector<AnaSamples::FileSummary>> fileMap;

    //Select approperiate datasets here
    if(dataSets.compare("TEST") == 0)
    {
        std::cout<<"What are you doing???"<<std::endl;
        return 0;
        //fileMap["DYJetsToLL"]  = {ss["DYJetsToLL_HT_600toInf"]};
        //fileMap["ZJetsToNuNu"] = {ss["ZJetsToNuNu_HT_2500toInf"]};
        //fileMap["DYJetsToLL_HT_600toInf"] = {ss["DYJetsToLL_HT_600toInf"]};
        //fileMap["ZJetsToNuNu_HT_2500toInf"] = {ss["ZJetsToNuNu_HT_2500toInf"]};
        //fileMap["TTbarDiLep"] = {ss["TTbarDiLep"]};
        //fileMap["TTbarNoHad"] = {ss["TTbarDiLep"]};
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

    const std::map<std::string, std::vector<std::string>> variables =
    {
        {"EventShapeVar", {"EvtNum_double",
                           "sampleWgt",
                           "Weight",
                           "fwm2_top6", 
                           "fwm3_top6", 
                           "fwm4_top6", 
                           "fwm5_top6", 
                           "fwm6_top6", 
                           "fwm7_top6", 
                           "fwm8_top6", 
                           "fwm9_top6", 
                           "fwm10_top6", 
                           "jmt_ev0_top6", 
                           "jmt_ev1_top6", 
                           "jmt_ev2_top6",
                           "NGoodJets_double"} }
    };

    //parse sample splitting and set up minituples
    vector<pair<std::unique_ptr<HDF5Writer>, int>> mtmVec;
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
        mtmVec.emplace_back(std::unique_ptr<HDF5Writer>(new HDF5Writer(variables, 500000, ofname)), splitNum);
    }

    for(auto& fileVec : fileMap)
    {
        for(auto& file : fileVec.second)
        {
            int startCount = 0, fileCount = 0, NEvtsTotal = 0;

            std::cout << fileVec.first << std::endl;
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
                    //PrepVariables prepVars(variables);
                    //tr.registerFunction(prepVars);
                    std::string runtype = (file.tag.find("Data") != std::string::npos) ? "Data" : "MC";
                    tr.registerDerivedVar<std::string>("runtype",runtype);
                    tr.registerDerivedVar<std::string>("filetag",file.tag);
                    tr.registerDerivedVar<double>("etaCut",2.4);
                    tr.registerDerivedVar<bool>("blind",true);                        
                    
                    Muon muon;
                    Electron electron;
                    MakeMVAVariables makeMVAVariables(false);
                    Jet jet;
                    BJet bjet;
                    CommonVariables commonVariables;
                    Baseline baseline;
                    tr.registerFunction( std::move(muon) );
                    tr.registerFunction( std::move(electron) );
                    tr.registerFunction( std::move(makeMVAVariables) );
                    tr.registerFunction( std::move(jet) );
                    tr.registerFunction( std::move(bjet) );
                    tr.registerFunction( std::move(commonVariables) );
                    tr.registerFunction( std::move(baseline) );

                    int splitCounter = 0, mtmIndex = 0;
                    bool branchesInitialized = false;

                    while(tr.getNextEvent())
                    {
                        //If nEvts is set, stop after so many events
                        if(nEvts > 0 && NEvtsTotal > nEvts) break;
                        if(tr.getEvtNum() % printInterval == 0) std::cout << "Event #: " << tr.getEvtNum() << std::endl;

                        //Get sample lumi weight and correct for the actual number of events 
                        //This needs to happen before we ad sampleWgt to the mini tuple variables to save
                        double weight = file.getWeight();
                        bool isData = false; bool isSignal = false;
                        if(file.tag.find("Data") != std::string::npos)
                        {
                            isData = true;
                        }
                        else 
                        {
                            isSignal = true;
                        }
                        tr.registerDerivedVar("sampleWgt", weight);
                        tr.registerDerivedVar("isData", isData);
                        tr.registerDerivedVar("isSignal", isSignal);
                        tr.registerDerivedVar("EvtNum_double", static_cast<double>(tr.getVar<int>("EvtNum"))); 
                        tr.registerDerivedVar("NGoodJets_double", static_cast<double>(tr.getVar<int>("NGoodJets"))); 
                        
                        //Things to run only on first event
                        if(!branchesInitialized)
                        {
                            try
                            {
                                //Initialize the mini tuple branches, needs to be done after first call of tr.getNextEvent()
                                for(auto& mtm : mtmVec)
                                {
                                    mtm.first->initBranches(tr);
                                }
                                branchesInitialized = true;
                                std::cout<<"I might not work"<<std::endl;
                            }
                            catch(const SATException& e)
                            {
                                //do nothing here - this is sort of hacky
                                std::cout<<"I did not work"<<std::endl;
                                continue;
                            }
                        }

                        //Get cut variable 
                        //const bool& passMVABaseline = tr.getVar<bool>("passMVABaseline");
			//const bool& passValidationBaseline = tr.getVar<bool>("passValidationBaseline");
                        const auto& passBaseline0l_Good = tr.getVar<bool>("passBaseline0l_Good");
                        const auto& passBaseline1l_Good = tr.getVar<bool>("passBaseline1l_Good");
                        const auto& Mbl = tr.getVar<double>("Mbl");
                        
			//fill mini tuple
			bool passbaseline = passBaseline1l_Good && Mbl>30 && Mbl<180;
			if(passbaseline)
                        {
                            //std::cout<<"Got one"<<std::endl;
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

                    f->Close();
                }
                catch(const SATException& e)
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
