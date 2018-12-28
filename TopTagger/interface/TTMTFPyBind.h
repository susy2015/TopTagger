#ifndef TTMTFPYBIND_H
#define TTMTFPYBIND_H

#include "TopTagger/TopTagger/interface/TTModule.h"

#include <string>
#include <vector>
#include <memory>

#ifdef DOPYCAPIBIND
#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include "Python.h"
#include "numpy/arrayobject.h"
#endif

namespace ttUtility
{
    class MVAInputCalculator;
}

/**
 *This module implements an interface to Tensorflow through python for filtering top candidates.  This module places top candidates which pass the requirements directly into the final top list.
 *
 *@param discCut (float) Highest minimum discriminator threshold allowed (If discOffest is set > 1 and discSlope is positive then this serves as a basic discriminator threshold)
 *@param discOffset (float) Discriminator cut for zero pt top candidates 
 *@param discSlope (float) Pt dependent slopt for discriminator cut
 *@param modelFile (string) Path to the model file
 *@param NConstituents (int) Type of constituent to apply selection to (1 - monojet, 2 - dijet, 3 - trijet)
 *@param inputOp (string) Input operation for the tensorflow graph
 *@param outputOp (string) Output operation of the tensorflow graph
 *@param csvThreshold (float) Threshold on b-tag discriminator to be considered a b-jet.  
 *@param bEtaCut (float) Requirment on |eta| for a constituent to be considered a b-jet
 *@param maxNbInTop (int) The maximum number of constituent jets which can be b-tagged for the candidate to be a final top (set < 0 to disable)
 *@param mvaVar[] (string - array) MVA variable input names
 */
class TTMTFPyBind : public TTModule
{
private:
#ifdef DOPYCAPIBIND
    double discriminator_;
    double discOffset_;
    double discSlope_;
    double csvThreshold_;
    double bEtaCut_;
    int maxNbInTop_;
    int NConstituents_;

    std::string modelFile_, inputOp_, outputOp_;

    //Input variable names 
    std::vector<std::string> vars_;

    PyObject *pModule_, *pMain_;
    PyObject *pGlobal_;
    PyObject *nparray_;
    PyObject *inputs_;
    
    //variable calclator
    std::unique_ptr<ttUtility::MVAInputCalculator> varCalculator_;

    void initializePyInterpreter();
    PyObject* callPython(const std::string& func, PyObject* pArgs);

#endif

public:
    ~TTMTFPyBind();

    void getParameters(const cfg::CfgDocument*, const std::string&);
    void run(TopTaggerResults&);
};
REGISTER_TTMODULE(TTMTFPyBind);

#endif
