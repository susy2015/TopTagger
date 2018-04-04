#ifndef TTMOPENCVMVA_H
#define TTMOPENCVMVA_H

#include "TopTagger/TopTagger/include/TTModule.h"

#include <string>
#include <vector>

#ifdef SHOTTOPTAGGER_DO_OPENCV
#include "opencv/cv.h"
#include "opencv/ml.h"
#endif

/**
 *This module implements an interface to the OpenCV randomforest package for filtering top candidates.  This module can either pass entries directly into the final top list, or filter entries out of the final top list if they do not pass the selection criteria. 
 *
 *@param discCut (float) Minimum threshold for the TMVA discriminator for the candidate to pass the selection
 *@param modelFile (string) Path to the model file
 *@param csvThreshold (float) Threshold on b-tag discriminator to be considered a b-jet.  
 *@param bEtaCut (float) Requirment on |eta| for a constituent to be considered a b-jet
 *@param maxNbInTop (int) The maximum number of constituent jets which can be b-tagged for the candidate to be a final *@param mvaVar[] (string - array) MVA variable input names
 */
class TTMOpenCVMVA : public TTModule
{
#ifdef SHOTTOPTAGGER_DO_OPENCV
private:
    double discriminator_;
    std::string modelFile_;
    double csvThreshold_;
    double bEtaCut_;
    int maxNbInTop_;

    //cv::Ptr is the opencv implementation of a smart pointer
    cv::Ptr<cv::ml::RTrees> treePtr_;
    std::vector<std::string> vars_;

#endif

public:
    void getParameters(const cfg::CfgDocument*, const std::string&);
    void run(TopTaggerResults&);

};
REGISTER_TTMODULE(TTMOpenCVMVA);

#endif
