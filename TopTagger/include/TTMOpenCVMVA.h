#ifndef TTMOPENCVMVA_H
#define TTMOPENCVMVA_H

#include "TopTagger/TopTagger/include/TTModule.h"

#include <string>

#include "opencv/include/opencv/cv.h"
#include "opencv/include/opencv/mh.h"

class TTMOpenCVMVA : public TTModule
{
private:
    double discriminator_;
    std::string modelFile_;

    cv::Ptr<RTrees> treePtr_;

public:
    void getParameters(const cfg::CfgDocument*);
    void run(TopTaggerResults&);
};
REGISTER_TTMODULE(TTMOpenCVMVA);

#endif
