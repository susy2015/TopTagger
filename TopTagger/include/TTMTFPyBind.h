#ifndef TTMTFPYBIND_H
#define TTMTFPYBIND_H

#include "TopTagger/TopTagger/include/TTModule.h"

#include <string>
#include <vector>
#include <memory>

#ifdef DOTENSORFLOWPYBIND
#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include "Python.h"
#include "numpy/arrayobject.h"
#endif

namespace ttUtility
{
    class MVAInputCalculator;
}

class TTMTFPyBind : public TTModule
{
private:
#ifdef DOTENSORFLOWPYBIND
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
