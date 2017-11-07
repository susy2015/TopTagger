#ifndef TTMTFPYBIND_H
#define TTMTFPYBIND_H

#include "TopTagger/TopTagger/include/TTModule.h"

#include <string>
#include <vector>

#ifdef DOTENSORFLOWPYBIND
#include "Python.h"
#include "numpy/arrayobject.h"
#endif

class TTMTFPyBind : public TTModule
{
private:
#ifdef DOTENSORFLOWPYBIND
    double discriminator_;
    double csvThreshold_;
    double bEtaCut_;
    int maxNbInTop_;

    std::string modelFile_, inputOp_, outputOp_;

    //Input variable names 
    std::vector<std::string> vars_;

    PyObject *pModule, *pMain;
    PyObject *pGlobal;

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
