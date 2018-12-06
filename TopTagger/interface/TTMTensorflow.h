#ifndef TTMTENSORFLOW_H
#define TTMTENSORFLOW_H

#include "TopTagger/TopTagger/interface/TTModule.h"

#include <string>
#include <memory>
#include <vector>

#ifdef DOTENSORFLOW
#include "tensorflow/c/c_api.h"
#endif

//class TF_Session;
//class TF_Buffer;
//class TF_Output;
//class TF_Operation;

namespace ttUtility
{
    class MVAInputCalculator;
}

/**
 *This module implements an interface to Tensorflow through the tensorflow c-api for filtering top candidates.  This module places top candidates which pass the requirements directly into the final top list.
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
class TTMTensorflow : public TTModule
{
private:
#ifdef DOTENSORFLOW
    double discriminator_;
    double discOffset_;
    double discSlope_;
    std::string modelFile_, inputOp_, outputOp_;
    double csvThreshold_;
    double bEtaCut_;
    int maxNbInTop_;
    int NConstituents_;

    //Tensoflow session pointer
    TF_Session *session_;

    //Input variable names 
    std::vector<std::string> vars_;

    std::vector<TF_Output>     inputs_;
    std::vector<TF_Output>     outputs_;
    std::vector<TF_Operation*> targets_;

    TF_Buffer* read_file(const std::string& file);

    //variable calclator                                                                                                                                                                                                                     
    std::unique_ptr<ttUtility::MVAInputCalculator> varCalculator_;
#endif

public:
    ~TTMTensorflow();

    void getParameters(const cfg::CfgDocument*, const std::string&);
    void run(TopTaggerResults&);
};
REGISTER_TTMODULE(TTMTensorflow);

#endif
