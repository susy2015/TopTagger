#ifdef DOPYCAPIBIND
#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include "Python.h"
#include "numpy/arrayobject.h"

#include "TLorentzVector.h"
#include "Math/VectorUtil.h"

#include <vector>
#include <iostream>
#include <memory>

#include "TopTagger/TopTagger/interface/TopTagger.h"
#include "TopTagger/TopTagger/interface/TopTaggerResults.h"
#include "TopTagger/TopTagger/interface/TopTaggerPython.h"
#include "TopTagger/TopTagger/interface/TopTaggerUtilities.h"
#include "TopTagger/CfgParser/include/TTException.h"

/// Destructor function for TopTagger object cleanup 
static void TopTaggerInterface_cleanup(PyObject *ptt)
{
    //Get top tagger pointer from capsule 
    TopTagger* tt = (TopTagger*) PyCapsule_GetPointer(ptt, "TopTagger");
    
    if(tt) delete tt;
}

static int TopTaggerInterface_makeAK4Const(std::unique_ptr<ttUtility::ConstAK4Inputs<Float_t, ttPython::Py_buffer_wrapper<Float_t>>>& ak4ConstInputs, std::vector<ttPython::Py_buffer_wrapper<Float_t>>& tempFloatBuffers, std::vector<std::vector<Float_t>>& tempIntToFloatVectors, std::vector<TLorentzVector>& jetsLV, std::vector<unsigned char>& filterVec, PyObject* pArgTuple)
{
    //number of variables which are not stored in supplamental dictionaries
    const unsigned int NEXTRAVAR = 1;

    PyObject *pJetPt, *pJetEta, *pJetPhi, *pJetMass, *pJetBtag, *pFloatVarsDict, *pIntVarsDict, *pElectronIdx1 = nullptr, *pMuonIdx1 = nullptr, *pElectron_pt = nullptr, *pElectron_eta = nullptr, *pElectron_phi = nullptr, *pElectron_mass = nullptr, *pElectron_cutBasedBits = nullptr, *pElectron_miniPFRelIso = nullptr, *pMuon_pt = nullptr, *pMuon_eta = nullptr, *pMuon_phi = nullptr, *pMuon_mass = nullptr, *pMuon_id = nullptr, *pMuon_pfRelIso = nullptr;
    if (!PyArg_ParseTuple(pArgTuple, "OOOOOO!O!|OOOOOOOOOOOOOO", &pJetPt, &pJetEta, &pJetPhi, &pJetMass, &pJetBtag, &PyDict_Type, &pFloatVarsDict, &PyDict_Type, &pIntVarsDict, &pElectronIdx1, &pMuonIdx1, &pElectron_pt, &pElectron_eta, &pElectron_phi, &pElectron_mass, &pElectron_cutBasedBits, &pElectron_miniPFRelIso, &pMuon_pt, &pMuon_eta, &pMuon_phi, &pMuon_mass, &pMuon_id, &pMuon_pfRelIso))
    {
        return 1;
    }

    //reserve space for the vector to stop reallocations during emplacing, need space for float and int "vectors" stored here
    Py_ssize_t floatSize = PyDict_Size(pFloatVarsDict);
    Py_ssize_t intSize = PyDict_Size(pIntVarsDict);
    tempFloatBuffers.reserve(NEXTRAVAR + floatSize + intSize);

    //Prepare std::vector<TLorentzVector> for jets lorentz vectors
    ttPython::Py_buffer_wrapper<Float_t> jetPt(pJetPt);
    ttPython::Py_buffer_wrapper<Float_t> jetEta(pJetEta);
    ttPython::Py_buffer_wrapper<Float_t> jetPhi(pJetPhi);
    ttPython::Py_buffer_wrapper<Float_t> jetM(pJetMass);

    //lepton matching variables
    ttPython::Py_buffer_wrapper<Int_t> elecIdx1(pElectronIdx1);
    ttPython::Py_buffer_wrapper<Int_t> muonIdx1(pMuonIdx1);

    ttPython::Py_buffer_wrapper<Float_t> elecPt(pElectron_pt);
    ttPython::Py_buffer_wrapper<Float_t> elecEta(pElectron_eta);
    ttPython::Py_buffer_wrapper<Float_t> elecPhi(pElectron_phi);
    ttPython::Py_buffer_wrapper<Float_t> elecM(pElectron_mass);
    ttPython::Py_buffer_wrapper<Int_t> elecCutBits(pElectron_cutBasedBits);
    ttPython::Py_buffer_wrapper<Float_t> elecMiniPFRelIso(pElectron_miniPFRelIso);

    ttPython::Py_buffer_wrapper<Float_t> muonPt(pMuon_pt);
    ttPython::Py_buffer_wrapper<Float_t> muonEta(pMuon_eta);
    ttPython::Py_buffer_wrapper<Float_t> muonPhi(pMuon_phi);
    ttPython::Py_buffer_wrapper<Float_t> muonM(pMuon_mass);
    ttPython::Py_buffer_wrapper<Bool_t> muonID(pMuon_id);
    ttPython::Py_buffer_wrapper<Float_t> muonPFRelIso(pMuon_pfRelIso);

    //reserve space for the vector to stop reallocations during emplacing
    jetsLV.resize(jetPt.size());
    filterVec.resize(jetPt.size(), true);
    for(unsigned int iJet = 0; iJet < jetPt.size(); ++iJet)
    {
        jetsLV[iJet].SetPtEtaPhiM(jetPt[iJet], jetEta[iJet], jetPhi[iJet], jetM[iJet]);

        //check first and last optional parameter ... assume others are here 
        if(pElectronIdx1 && pMuon_pfRelIso)
        {
            bool isLep = false;

            //recalculate electron ID without isolation 
            //VID compressed bitmap (MinPtCut,GsfEleSCEtaMultiRangeCut,GsfEleDEtaInSeedCut,GsfEleDPhiInCut,GsfEleFull5x5SigmaIEtaIEtaCut,GsfEleHadronicOverEMEnergyScaledCut,GsfEleEInverseMinusPInverseCut,GsfEleRelPFIsoScaledCut,GsfEleConversionVetoCut,GsfEleMissingHitsCut), 3 bits per cut
            //this is a 'bit' awful <- that pun is awful, but yes, this should be made a little better 
            if(elecIdx1[iJet] >= 0 && elecPt[elecIdx1[iJet]] > 10.0)
            {
                const int NCUTS = 10;
                const int BITSTRIDE = 3;
                const int BITMASK = 0x7;
                const int ISOBITMASK = 070000000;  //note to the curious, 0 before an integer is octal, so 070 = 0x38 = 56, so this corrosponds to the three pfRelIso bits 
                int cutBits = elecCutBits[elecIdx1[iJet]] | ISOBITMASK; // the | masks the iso cut
                int elecID = 07; // start with the largest 3 bit number
                for(int i = 0; i < NCUTS; ++i)
                {
                    elecID = std::min(elecID, cutBits & BITMASK);
                    cutBits = cutBits >> BITSTRIDE;
                }

                TLorentzVector tlv;
                tlv.SetPtEtaPhiM(elecPt[elecIdx1[iJet]], elecEta[elecIdx1[iJet]], elecPhi[elecIdx1[iJet]], elecM[elecIdx1[iJet]]);
                double dR = ROOT::Math::VectorUtil::DeltaR(jetsLV[iJet], tlv);

                isLep = isLep || (elecID >= 1 && elecMiniPFRelIso[elecIdx1[iJet]] < 0.10 && dR < 0.2);
            }
            
            if(muonIdx1[iJet] >= 0 && muonPt[muonIdx1[iJet]] > 10.0)
            {
                TLorentzVector tlv;
                tlv.SetPtEtaPhiM(muonPt[muonIdx1[iJet]], muonEta[muonIdx1[iJet]], muonPhi[muonIdx1[iJet]], muonM[muonIdx1[iJet]]);
                double dR = ROOT::Math::VectorUtil::DeltaR(jetsLV[iJet], tlv);

                //pMuon_id == Py_None is a hack because moun loose ID is not a variable, but instead only loose muons are saved 
                isLep = isLep || ((pMuon_id == Py_None || muonID[muonIdx1[iJet]]) && muonPFRelIso[muonIdx1[iJet]] < 0.2 && dR < 0.2);
            }

            //filter out jet if it is matched to a lepton, of if it has pt < 20 GeV
            filterVec[iJet] = !isLep && jetPt[iJet] > 20.0;
        }
    }

    //prepare b-tag discriminator
    tempFloatBuffers.emplace_back(pJetBtag);
    auto& jetBTag = tempFloatBuffers.back();

    //Create the AK4 constituent helper
    ak4ConstInputs.reset(new ttUtility::ConstAK4Inputs<Float_t, ttPython::Py_buffer_wrapper<Float_t>>(jetsLV, jetBTag));
    ak4ConstInputs->setFilterVector(filterVec);

    //prepare floating point supplamental "vectors"
    PyObject *key, *value;
    Py_ssize_t pos = 0;

    while(PyDict_Next(pFloatVarsDict, &pos, &key, &value))
    {
        if(PyString_Check(key))
        {
            //Get the ROOT.PyFloatBuffer into a c++ friendly format
            tempFloatBuffers.emplace_back(value);

            char *vecName = PyString_AsString(key);
            ak4ConstInputs->addSupplamentalVector(vecName, tempFloatBuffers.back());
        }
        else
        {
            //Handle error here
            PyErr_SetString(PyExc_KeyError, "Dictionary keys must be strings for top tagger supplamentary variables.");
            return 1;
        }
    }

    //prepare integer supplamental "vectors" and convert to float vector 
    pos = 0;
    //reserve space for the vector to stop reallocations during emplacing 
    tempIntToFloatVectors.reserve(intSize);
    while(PyDict_Next(pIntVarsDict, &pos, &key, &value)) 
    {
        if(PyString_Check(key))
        {
            //Get the ROOT.PyIntBuffer into a c++ friendly format
            ttPython::Py_buffer_wrapper<Int_t> buffer(value);
            
            //translate the integers to floats
            tempIntToFloatVectors.emplace_back(buffer.begin(), buffer.end());

            //wrap vector in buffer
            tempFloatBuffers.emplace_back(&(tempIntToFloatVectors.back()));
            
            char *vecName = PyString_AsString(key);
            ak4ConstInputs->addSupplamentalVector(vecName, tempFloatBuffers.back());
        }
        else
        {
            //Handle error here
            PyErr_SetString(PyExc_KeyError, "Dictionary keys must be strings for top tagger supplamentary variables.");
            return 1;
        }
    }

    return 0;
}

static int TopTaggerInterface_makeAK8Const(std::unique_ptr<ttUtility::ConstAK8Inputs<Float_t, ttPython::Py_buffer_wrapper<Float_t>>>& ak8ConstInputs, std::vector<ttPython::Py_buffer_wrapper<Float_t>>& tempFloatBuffers, std::vector<TLorentzVector>& jetsLV, std::vector<std::vector<TLorentzVector>>& subjetsLV, PyObject* pArgTuple)
{
    //number of variables
    const unsigned int NFLOATVAR = 3;

    PyObject *pJetPt, *pJetEta, *pJetPhi, *pJetMass, *pJetSDMass, *pJetTDisc, *pJetWDisc, *pSubjetPt, *pSubjetEta, *pSubjetPhi, *pSubjetMass, *pSubjetIdx1, *pSubjetIdx2;
    if (!PyArg_ParseTuple(pArgTuple, "OOOOOOOOOOOOO", &pJetPt, &pJetEta, &pJetPhi, &pJetMass, &pJetSDMass, &pJetTDisc, &pJetWDisc, &pSubjetPt, &pSubjetEta, &pSubjetPhi, &pSubjetMass, &pSubjetIdx1, &pSubjetIdx2))
    {
        return 1;
    }

    //Prepare std::vector<TLorentzVector> for jets and subjets lorentz vectors and subjet linking 
    ttPython::Py_buffer_wrapper<Float_t> jetPt(pJetPt);
    ttPython::Py_buffer_wrapper<Float_t> jetEta(pJetEta);
    ttPython::Py_buffer_wrapper<Float_t> jetPhi(pJetPhi);
    ttPython::Py_buffer_wrapper<Float_t> jetM(pJetMass);

    ttPython::Py_buffer_wrapper<Int_t> subjetIdx1(pSubjetIdx1);
    ttPython::Py_buffer_wrapper<Int_t> subjetIdx2(pSubjetIdx2);

    ttPython::Py_buffer_wrapper<Float_t> subjetPt(pSubjetPt);
    ttPython::Py_buffer_wrapper<Float_t> subjetEta(pSubjetEta);
    ttPython::Py_buffer_wrapper<Float_t> subjetPhi(pSubjetPhi);
    ttPython::Py_buffer_wrapper<Float_t> subjetM(pSubjetMass);

    //reserve space for the vector to stop reallocations during emplacing
    jetsLV.resize(jetPt.size());
    subjetsLV.reserve(jetPt.size());
    for(unsigned int iJet = 0; iJet < jetPt.size(); ++iJet)
    {
        jetsLV[iJet].SetPtEtaPhiM(jetPt[iJet], jetEta[iJet], jetPhi[iJet], jetM[iJet]);

        //reserve space for the vector to stop reallocations during emplacing
        int nSubjet = 0;
        int idx1 = subjetIdx1[iJet];
        int idx2 = subjetIdx2[iJet];
        if(idx1 < 0 && idx2 < 0)        nSubjet = 0;
        else if(idx1 >= 0 && idx2 >= 0) nSubjet = 2;
        else                            nSubjet = 1;

        subjetsLV.emplace_back(nSubjet);
        int iSubjet = 0;
        if(idx1 >= 0) subjetsLV[iJet][iSubjet++].SetPtEtaPhiM(subjetPt[idx1], subjetEta[idx1], subjetPhi[idx1], subjetM[idx1]);
        if(idx2 >= 0) subjetsLV[iJet][iSubjet++].SetPtEtaPhiM(subjetPt[idx2], subjetEta[idx2], subjetPhi[idx2], subjetM[idx2]);
    }

    //Wrap basic floating point vectors
    //reserve space for the vector to stop reallocations during emplacing
    tempFloatBuffers.reserve(NFLOATVAR);

    tempFloatBuffers.emplace_back(pJetSDMass);
    auto& jetSDMass = tempFloatBuffers.back();

    tempFloatBuffers.emplace_back(pJetTDisc);
    auto& jetTopDisc = tempFloatBuffers.back();

    tempFloatBuffers.emplace_back(pJetWDisc);
    auto& jetWDisc = tempFloatBuffers.back();

    //Create the AK8 constituent helper
    ak8ConstInputs.reset(new ttUtility::ConstAK8Inputs<Float_t, ttPython::Py_buffer_wrapper<Float_t>>(jetsLV, jetTopDisc, jetWDisc, jetSDMass, subjetsLV));

    return 0;
}

static int TopTaggerInterface_makeResolvedTopConst(std::unique_ptr<ttUtility::ConstResolvedCandInputs<Float_t, ttPython::Py_buffer_wrapper<Float_t>, ttPython::Py_buffer_wrapper<Int_t>>>& resolvedTopConstInputs, std::vector<ttPython::Py_buffer_wrapper<Float_t>>& tempFloatBuffers, std::vector<ttPython::Py_buffer_wrapper<Int_t>>& tempIntBuffers, std::vector<TLorentzVector>& topCandsLV, PyObject* pArgTuple)
{
    //number of variables
    const unsigned int NFLOATVAR = 1;
    const unsigned int NINTVAR = 3;

    PyObject *pTopCandPt, *pTopCandEta, *pTopCandPhi, *pTopCandMass, *pTopCandDisc, *pTopCandIdxJ1, *pTopCandIdxJ2, *pTopCandIdxJ3;
    if (!PyArg_ParseTuple(pArgTuple, "OOOOOOOO", &pTopCandPt, &pTopCandEta, &pTopCandPhi, &pTopCandMass, &pTopCandDisc, &pTopCandIdxJ1, &pTopCandIdxJ2, &pTopCandIdxJ3))
    {
        return 1;
    }

    //Prepare std::vector<TLorentzVector> for topCand 4-vector
    ttPython::Py_buffer_wrapper<Float_t> topCandPt(pTopCandPt);
    ttPython::Py_buffer_wrapper<Float_t> topCandEta(pTopCandEta);
    ttPython::Py_buffer_wrapper<Float_t> topCandPhi(pTopCandPhi);
    ttPython::Py_buffer_wrapper<Float_t> topCandM(pTopCandMass);
    
    //reserve space for the vector to stop reallocations during emplacing
    topCandsLV.resize(topCandPt.size());
    for(unsigned int iTopCand = 0; iTopCand < topCandPt.size(); ++iTopCand)
    {
        topCandsLV[iTopCand].SetPtEtaPhiM(topCandPt[iTopCand], topCandEta[iTopCand], topCandPhi[iTopCand], topCandM[iTopCand]);
    }

    //Wrap basic floating point/integer vectors
    //reserve space for the vector to stop reallocations during emplacing
    tempFloatBuffers.reserve(NFLOATVAR);
    tempIntBuffers.reserve(NINTVAR);

    tempFloatBuffers.emplace_back(pTopCandDisc);
    auto& topCandDisc = tempFloatBuffers.back();

    tempIntBuffers.emplace_back(pTopCandIdxJ1);
    auto& topCandIdxJ1 = tempIntBuffers.back();

    tempIntBuffers.emplace_back(pTopCandIdxJ2);
    auto& topCandIdxJ2 = tempIntBuffers.back();

    tempIntBuffers.emplace_back(pTopCandIdxJ3);
    auto& topCandIdxJ3 = tempIntBuffers.back();

    //Create the AK8 constituent helper
    resolvedTopConstInputs.reset(new ttUtility::ConstResolvedCandInputs<Float_t, ttPython::Py_buffer_wrapper<Float_t>, ttPython::Py_buffer_wrapper<Int_t>>(topCandsLV, topCandDisc, topCandIdxJ1, topCandIdxJ2, topCandIdxJ3));

    return 0;
}

template<typename A, typename B, typename C>
static inline std::vector<Constituent> createConstituents(A& ak4Inputs, B& ak8Inputs, C& resTopInputs)
{
    //bit mask for terrible switch below
    unsigned int constituentCreationBitMask = 0;
    if(ak4Inputs)    constituentCreationBitMask |= 0x1;
    if(ak8Inputs)    constituentCreationBitMask |= 0x2;
    if(resTopInputs) constituentCreationBitMask |= 0x4;

    //Create constituents 
    switch(constituentCreationBitMask)
    {
    case 0x1: return ttUtility::packageConstituents(*ak4Inputs);
    case 0x5: return ttUtility::packageConstituents(*ak4Inputs, *resTopInputs);
    case 0x2: return ttUtility::packageConstituents(*ak8Inputs);
    case 0x3: return ttUtility::packageConstituents(*ak4Inputs, *ak8Inputs);
    case 0x7: return ttUtility::packageConstituents(*ak4Inputs, *resTopInputs, *ak8Inputs);
    default:
        THROW_TTEXCEPTION("Illegal constituent combination");
        break;
    }
}


extern "C"
{
    static PyObject* TopTaggerInterface_setup(PyObject *self, PyObject *args)
    {
        //suppress unused parameter warning as self is manditory
        (void)self;

        char *cfgFile, *workingDir = nullptr;

        if (!PyArg_ParseTuple(args, "s|s", &cfgFile, &workingDir)) {
            return NULL;
        }

        //Setup top tagger 
        TopTagger *tt = nullptr;

        try
        {
            tt = new TopTagger();

            //Check that "new" succeeded
            if(!tt)
            {
                PyErr_NoMemory();
                return NULL;
            }

            //Disable internal print statements on exception 
            tt->setVerbosity(0);

            if(workingDir && strlen(workingDir) > 0)
            {
                tt->setWorkingDirectory(workingDir);
            }
            tt->setCfgFile(cfgFile);
        }
        catch(const TTException& e)
        {
            std::cout << "TopTagger exception message: " << e << std::endl;
            PyErr_SetString(PyExc_RuntimeError, "TopTagger exception thrown (look above to find specific exception message)");
            return NULL;
        }

        PyObject * ret = PyCapsule_New(tt, "TopTagger", TopTaggerInterface_cleanup);

        return Py_BuildValue("N", ret);
    }

    static PyObject* TopTaggerInterface_run(PyObject *self, PyObject *args, PyObject *kwargs)
    {
        //suppress unused parameter warning as self is manditory
        (void)self;

        PyObject *ptt, *pAK4Inputs = nullptr, *pAK8Inputs = nullptr, *pResolvedTopCandInputs = nullptr;
        //PYTHON, LEARN ABOUT CONST!!!!!!!!!!!!!!!!!!
        char kw1[] = "topTagger", kw2[] = "ak4Inputs", kw3[] = "ak8Inputs", kw4[] = "resolvedTopInputs";
        char *keywords[] = {kw1, kw2, kw3, kw4, NULL};
        if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O!|O!O!O!", keywords, &PyCapsule_Type, &ptt, &PyTuple_Type, &pAK4Inputs, &PyTuple_Type, &pAK8Inputs, &PyTuple_Type, &pResolvedTopCandInputs))
        {
            return NULL;
        }

        Py_INCREF(ptt);
        if(pAK4Inputs) Py_INCREF(pAK4Inputs);
        if(pAK8Inputs) Py_INCREF(pAK8Inputs);
        if(pResolvedTopCandInputs) Py_INCREF(pResolvedTopCandInputs);

        //Prepare ak4 jet input constituents 
        std::unique_ptr<ttUtility::ConstAK4Inputs<Float_t, ttPython::Py_buffer_wrapper<Float_t>>> ak4ConstInputs;
        std::vector<ttPython::Py_buffer_wrapper<Float_t>> ak4TempFloatBuffers;
        std::vector<std::vector<Float_t>> ak4TempIntToFloatVectors;
        std::vector<TLorentzVector> ak4JetsLV;
        std::vector<unsigned char> ak4FilterVec;
        if(pAK4Inputs && TopTaggerInterface_makeAK4Const(ak4ConstInputs, ak4TempFloatBuffers, ak4TempIntToFloatVectors, ak4JetsLV, ak4FilterVec, pAK4Inputs))
        {
            //Status is not 0, there was an error, PyErr_SetString is called in function 
            Py_DECREF(ptt);
            if(pAK4Inputs) Py_DECREF(pAK4Inputs);
            if(pAK8Inputs) Py_DECREF(pAK8Inputs);
            if(pResolvedTopCandInputs) Py_DECREF(pResolvedTopCandInputs);

            return NULL;
        }

        //Prepare ak8 jet input constituents 
        std::unique_ptr<ttUtility::ConstAK8Inputs<Float_t, ttPython::Py_buffer_wrapper<Float_t>>> ak8ConstInputs;
        std::vector<ttPython::Py_buffer_wrapper<Float_t>> ak8TempFloatBuffers;
        std::vector<TLorentzVector> ak8JetsLV;
        std::vector<std::vector<TLorentzVector>> ak8SubjetsLV;
        if(pAK8Inputs && TopTaggerInterface_makeAK8Const(ak8ConstInputs, ak8TempFloatBuffers, ak8JetsLV, ak8SubjetsLV, pAK8Inputs))
        {
            //Status is not 0, there was an error, PyErr_SetString is called in function 
            Py_DECREF(ptt);
            if(pAK4Inputs) Py_DECREF(pAK4Inputs);
            if(pAK8Inputs) Py_DECREF(pAK8Inputs);
            if(pResolvedTopCandInputs) Py_DECREF(pResolvedTopCandInputs);

            return NULL;
        }

        std::unique_ptr<ttUtility::ConstResolvedCandInputs<Float_t, ttPython::Py_buffer_wrapper<Float_t>, ttPython::Py_buffer_wrapper<Int_t>>> resolvedTopConstInputs;
        std::vector<ttPython::Py_buffer_wrapper<Float_t>> resTopTempFloatBuffers;
        std::vector<ttPython::Py_buffer_wrapper<Int_t>> resTopTempIntBuffers;
        std::vector<TLorentzVector> topCandsLV;
        if(pResolvedTopCandInputs && TopTaggerInterface_makeResolvedTopConst(resolvedTopConstInputs, resTopTempFloatBuffers, resTopTempIntBuffers, topCandsLV, pResolvedTopCandInputs))
        {
            //Status is not 0, there was an error, PyErr_SetString is called in function 
            Py_DECREF(ptt);
            if(pAK4Inputs) Py_DECREF(pAK4Inputs);
            if(pAK8Inputs) Py_DECREF(pAK8Inputs);
            if(pResolvedTopCandInputs) Py_DECREF(pResolvedTopCandInputs);

            return NULL;
        }

        //Get top tagger pointer from capsule 
        TopTagger* tt;
        if (!(tt = (TopTagger*) PyCapsule_GetPointer(ptt, "TopTagger"))) 
        {
            //Handle exception here 
            Py_DECREF(ptt);
            if(pAK4Inputs) Py_DECREF(pAK4Inputs);
            if(pAK8Inputs) Py_DECREF(pAK8Inputs);
            if(pResolvedTopCandInputs) Py_DECREF(pResolvedTopCandInputs);

            PyErr_SetString(PyExc_ReferenceError, "TopTagger pointer invalid");
            return NULL;
        }

        //Run top tagger
        try
        {
            //create constituent vector 
            const auto constituents = createConstituents(ak4ConstInputs, ak8ConstInputs, resolvedTopConstInputs);

            //Run top tagger 
            tt->runTagger(constituents);
        }
        catch(const TTException& e)
        {
            Py_DECREF(ptt);
            if(pAK4Inputs) Py_DECREF(pAK4Inputs);
            if(pAK8Inputs) Py_DECREF(pAK8Inputs);
            if(pResolvedTopCandInputs) Py_DECREF(pResolvedTopCandInputs);

            std::cout << "TopTagger exception message: " << e << std::endl;
            PyErr_SetString(PyExc_RuntimeError, "TopTagger exception thrown (look above to find specific exception message)");
            return NULL;
        }

        Py_DECREF(ptt);
        if(pAK4Inputs) Py_DECREF(pAK4Inputs);
        if(pAK8Inputs) Py_DECREF(pAK8Inputs);
        if(pResolvedTopCandInputs) Py_DECREF(pResolvedTopCandInputs);

        Py_INCREF(Py_None);
        return Py_None;
    }

    static PyObject* TopTaggerInterface_getResults(PyObject *self, PyObject *args)
    {
        //suppress unused parameter warning as self is manditory
        (void)self;

        PyObject *ptt;
        if (!PyArg_ParseTuple(args, "O!", &PyCapsule_Type, &ptt))
        {
            return NULL;
        }

        Py_INCREF(ptt);

        //Get top tagger pointer from capsule 
        TopTagger* tt;
        if (!(tt = (TopTagger*) PyCapsule_GetPointer(ptt, "TopTagger"))) 
        {
            //Handle exception here 
            Py_DECREF(ptt);

            PyErr_SetString(PyExc_ReferenceError, "TopTagger pointer invalid");
            return NULL;
        }

        try
        {
            //Get top tagger results 
            const auto& ttr = tt->getResults();

            //Get tops 
            const auto& tops = ttr.getTops();

            //create numpy arrays for passing top data to python
            const npy_intp NVARSFLOAT = 5;
            const npy_intp NVARSINT = 4;
            const npy_intp NTOPS = static_cast<npy_intp>(tops.size());
    
            npy_intp floatsizearray[] = {NTOPS, NVARSFLOAT};
            PyArrayObject* topArrayFloat = reinterpret_cast<PyArrayObject*>(PyArray_SimpleNew(2, floatsizearray, NPY_FLOAT));

            npy_intp intsizearray[] = {NTOPS, NVARSINT};
            PyArrayObject* topArrayInt = reinterpret_cast<PyArrayObject*>(PyArray_SimpleNew(2, intsizearray, NPY_INT));

            //fill numpy array
            for(unsigned int iTop = 0; iTop < tops.size(); ++iTop)
            {
                *static_cast<npy_float*>(PyArray_GETPTR2(topArrayFloat, iTop, 0)) = tops[iTop]->p().Pt();
                *static_cast<npy_float*>(PyArray_GETPTR2(topArrayFloat, iTop, 1)) = tops[iTop]->p().Eta();
                *static_cast<npy_float*>(PyArray_GETPTR2(topArrayFloat, iTop, 2)) = tops[iTop]->p().Phi();
                *static_cast<npy_float*>(PyArray_GETPTR2(topArrayFloat, iTop, 3)) = tops[iTop]->p().M();
                *static_cast<npy_float*>(PyArray_GETPTR2(topArrayFloat, iTop, 4)) = tops[iTop]->getDiscriminator();

                *static_cast<npy_int*>(PyArray_GETPTR2(topArrayInt, iTop, 0)) = static_cast<int>(tops[iTop]->getType());

                //get constituents vector to retrieve matching index
                const auto& topConstituents = tops[iTop]->getConstituents();
                if(topConstituents.size() > 0) *static_cast<npy_int*>(PyArray_GETPTR2(topArrayInt, iTop, 1)) = static_cast<int>(topConstituents[0]->getIndex());
                else                           *static_cast<npy_int*>(PyArray_GETPTR2(topArrayInt, iTop, 1)) = -1;

                if(topConstituents.size() > 1) *static_cast<npy_int*>(PyArray_GETPTR2(topArrayInt, iTop, 2)) = static_cast<int>(topConstituents[1]->getIndex());
                else                           *static_cast<npy_int*>(PyArray_GETPTR2(topArrayInt, iTop, 2)) = -1;

                if(topConstituents.size() > 2) *static_cast<npy_int*>(PyArray_GETPTR2(topArrayInt, iTop, 3)) = static_cast<int>(topConstituents[2]->getIndex());
                else                           *static_cast<npy_int*>(PyArray_GETPTR2(topArrayInt, iTop, 3)) = -1;
            }

            Py_DECREF(ptt);

            return Py_BuildValue("NN", topArrayFloat, topArrayInt);
        }
        catch(const TTException& e)
        {
            Py_DECREF(ptt);

            std::cout << "TopTagger exception message: " << e << std::endl;
            PyErr_SetString(PyExc_RuntimeError, "TopTagger exception thrown (look above to find specific exception message)");
            return NULL;
        }

    }

    static PyMethodDef TopTaggerInterfaceMethods[] = {
        {"setup",       TopTaggerInterface_setup,            METH_VARARGS,                 "Configure Top Tagger."},
        {"run",         (PyCFunction)TopTaggerInterface_run, METH_VARARGS | METH_KEYWORDS, "Run Top Tagger."},
        {"getResults",  TopTaggerInterface_getResults,       METH_VARARGS,                 "Get Top Tagger results."},
        {NULL, NULL, 0, NULL}        /* Sentinel */
    };

    PyMODINIT_FUNC
    initTopTaggerInterface(void)
    {
        (void) Py_InitModule("TopTaggerInterface", TopTaggerInterfaceMethods);

        //Setup numpy
        import_array();
    }

}

#endif
