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
    HDF5Writer(const std::map<std::string, std::vector<std::string>>& variables, int eventsPerFile, std::string ofname) : variables_(variables), nEvtsPerFile_(eventsPerFile), nEvts_(0), nFile_(0), ofname_(ofname)
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
                    //std::cout << type << std::endl;
                    //if(type.find("*") != std::string::npos)
                    //{
                    //    throw "MiniTupleMaker::initBranches(...): Vectors of pointers are not allowed in MiniTuples!!!";
                    //}
                    //else
                    {
                        if(type.find("double") != std::string::npos)
                        {
                            ptrPair.push_back(std::make_pair(true, tr.getVecPtr(var)));
                        }
                        else
                        {
                        throw "HDF5Writer::initBranches(...): Variable type unknown!!! var: " + var + ", type: " + type;           
                        }
                    }
                }
                else
                {
                    if(type.find("double") != std::string::npos)
                    {
                        ptrPair.push_back(std::make_pair(false, tr.getPtr(var)));
                    }
                    else
                    {
                        throw "HDF5Writer::initBranches(...): Variable type unknown!!! var: " + var + ", type: " + type;
                    }
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
        for(const auto& dataset : variables_)
        {
            auto& ptrPair = pointers_[dataset.first];
            auto& varVec = variables_[dataset.first];
            auto& dataVec = data_[dataset.first];
            int nCand = 0;
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
                for(const auto& pp : ptrPair)
                {
                    //Check if this is a vector or a pointer 
                    if(pp.first) dataVec.push_back(nCand?((**static_cast<const std::vector<double> * const * const>(pp.second))[i]):0.0);
                    else         dataVec.push_back(*static_cast<const double * const>(pp.second));
                }
            }
        }

        if(nEvts_ >= nEvtsPerFile_)
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
    int eventNum_;

    void prepVariables(NTupleReader& tr)
    {
        const std::vector<TLorentzVector>& jetsLVec  = tr.getVec<TLorentzVector>("jetsLVec");
        const std::vector<double>& recoJetsBtag      = tr.getVec<double>("recoJetsBtag_0");
        const std::vector<double>& qgLikelihood      = tr.getVec<double>("qgLikelihood");

        const std::vector<int>& qgMult  = tr.getVec<int>("qgMult");
        const std::vector<double>& qgPtD   = tr.getVec<double>("qgPtD");
        const std::vector<double>& qgAxis1 = tr.getVec<double>("qgAxis1");
        const std::vector<double>& qgAxis2 = tr.getVec<double>("qgAxis2");

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

        auto convertToDoubleandRegister = [](NTupleReader& tr, std::string name)
        {
            const std::vector<int>& intVec = tr.getVec<int>(name);
            std::vector<double>* doubleVec = new std::vector<double>(intVec.begin(), intVec.end());
            tr.registerDerivedVec(name+"ConvertedToDouble", doubleVec);
            return doubleVec;
        };

        myConstAK4Inputs.addQGLVectors(qgMult, qgPtD, qgAxis1, qgAxis2);
        myConstAK4Inputs.addSupplamentalVector("recoJetsFlavor"                      , tr.getVec<double>("recoJetsFlavor"));
        myConstAK4Inputs.addSupplamentalVector("recoJetsJecScaleRawToFull"           , tr.getVec<double>("recoJetsJecScaleRawToFull"));
        myConstAK4Inputs.addSupplamentalVector("recoJetschargedHadronEnergyFraction" , tr.getVec<double>("recoJetschargedHadronEnergyFraction"));
        myConstAK4Inputs.addSupplamentalVector("recoJetschargedEmEnergyFraction"     , tr.getVec<double>("recoJetschargedEmEnergyFraction"));
        myConstAK4Inputs.addSupplamentalVector("recoJetsneutralEmEnergyFraction"     , tr.getVec<double>("recoJetsneutralEmEnergyFraction"));
        myConstAK4Inputs.addSupplamentalVector("recoJetsmuonEnergyFraction"          , tr.getVec<double>("recoJetsmuonEnergyFraction"));
        myConstAK4Inputs.addSupplamentalVector("recoJetsHFHadronEnergyFraction"      , tr.getVec<double>("recoJetsHFHadronEnergyFraction"));
        myConstAK4Inputs.addSupplamentalVector("recoJetsHFEMEnergyFraction"          , tr.getVec<double>("recoJetsHFEMEnergyFraction"));
        myConstAK4Inputs.addSupplamentalVector("recoJetsneutralEnergyFraction"       , tr.getVec<double>("recoJetsneutralEnergyFraction"));
        myConstAK4Inputs.addSupplamentalVector("PhotonEnergyFraction"                , tr.getVec<double>("PhotonEnergyFraction"));
        myConstAK4Inputs.addSupplamentalVector("ElectronEnergyFraction"              , tr.getVec<double>("ElectronEnergyFraction"));
        myConstAK4Inputs.addSupplamentalVector("ChargedHadronMultiplicity"           , tr.getVec<double>("ChargedHadronMultiplicity"));
        myConstAK4Inputs.addSupplamentalVector("NeutralHadronMultiplicity"           , tr.getVec<double>("NeutralHadronMultiplicity"));
        myConstAK4Inputs.addSupplamentalVector("PhotonMultiplicity"                  , tr.getVec<double>("PhotonMultiplicity"));
        myConstAK4Inputs.addSupplamentalVector("ElectronMultiplicity"                , tr.getVec<double>("ElectronMultiplicity"));
        myConstAK4Inputs.addSupplamentalVector("MuonMultiplicity"                    , tr.getVec<double>("MuonMultiplicity"));
        myConstAK4Inputs.addSupplamentalVector("DeepCSVb"                            , tr.getVec<double>("DeepCSVb"));
        myConstAK4Inputs.addSupplamentalVector("DeepCSVc"                            , tr.getVec<double>("DeepCSVc"));
        myConstAK4Inputs.addSupplamentalVector("DeepCSVl"                            , tr.getVec<double>("DeepCSVl"));
        myConstAK4Inputs.addSupplamentalVector("DeepCSVbb"                           , tr.getVec<double>("DeepCSVbb"));
        myConstAK4Inputs.addSupplamentalVector("DeepCSVcc"                           , tr.getVec<double>("DeepCSVcc"));
        myConstAK4Inputs.addSupplamentalVector("CvsL"                                , tr.getVec<double>("CvsL"));
        myConstAK4Inputs.addSupplamentalVector("CvsB"                                , tr.getVec<double>("CvsB"));
        myConstAK4Inputs.addSupplamentalVector("CombinedSvtx"                        , tr.getVec<double>("CombinedSvtx"));
        myConstAK4Inputs.addSupplamentalVector("Svtx"                                , tr.getVec<double>("Svtx"));
        myConstAK4Inputs.addSupplamentalVector("SoftM"                               , tr.getVec<double>("SoftM"));
        myConstAK4Inputs.addSupplamentalVector("SoftE"                               , tr.getVec<double>("SoftE"));
        myConstAK4Inputs.addSupplamentalVector("JetProba"                            , tr.getVec<double>("JetProba_0"));
        myConstAK4Inputs.addSupplamentalVector("JetBprob"                            , tr.getVec<double>("JetBprob"));
        myConstAK4Inputs.addSupplamentalVector("recoJetsCharge"                      , tr.getVec<double>("recoJetsCharge_0"));
        myConstAK4Inputs.addSupplamentalVector("CSVTrackJetPt"                       , tr.getVec<double>("CSVTrackJetPt"));
        myConstAK4Inputs.addSupplamentalVector("CSVVertexCategory"                   , tr.getVec<double>("CSVVertexCategory"));
        myConstAK4Inputs.addSupplamentalVector("CSVJetNSecondaryVertices"            , *convertToDoubleandRegister(tr, "CSVJetNSecondaryVertices"));
        myConstAK4Inputs.addSupplamentalVector("CSVTrackSumJetEtRatio"               , tr.getVec<double>("CSVTrackSumJetEtRatio"));
        myConstAK4Inputs.addSupplamentalVector("CSVTrackSumJetDeltaR"                , tr.getVec<double>("CSVTrackSumJetDeltaR"));
        myConstAK4Inputs.addSupplamentalVector("CSVTrackSip2dValAboveCharm"          , tr.getVec<double>("CSVTrackSip2dValAboveCharm"));
        myConstAK4Inputs.addSupplamentalVector("CSVTrackSip2dSigAboveCharm"          , tr.getVec<double>("CSVTrackSip2dSigAboveCharm"));
        myConstAK4Inputs.addSupplamentalVector("CSVTrackSip3dValAboveCharm"          , tr.getVec<double>("CSVTrackSip3dValAboveCharm"));
        myConstAK4Inputs.addSupplamentalVector("CSVTrackSip3dSigAboveCharm"          , tr.getVec<double>("CSVTrackSip3dSigAboveCharm"));
        myConstAK4Inputs.addSupplamentalVector("CSVVertexMass"                       , tr.getVec<double>("CSVVertexMass"));
        myConstAK4Inputs.addSupplamentalVector("CSVVertexNTracks"                    , *convertToDoubleandRegister(tr, "CSVVertexNTracks"));
        myConstAK4Inputs.addSupplamentalVector("CSVVertexEnergyRatio"                , tr.getVec<double>("CSVVertexEnergyRatio"));
        myConstAK4Inputs.addSupplamentalVector("CSVVertexJetDeltaR"                  , tr.getVec<double>("CSVVertexJetDeltaR"));
        myConstAK4Inputs.addSupplamentalVector("CSVFlightDistance2dVal"              , tr.getVec<double>("CSVFlightDistance2dVal"));
        myConstAK4Inputs.addSupplamentalVector("CSVFlightDistance2dSig"              , tr.getVec<double>("CSVFlightDistance2dSig"));
        myConstAK4Inputs.addSupplamentalVector("CSVFlightDistance3dVal"              , tr.getVec<double>("CSVFlightDistance3dVal"));
        myConstAK4Inputs.addSupplamentalVector("CSVFlightDistance3dSig"              , tr.getVec<double>("CSVFlightDistance3dSig"));
        myConstAK4Inputs.addSupplamentalVector("CTagVertexCategory"                  , tr.getVec<double>("CTagVertexCategory"));
        myConstAK4Inputs.addSupplamentalVector("CTagJetNSecondaryVertices"           , *convertToDoubleandRegister(tr, "CTagJetNSecondaryVertices"));
        myConstAK4Inputs.addSupplamentalVector("CTagTrackSumJetEtRatio"              , tr.getVec<double>("CTagTrackSumJetEtRatio"));
        myConstAK4Inputs.addSupplamentalVector("CTagTrackSumJetDeltaR"               , tr.getVec<double>("CTagTrackSumJetDeltaR"));
        myConstAK4Inputs.addSupplamentalVector("CTagTrackSip2dSigAboveCharm"         , tr.getVec<double>("CTagTrackSip2dSigAboveCharm"));
        myConstAK4Inputs.addSupplamentalVector("CTagTrackSip3dSigAboveCharm"         , tr.getVec<double>("CTagTrackSip3dSigAboveCharm"));
        myConstAK4Inputs.addSupplamentalVector("CTagVertexMass"                      , tr.getVec<double>("CTagVertexMass"));
        myConstAK4Inputs.addSupplamentalVector("CTagVertexNTracks"                   , *convertToDoubleandRegister(tr, "CTagVertexNTracks"));
        myConstAK4Inputs.addSupplamentalVector("CTagVertexEnergyRatio"               , tr.getVec<double>("CTagVertexEnergyRatio"));
        myConstAK4Inputs.addSupplamentalVector("CTagVertexJetDeltaR"                 , tr.getVec<double>("CTagVertexJetDeltaR"));
        myConstAK4Inputs.addSupplamentalVector("CTagFlightDistance2dSig"             , tr.getVec<double>("CTagFlightDistance2dSig"));
        myConstAK4Inputs.addSupplamentalVector("CTagFlightDistance3dSig"             , tr.getVec<double>("CTagFlightDistance3dSig"));
        myConstAK4Inputs.addSupplamentalVector("CTagMassVertexEnergyFraction"        , tr.getVec<double>("CTagMassVertexEnergyFraction"));
        myConstAK4Inputs.addSupplamentalVector("CTagVertexBoostOverSqrtJetPt"        , tr.getVec<double>("CTagVertexBoostOverSqrtJetPt"));
        myConstAK4Inputs.addSupplamentalVector("CTagVertexLeptonCategory"            , tr.getVec<double>("CTagVertexLeptonCategory"));

        std::vector<Constituent> constituents = ttUtility::packageConstituents(myConstAK4Inputs);

        //run tagger
        topTagger_->runTagger(constituents);

        //retrieve results
        const TopTaggerResults& ttr = topTagger_->getResults();
        const std::vector<TopObject>& topCands = ttr.getTopCandidates();

        //Get gen matching results

        std::vector<TLorentzVector> genTops = genUtility::GetHadTopLVec(genDecayLVec, genDecayPdgIdVec, genDecayIdxVec, genDecayMomIdxVec);

        std::pair<std::vector<int>, std::pair<std::vector<int>, std::vector<TLorentzVector>>> genMatches = topMatcher_.TopConst(topCands, genDecayLVec, genDecayPdgIdVec, genDecayIdxVec, genDecayMomIdxVec);

        std::vector<double> *genMatchdR = new std::vector<double>();//genMatches.first);
        std::vector<double> *genMatchConst = new std::vector<double>();//genMatches.second.first);
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

        std::vector<double>* candNum = new std::vector<double>();
        int iTop = 0;
        //prepare reco top quantities
        for(const TopObject& topCand : topCands)
        {
            candNum->push_back(static_cast<double>(iTop++));
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

        tr.registerDerivedVar("eventNum", static_cast<double>(eventNum_++));
        tr.registerDerivedVec("candNum", candNum);
        tr.registerDerivedVar("ncand", static_cast<double>(candNum->size()));

        //Generate basic MVA selection 
        bool passMVABaseline = true;//met > 100 && cntNJetsPt30 >= 5 && cntCSVS >= 1 && cntCSVL >= 2;//true;//(topCands.size() >= 1) || genMatches.second.second->size() >= 1;
        tr.registerDerivedVar("passMVABaseline", passMVABaseline);
	const bool passValidationBaseline = cntNJetsPt30>=AnaConsts::nJetsSelPt30Eta24 && met>=AnaConsts::defaultMETcut && cntCSVS>=AnaConsts::low_nJetsSelBtagged;
	tr.registerDerivedVar("passValidationBaseline",passValidationBaseline);
	tr.registerDerivedVar("MET", met);
	tr.registerDerivedVar("Njet",      static_cast<double>(cntNJetsPt30));
        tr.registerDerivedVar("Bjet",      static_cast<double>(cntCSVS));
        tr.registerDerivedVar("passnJets", static_cast<double>(passnJets));
        tr.registerDerivedVar("passMET",   static_cast<double>(passMET));
        tr.registerDerivedVar("passdPhis", static_cast<double>(passdPhis));
        tr.registerDerivedVar("passBJets", static_cast<double>(passBJets));
	
    }

public:
    PrepVariables()
    {
        eventNum_ = 0;

        topTagger_ = new TopTagger();
        topTagger_->setCfgFile("TopTaggerClusterOnly.cfg");

        //double variables list here
        //allowedVarsD_ = {"cand_pt", "cand_eta", "cand_phi", "cand_m", "cand_dRMax", "j1_pt", "j1_eta", "j1_phi", "j1_m", "j1_CSV", "j2_pt", "j2_eta", "j2_phi", "j2_m", "j2_CSV", "j3_pt", "j3_eta", "j3_phi", "j3_m",  "j3_CSV", "dR12", "dEta12", "dPhi12", "dR13", "dEta13", "dPhi13", "dR23", "dEta23", "dPhi23", "j12_m", "j13_m", "j23_m", "j12_pt", "j13_pt", "j23_pt", "j12j3_dR", "j13j2_dR", "j23j1_dR", "genTopPt", "j1_QGL", "j2_QGL", "j3_QGL" , "MET"};
        allowedVarsD_ = {"genTopPt", 
                         "cand_dRMax",
                         "cand_eta",
                         "cand_m",
                         "cand_phi",
                         "cand_pt",
                         "dR12_lab",
                         "dR13_lab",
                         "dR1_23_lab",
                         "dR23_lab",
                         "dR2_13_lab",
                         "dR3_12_lab",
                         "dRPtTop",
                         "dRPtW",
                         "dTheta12",
                         "dTheta13",
                         "dTheta23",
                         "j12_m",
                         "j12_m_lab",
                         "j13_m",
                         "j13_m_lab",
                         "j1_CSV",
                         "j1_CSVFlightDistance2dSig",
                         "j1_CSVFlightDistance2dVal",
                         "j1_CSVFlightDistance3dSig",
                         "j1_CSVFlightDistance3dVal",
                         "j1_CSVJetNSecondaryVertices",
                         "j1_CSVTrackJetPt",
                         "j1_CSVTrackSip2dSigAboveCharm",
                         "j1_CSVTrackSip2dValAboveCharm",
                         "j1_CSVTrackSip3dSigAboveCharm",
                         "j1_CSVTrackSip3dValAboveCharm",
                         "j1_CSVTrackSumJetDeltaR",
                         "j1_CSVTrackSumJetEtRatio",
                         "j1_CSVVertexCategory",
                         "j1_CSVVertexEnergyRatio",
                         "j1_CSVVertexJetDeltaR",
                         "j1_CSVVertexMass",
                         "j1_CSVVertexNTracks",
                         "j1_CSV_lab",
                         "j1_CTagFlightDistance2dSig",
                         "j1_CTagFlightDistance3dSig",
                         "j1_CTagJetNSecondaryVertices",
                         "j1_CTagMassVertexEnergyFraction",
                         "j1_CTagTrackSip2dSigAboveCharm",
                         "j1_CTagTrackSip3dSigAboveCharm",
                         "j1_CTagTrackSumJetDeltaR",
                         "j1_CTagTrackSumJetEtRatio",
                         "j1_CTagVertexBoostOverSqrtJetPt",
                         "j1_CTagVertexCategory",
                         "j1_CTagVertexEnergyRatio",
                         "j1_CTagVertexJetDeltaR",
                         "j1_CTagVertexLeptonCategory",
                         "j1_CTagVertexMass",
                         "j1_CTagVertexNTracks",
                         "j1_ChargedHadronMultiplicity",
                         "j1_CombinedSvtx",
                         "j1_CvsB",
                         "j1_CvsL",
                         "j1_DeepCSVb",
                         "j1_DeepCSVbb",
                         "j1_DeepCSVc",
                         "j1_DeepCSVcc",
                         "j1_DeepCSVl",
                         "j1_ElectronEnergyFraction",
                         "j1_ElectronMultiplicity",
                         "j1_JetBprob",
                         "j1_JetProba",
                         "j1_MuonMultiplicity",
                         "j1_NeutralHadronMultiplicity",
                         "j1_PhotonEnergyFraction",
                         "j1_PhotonMultiplicity",
                         "j1_QGL",
                         "j1_QGL_lab",
                         "j1_SoftE",
                         "j1_SoftM",
                         "j1_Svtx",
                         "j1_eta_lab",
                         "j1_m",
                         "j1_m_lab",
                         "j1_p",
                         "j1_phi_lab",
                         "j1_pt_lab",
                         "j1_qgAxis1",
                         "j1_qgAxis1_lab",
                         "j1_qgAxis2",
                         "j1_qgAxis2_lab",
                         "j1_qgMult",
                         "j1_qgMult_lab",
                         "j1_qgPtD",
                         "j1_qgPtD_lab",
                         "j1_recoJetsCharge",
                         "j1_recoJetsFlavor",
                         "j1_recoJetsHFEMEnergyFraction",
                         "j1_recoJetsHFHadronEnergyFraction",
                         "j1_recoJetsJecScaleRawToFull",
                         "j1_recoJetschargedEmEnergyFraction",
                         "j1_recoJetschargedHadronEnergyFraction",
                         "j1_recoJetsmuonEnergyFraction",
                         "j1_recoJetsneutralEmEnergyFraction",
                         "j1_recoJetsneutralEnergyFraction",
                         "j23_m",
                         "j23_m_lab",
                         "j2_CSV",
                         "j2_CSVFlightDistance2dSig",
                         "j2_CSVFlightDistance2dVal",
                         "j2_CSVFlightDistance3dSig",
                         "j2_CSVFlightDistance3dVal",
                         "j2_CSVJetNSecondaryVertices",
                         "j2_CSVTrackJetPt",
                         "j2_CSVTrackSip2dSigAboveCharm",
                         "j2_CSVTrackSip2dValAboveCharm",
                         "j2_CSVTrackSip3dSigAboveCharm",
                         "j2_CSVTrackSip3dValAboveCharm",
                         "j2_CSVTrackSumJetDeltaR",
                         "j2_CSVTrackSumJetEtRatio",
                         "j2_CSVVertexCategory",
                         "j2_CSVVertexEnergyRatio",
                         "j2_CSVVertexJetDeltaR",
                         "j2_CSVVertexMass",
                         "j2_CSVVertexNTracks",
                         "j2_CSV_lab",
                         "j2_CTagFlightDistance2dSig",
                         "j2_CTagFlightDistance3dSig",
                         "j2_CTagJetNSecondaryVertices",
                         "j2_CTagMassVertexEnergyFraction",
                         "j2_CTagTrackSip2dSigAboveCharm",
                         "j2_CTagTrackSip3dSigAboveCharm",
                         "j2_CTagTrackSumJetDeltaR",
                         "j2_CTagTrackSumJetEtRatio",
                         "j2_CTagVertexBoostOverSqrtJetPt",
                         "j2_CTagVertexCategory",
                         "j2_CTagVertexEnergyRatio",
                         "j2_CTagVertexJetDeltaR",
                         "j2_CTagVertexLeptonCategory",
                         "j2_CTagVertexMass",
                         "j2_CTagVertexNTracks",
                         "j2_ChargedHadronMultiplicity",
                         "j2_CombinedSvtx",
                         "j2_CvsB",
                         "j2_CvsL",
                         "j2_DeepCSVb",
                         "j2_DeepCSVbb",
                         "j2_DeepCSVc",
                         "j2_DeepCSVcc",
                         "j2_DeepCSVl",
                         "j2_ElectronEnergyFraction",
                         "j2_ElectronMultiplicity",
                         "j2_JetBprob",
                         "j2_JetProba",
                         "j2_MuonMultiplicity",
                         "j2_NeutralHadronMultiplicity",
                         "j2_PhotonEnergyFraction",
                         "j2_PhotonMultiplicity",
                         "j2_QGL",
                         "j2_QGL_lab",
                         "j2_SoftE",
                         "j2_SoftM",
                         "j2_Svtx",
                         "j2_eta_lab",
                         "j2_m",
                         "j2_m_lab",
                         "j2_p",
                         "j2_phi_lab",
                         "j2_pt_lab",
                         "j2_qgAxis1",
                         "j2_qgAxis1_lab",
                         "j2_qgAxis2",
                         "j2_qgAxis2_lab",
                         "j2_qgMult",
                         "j2_qgMult_lab",
                         "j2_qgPtD",
                         "j2_qgPtD_lab",
                         "j2_recoJetsCharge",
                         "j2_recoJetsFlavor",
                         "j2_recoJetsHFEMEnergyFraction",
                         "j2_recoJetsHFHadronEnergyFraction",
                         "j2_recoJetsJecScaleRawToFull",
                         "j2_recoJetschargedEmEnergyFraction",
                         "j2_recoJetschargedHadronEnergyFraction",
                         "j2_recoJetsmuonEnergyFraction",
                         "j2_recoJetsneutralEmEnergyFraction",
                         "j2_recoJetsneutralEnergyFraction",
                         "j3_CSV",
                         "j3_CSVFlightDistance2dSig",
                         "j3_CSVFlightDistance2dVal",
                         "j3_CSVFlightDistance3dSig",
                         "j3_CSVFlightDistance3dVal",
                         "j3_CSVJetNSecondaryVertices",
                         "j3_CSVTrackJetPt",
                         "j3_CSVTrackSip2dSigAboveCharm",
                         "j3_CSVTrackSip2dValAboveCharm",
                         "j3_CSVTrackSip3dSigAboveCharm",
                         "j3_CSVTrackSip3dValAboveCharm",
                         "j3_CSVTrackSumJetDeltaR",
                         "j3_CSVTrackSumJetEtRatio",
                         "j3_CSVVertexCategory",
                         "j3_CSVVertexEnergyRatio",
                         "j3_CSVVertexJetDeltaR",
                         "j3_CSVVertexMass",
                         "j3_CSVVertexNTracks",
                         "j3_CSV_lab",
                         "j3_CTagFlightDistance2dSig",
                         "j3_CTagFlightDistance3dSig",
                         "j3_CTagJetNSecondaryVertices",
                         "j3_CTagMassVertexEnergyFraction",
                         "j3_CTagTrackSip2dSigAboveCharm",
                         "j3_CTagTrackSip3dSigAboveCharm",
                         "j3_CTagTrackSumJetDeltaR",
                         "j3_CTagTrackSumJetEtRatio",
                         "j3_CTagVertexBoostOverSqrtJetPt",
                         "j3_CTagVertexCategory",
                         "j3_CTagVertexEnergyRatio",
                         "j3_CTagVertexJetDeltaR",
                         "j3_CTagVertexLeptonCategory",
                         "j3_CTagVertexMass",
                         "j3_CTagVertexNTracks",
                         "j3_ChargedHadronMultiplicity",
                         "j3_CombinedSvtx",
                         "j3_CvsB",
                         "j3_CvsL",
                         "j3_DeepCSVb",
                         "j3_DeepCSVbb",
                         "j3_DeepCSVc",
                         "j3_DeepCSVcc",
                         "j3_DeepCSVl",
                         "j3_ElectronEnergyFraction",
                         "j3_ElectronMultiplicity",
                         "j3_JetBprob",
                         "j3_JetProba",
                         "j3_MuonMultiplicity",
                         "j3_NeutralHadronMultiplicity",
                         "j3_PhotonEnergyFraction",
                         "j3_PhotonMultiplicity",
                         "j3_QGL",
                         "j3_QGL_lab",
                         "j3_SoftE",
                         "j3_SoftM",
                         "j3_Svtx",
                         "j3_eta_lab",
                         "j3_m",
                         "j3_m_lab",
                         "j3_p",
                         "j3_phi_lab",
                         "j3_pt_lab",
                         "j3_qgAxis1",
                         "j3_qgAxis1_lab",
                         "j3_qgAxis2",
                         "j3_qgAxis2_lab",
                         "j3_qgMult",
                         "j3_qgMult_lab",
                         "j3_qgPtD",
                         "j3_qgPtD_lab",
                         "j3_recoJetsCharge",
                         "j3_recoJetsFlavor",
                         "j3_recoJetsHFEMEnergyFraction",
                         "j3_recoJetsHFHadronEnergyFraction",
                         "j3_recoJetsJecScaleRawToFull",
                         "j3_recoJetschargedEmEnergyFraction",
                         "j3_recoJetschargedHadronEnergyFraction",
                         "j3_recoJetsmuonEnergyFraction",
                         "j3_recoJetsneutralEmEnergyFraction",
                         "j3_recoJetsneutralEnergyFraction",
                         "sd_n2",};

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

    const std::map<std::string, std::vector<std::string>> variables =
    {
        {"gen_tops", {"eventNum", "candNum", "genTopPt", "sampleWgt","Njet"} },
        {"reco_candidates", {"eventNum", "candNum", "ncand", "cand_dRMax", "cand_eta", "cand_m", "cand_phi", "cand_pt", "dR12_lab", "dR13_lab", "dR1_23_lab", "dR23_lab", "dR2_13_lab", "dR3_12_lab", "dRPtTop", "dRPtW", "dTheta12", "dTheta13", "dTheta23", "j12_m", "j12_m_lab", "j13_m", "j13_m_lab", "j1_CSV", "j1_CSVFlightDistance2dSig", "j1_CSVFlightDistance2dVal", "j1_CSVFlightDistance3dSig", "j1_CSVFlightDistance3dVal", "j1_CSVJetNSecondaryVertices", "j1_CSVTrackJetPt", "j1_CSVTrackSip2dSigAboveCharm", "j1_CSVTrackSip2dValAboveCharm", "j1_CSVTrackSip3dSigAboveCharm", "j1_CSVTrackSip3dValAboveCharm", "j1_CSVTrackSumJetDeltaR", "j1_CSVTrackSumJetEtRatio", "j1_CSVVertexCategory", "j1_CSVVertexEnergyRatio", "j1_CSVVertexJetDeltaR", "j1_CSVVertexMass", "j1_CSVVertexNTracks", "j1_CSV_lab", "j1_CTagFlightDistance2dSig", "j1_CTagFlightDistance3dSig", "j1_CTagJetNSecondaryVertices", "j1_CTagMassVertexEnergyFraction", "j1_CTagTrackSip2dSigAboveCharm", "j1_CTagTrackSip3dSigAboveCharm", "j1_CTagTrackSumJetDeltaR", "j1_CTagTrackSumJetEtRatio", "j1_CTagVertexBoostOverSqrtJetPt", "j1_CTagVertexCategory", "j1_CTagVertexEnergyRatio", "j1_CTagVertexJetDeltaR", "j1_CTagVertexLeptonCategory", "j1_CTagVertexMass", "j1_CTagVertexNTracks", "j1_ChargedHadronMultiplicity", "j1_CombinedSvtx", "j1_CvsB", "j1_CvsL", "j1_DeepCSVb", "j1_DeepCSVbb", "j1_DeepCSVc", "j1_DeepCSVcc", "j1_DeepCSVl", "j1_ElectronEnergyFraction", "j1_ElectronMultiplicity", "j1_JetBprob", "j1_JetProba", "j1_MuonMultiplicity", "j1_NeutralHadronMultiplicity", "j1_PhotonEnergyFraction", "j1_PhotonMultiplicity", "j1_QGL", "j1_QGL_lab", "j1_SoftE", "j1_SoftM", "j1_Svtx", "j1_eta_lab", "j1_m", "j1_m_lab", "j1_p", "j1_phi_lab", "j1_pt_lab", "j1_qgAxis1", "j1_qgAxis1_lab", "j1_qgAxis2", "j1_qgAxis2_lab", "j1_qgMult", "j1_qgMult_lab", "j1_qgPtD", "j1_qgPtD_lab", "j1_recoJetsCharge", "j1_recoJetsFlavor", "j1_recoJetsHFEMEnergyFraction", "j1_recoJetsHFHadronEnergyFraction", "j1_recoJetsJecScaleRawToFull", "j1_recoJetschargedEmEnergyFraction", "j1_recoJetschargedHadronEnergyFraction", "j1_recoJetsmuonEnergyFraction", "j1_recoJetsneutralEmEnergyFraction", "j1_recoJetsneutralEnergyFraction", "j23_m", "j23_m_lab", "j2_CSV", "j2_CSVFlightDistance2dSig", "j2_CSVFlightDistance2dVal", "j2_CSVFlightDistance3dSig", "j2_CSVFlightDistance3dVal", "j2_CSVJetNSecondaryVertices", "j2_CSVTrackJetPt", "j2_CSVTrackSip2dSigAboveCharm", "j2_CSVTrackSip2dValAboveCharm", "j2_CSVTrackSip3dSigAboveCharm", "j2_CSVTrackSip3dValAboveCharm", "j2_CSVTrackSumJetDeltaR", "j2_CSVTrackSumJetEtRatio", "j2_CSVVertexCategory", "j2_CSVVertexEnergyRatio", "j2_CSVVertexJetDeltaR", "j2_CSVVertexMass", "j2_CSVVertexNTracks", "j2_CSV_lab", "j2_CTagFlightDistance2dSig", "j2_CTagFlightDistance3dSig", "j2_CTagJetNSecondaryVertices", "j2_CTagMassVertexEnergyFraction", "j2_CTagTrackSip2dSigAboveCharm", "j2_CTagTrackSip3dSigAboveCharm", "j2_CTagTrackSumJetDeltaR", "j2_CTagTrackSumJetEtRatio", "j2_CTagVertexBoostOverSqrtJetPt", "j2_CTagVertexCategory", "j2_CTagVertexEnergyRatio", "j2_CTagVertexJetDeltaR", "j2_CTagVertexLeptonCategory", "j2_CTagVertexMass", "j2_CTagVertexNTracks", "j2_ChargedHadronMultiplicity", "j2_CombinedSvtx", "j2_CvsB", "j2_CvsL", "j2_DeepCSVb", "j2_DeepCSVbb", "j2_DeepCSVc", "j2_DeepCSVcc", "j2_DeepCSVl", "j2_ElectronEnergyFraction", "j2_ElectronMultiplicity", "j2_JetBprob", "j2_JetProba", "j2_MuonMultiplicity", "j2_NeutralHadronMultiplicity", "j2_PhotonEnergyFraction", "j2_PhotonMultiplicity", "j2_QGL", "j2_QGL_lab", "j2_SoftE", "j2_SoftM", "j2_Svtx", "j2_eta_lab", "j2_m", "j2_m_lab", "j2_p", "j2_phi_lab", "j2_pt_lab", "j2_qgAxis1", "j2_qgAxis1_lab", "j2_qgAxis2", "j2_qgAxis2_lab", "j2_qgMult", "j2_qgMult_lab", "j2_qgPtD", "j2_qgPtD_lab", "j2_recoJetsCharge", "j2_recoJetsFlavor", "j2_recoJetsHFEMEnergyFraction", "j2_recoJetsHFHadronEnergyFraction", "j2_recoJetsJecScaleRawToFull", "j2_recoJetschargedEmEnergyFraction", "j2_recoJetschargedHadronEnergyFraction", "j2_recoJetsmuonEnergyFraction", "j2_recoJetsneutralEmEnergyFraction", "j2_recoJetsneutralEnergyFraction", "j3_CSV", "j3_CSVFlightDistance2dSig", "j3_CSVFlightDistance2dVal", "j3_CSVFlightDistance3dSig", "j3_CSVFlightDistance3dVal", "j3_CSVJetNSecondaryVertices", "j3_CSVTrackJetPt", "j3_CSVTrackSip2dSigAboveCharm", "j3_CSVTrackSip2dValAboveCharm", "j3_CSVTrackSip3dSigAboveCharm", "j3_CSVTrackSip3dValAboveCharm", "j3_CSVTrackSumJetDeltaR", "j3_CSVTrackSumJetEtRatio", "j3_CSVVertexCategory", "j3_CSVVertexEnergyRatio", "j3_CSVVertexJetDeltaR", "j3_CSVVertexMass", "j3_CSVVertexNTracks", "j3_CSV_lab", "j3_CTagFlightDistance2dSig", "j3_CTagFlightDistance3dSig", "j3_CTagJetNSecondaryVertices", "j3_CTagMassVertexEnergyFraction", "j3_CTagTrackSip2dSigAboveCharm", "j3_CTagTrackSip3dSigAboveCharm", "j3_CTagTrackSumJetDeltaR", "j3_CTagTrackSumJetEtRatio", "j3_CTagVertexBoostOverSqrtJetPt", "j3_CTagVertexCategory", "j3_CTagVertexEnergyRatio", "j3_CTagVertexJetDeltaR", "j3_CTagVertexLeptonCategory", "j3_CTagVertexMass", "j3_CTagVertexNTracks", "j3_ChargedHadronMultiplicity", "j3_CombinedSvtx", "j3_CvsB", "j3_CvsL", "j3_DeepCSVb", "j3_DeepCSVbb", "j3_DeepCSVc", "j3_DeepCSVcc", "j3_DeepCSVl", "j3_ElectronEnergyFraction", "j3_ElectronMultiplicity", "j3_JetBprob", "j3_JetProba", "j3_MuonMultiplicity", "j3_NeutralHadronMultiplicity", "j3_PhotonEnergyFraction", "j3_PhotonMultiplicity", "j3_QGL", "j3_QGL_lab", "j3_SoftE", "j3_SoftM", "j3_Svtx", "j3_eta_lab", "j3_m", "j3_m_lab", "j3_p", "j3_phi_lab", "j3_pt_lab", "j3_qgAxis1", "j3_qgAxis1_lab", "j3_qgAxis2", "j3_qgAxis2_lab", "j3_qgMult", "j3_qgMult_lab", "j3_qgPtD", "j3_qgPtD_lab", "j3_recoJetsCharge", "j3_recoJetsFlavor", "j3_recoJetsHFEMEnergyFraction", "j3_recoJetsHFHadronEnergyFraction", "j3_recoJetsJecScaleRawToFull", "j3_recoJetschargedEmEnergyFraction", "j3_recoJetschargedHadronEnergyFraction", "j3_recoJetsmuonEnergyFraction", "j3_recoJetsneutralEmEnergyFraction", "j3_recoJetsneutralEnergyFraction", "sd_n2", "genTopMatchesVec", "genConstiuentMatchesVec", "genConstMatchGenPtVec", "Njet", "Bjet", "passnJets", "passMET", "passdPhis", "passBJets", "MET", "sampleWgt"} }
    };

    //parse sample splitting and set up minituples
    //vector<pair<std::unique_ptr<MiniTupleMaker>, int>> mtmVec;
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
        //mtmVec.emplace_back(std::unique_ptr<MiniTupleMaker>(new MiniTupleMaker(ofname, "slimmedTuple")), splitNum);
        mtmVec.emplace_back(std::unique_ptr<HDF5Writer>(new HDF5Writer(variables, 250000, ofname)), splitNum);
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
                    PrepVariables prepVars;
                    tr.registerFunction(prepVars);

                    int splitCounter = 0, mtmIndex = 0;

                    while(tr.getNextEvent())
                    {
                        //Get sample lumi weight and correct for the actual number of events 
                        //This needs to happen before we ad sampleWgt to the mini tuple variables to save
                        double weight = file.getWeight();
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
