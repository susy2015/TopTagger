#ifdef DOPYCAPIBIND
#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include "Python.h"
#include "numpy/arrayobject.h"

#include "TLorentzVector.h"

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

static int TopTaggerInterface_makeAK4Const(std::unique_ptr<ttUtility::ConstAK4Inputs<Float_t, ttPython::Py_buffer_wrapper<Float_t>>>& ak4ConstInputs, std::vector<ttPython::Py_buffer_wrapper<Float_t>>& tempFloatBuffers, std::vector<std::vector<Float_t>>& tempIntToFloatVectors, std::vector<TLorentzVector>& jetsLV, PyObject* pArgTuple)
{
    //number of variables which are not stored in supplamental dictionaries (
    const unsigned int NEXTRAVAR = 1;

    PyObject *pJetPt, *pJetEta, *pJetPhi, *pJetMass, *pJetBtag, *pFloatVarsDict, *pIntVarsDict;
    if (!PyArg_ParseTuple(pArgTuple, "OOOOOO!O!", &pJetPt, &pJetEta, &pJetPhi, &pJetMass, &pJetBtag, &PyDict_Type, &pFloatVarsDict, &PyDict_Type, &pIntVarsDict))
    {
        PyErr_SetString(PyExc_TypeError, "Incorrect function parameters");
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

    //reserve space for the vector to stop reallocations during emplacing
    jetsLV.resize(jetPt.size());
    for(unsigned int iJet = 0; iJet < jetPt.size(); ++iJet)
    {
        jetsLV[iJet].SetPtEtaPhiM(jetPt[iJet], jetEta[iJet], jetPhi[iJet], jetM[iJet]);
    }

    //prepare b-tag discriminator
    tempFloatBuffers.emplace_back(pJetBtag);
    auto& jetBTag = tempFloatBuffers.back();

    //Create the AK4 constituent helper
    ak4ConstInputs.reset(new ttUtility::ConstAK4Inputs<Float_t, ttPython::Py_buffer_wrapper<Float_t>>(jetsLV, jetBTag));

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
        PyErr_SetString(PyExc_TypeError, "Incorrect function parameters");
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

template<typename A, typename B>
static inline std::vector<Constituent> createConstituents(A& ak4Inputs, B& ak8Inputs)
{
    //bit mask for terrible switch below
    unsigned int constituentCreationBitMask = 0;
    if(ak4Inputs) constituentCreationBitMask |= 0x1;
    if(ak8Inputs) constituentCreationBitMask |= 0x2;

    //Create constituents 
    switch(constituentCreationBitMask)
    {
    case 0x1: return ttUtility::packageConstituents(*ak4Inputs);
    case 0x2: return ttUtility::packageConstituents(*ak8Inputs);
    case 0x3: return ttUtility::packageConstituents(*ak4Inputs, *ak8Inputs);
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
            PyErr_SetString(PyExc_TypeError, "Incorrect function parameters");
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

        PyObject *ptt, *pAK4Inputs = nullptr, *pAK8Inputs = nullptr;
        //PYTHON, LEARN ABOUT CONST!!!!!!!!!!!!!!!!!!
        char kw1[] = "topTagger", kw2[] = "ak4Inputs", kw3[] = "ak8Inputs";
        char *keywords[] = {kw1, kw2, kw3, NULL};
        if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O!|O!O!", keywords, &PyCapsule_Type, &ptt, &PyTuple_Type, &pAK4Inputs, &PyTuple_Type, &pAK8Inputs))
        {
            //PyErr_SetString(PyExc_TypeError, "Incorrect function parameters");
            return NULL;
        }

        Py_INCREF(ptt);
        if(pAK4Inputs) Py_INCREF(pAK4Inputs);
        if(pAK8Inputs) Py_INCREF(pAK8Inputs);

        //Prepare ak4 jet input constituents 
        std::unique_ptr<ttUtility::ConstAK4Inputs<Float_t, ttPython::Py_buffer_wrapper<Float_t>>> ak4ConstInputs;
        std::vector<ttPython::Py_buffer_wrapper<Float_t>> ak4TempFloatBuffers;
        std::vector<std::vector<Float_t>> ak4TempIntToFloatVectors;
        std::vector<TLorentzVector> ak4JetsLV;
        if(pAK4Inputs && TopTaggerInterface_makeAK4Const(ak4ConstInputs, ak4TempFloatBuffers, ak4TempIntToFloatVectors, ak4JetsLV, pAK4Inputs))
        {
            //Status is not 0, there was an error, PyErr_SetString is called in function 
            Py_DECREF(ptt);
            if(pAK4Inputs) Py_DECREF(pAK4Inputs);
            if(pAK8Inputs) Py_DECREF(pAK8Inputs);

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

            PyErr_SetString(PyExc_ReferenceError, "TopTagger pointer invalid");
            return NULL;
        }

        //Run top tagger
        try
        {
            //create constituent vector 
            const auto constituents = createConstituents(ak4ConstInputs, ak8ConstInputs);

            //Run top tagger 
            tt->runTagger(constituents);
        }
        catch(const TTException& e)
        {
            Py_DECREF(ptt);
            if(pAK4Inputs) Py_DECREF(pAK4Inputs);
            if(pAK8Inputs) Py_DECREF(pAK8Inputs);

            std::cout << "TopTagger exception message: " << e << std::endl;
            PyErr_SetString(PyExc_RuntimeError, "TopTagger exception thrown (look above to find specific exception message)");
            return NULL;
        }

        Py_DECREF(ptt);
        if(pAK4Inputs) Py_DECREF(pAK4Inputs);
        if(pAK8Inputs) Py_DECREF(pAK8Inputs);

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
            PyErr_SetString(PyExc_TypeError, "Incorrect function parameters");
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
                else                                  *static_cast<npy_int*>(PyArray_GETPTR2(topArrayInt, iTop, 1)) = -1;

                if(topConstituents.size() > 1) *static_cast<npy_int*>(PyArray_GETPTR2(topArrayInt, iTop, 2)) = static_cast<int>(topConstituents[1]->getIndex());
                else                                  *static_cast<npy_int*>(PyArray_GETPTR2(topArrayInt, iTop, 2)) = -1;

                if(topConstituents.size() > 2) *static_cast<npy_int*>(PyArray_GETPTR2(topArrayInt, iTop, 3)) = static_cast<int>(topConstituents[2]->getIndex());
                else                                  *static_cast<npy_int*>(PyArray_GETPTR2(topArrayInt, iTop, 3)) = -1;
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
