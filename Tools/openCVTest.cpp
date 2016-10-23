
#include "SusyAnaTools/Tools/NTupleReader.h"

#include "TopTagger/TopTagger/include/TopTaggerUtilities.h"
          
#include "opencv/cv.h"       // opencv general include file
#include "opencv/ml.h"  // opencv machine learning include file

#include <vector>
#include <string>
#include <utility>
#include <algorithm> 

#include "TFile.h"
#include "TLegend.h"
#include "TH1.h"
#include "TCanvas.h"

using namespace cv;
using namespace cv::ml; // OpenCV API is in the C++ "cv" namespace

//ClassificationDataset getDataset(std::string fname, bool doWeights = false)
Ptr<TrainData> getDataset(std::vector<std::string> fnames, bool doWeights = false)
{
    static const std::vector<std::string> variables = ttUtility::getMVAVars();

    std::vector<std::vector<float>> inputs;
    std::vector<unsigned int> outputs;
    std::vector<double> pt_weights;

    for(auto& fname : fnames)
    {
        TFile *f_train = TFile::Open(fname.c_str());
        TTree *t_train = (TTree*)f_train->Get("slimmedTuple");

        TH1 * hMatch   = new TH1D("hMatch",   "hMatch",   200, 0, 2000);
        TH1 * hNoMatch = new TH1D("hNoMatch", "hNoMatch", 200, 0, 2000);

        NTupleReader tr_pre(t_train);

        while(tr_pre.getNextEvent())
        {
            if(doWeights && tr_pre.getEvtNum() > 10000) break;
            const std::vector<double>& cand_pt = tr_pre.getVec<double>("cand_pt");
            const std::vector<int>& genConstiuentMatchesVec = tr_pre.getVec<int>("genConstiuentMatchesVec");

            for(int i = 0; i < cand_pt.size(); ++i)
            {
                if(genConstiuentMatchesVec[i] == 3) hMatch->Fill(cand_pt[i]);
                else                                hNoMatch->Fill(cand_pt[i]);
            }
        }

        NTupleReader tr(t_train);

        std::vector<const std::vector<double>*> vecs(variables.size(), nullptr);

        while(tr.getNextEvent())
        {
            if(doWeights && tr.getEvtNum() > 10000) break;
            for(int i = 0; i < variables.size(); ++i)
            {
                vecs[i] = &tr.getVec<double>(variables[i]);
            }

            const std::vector<int>& genConstiuentMatchesVec = tr.getVec<int>("genConstiuentMatchesVec");

            for(int i = 0; i < vecs.front()->size(); ++i)
            {
                std::vector<float> input;

                for(int j = 0; j < vecs.size(); ++j)
                {
                    input.push_back(vecs[j]->at(i));
                }

                int match = genConstiuentMatchesVec[i] == 3;

                inputs.push_back(input);
                outputs.push_back(match);

                if(match) pt_weights.push_back(1.0/hMatch->Integral());
                else      pt_weights.push_back(1.0/hNoMatch->Integral());
            }
        }
    }

    Mat data = Mat(inputs.size(), inputs.front().size(), CV_32F);
    Mat labels = Mat(outputs.size(), 1, CV_32F);
    Mat weights = Mat(outputs.size(), 1, CV_32F);

    //stupid randomization solution
    std::vector<int> sampleIndices(inputs.size(), 0);
    for(int i = 0; i < inputs.size(); ++i) sampleIndices[i] = i;
    std::random_shuffle ( sampleIndices.begin(), sampleIndices.end() );

    //stick data into matrix
    for(int iSample : sampleIndices)
    {
        labels.at<float>(iSample, 0) = outputs[iSample];
        for(int iFeature = 0; iFeature < inputs[iSample].size(); ++iFeature)
        {
            data.at<float>(iSample, iFeature) = inputs[iSample][iFeature];
        }
        weights.at<float>(iSample) = pt_weights[iSample];
    }

    if(doWeights)
    {
        //variables.size()

        return TrainData::create(data, ROW_SAMPLE, labels, Mat(), Mat(), weights);
    }
    else
    {
        return TrainData::create(data, 0, labels);
    }
}

int main()
{
    TH1::AddDirectory(false);

    //import training data

    std::cout << "Processing training data" << std::endl;

    auto trainingData = getDataset({"trainingTuple_division_0_TTbarSingleLep_TRF2.root", "trainingTuple_division_0_ZJetsToNuNu_TRF2.root"}, true);

    std::cout << "Training MVA" << std::endl;

    //Mat var_type = Mat(traindata.cols + 1, 1, CV_8U );
    //var_type.setTo(Scalar(CV_VAR_NUMERICAL) ); // all inputs are numerical
    //
    //// this is a classification problem (i.e. predict a discrete number of class
    //// outputs) so reset the last (+1) output var_type element to CV_VAR_CATEGORICAL
    //
    //var_type.at<uchar>(traindata.cols, 0) = CV_VAR_CATEGORICAL;

    Mat priors = Mat(2, 1, CV_32FC1);  // weights of each classification for classes
    priors.at<float>(0,0) = 1.0;
    priors.at<float>(1,0) = 1.0;
    //// (all equal as equal samples of each digit)
    //
    //RTParams params = RTParams(10, // max depth
    //                           5, // min sample count
    //                           0, // regression accuracy: N/A here
    //                           false, // compute surrogate split, no missing data
    //                           15, // max number of categories (use sub-optimal algorithm for larger numbers)
    //                           priors, // the array of priors
    //                           true,  // calculate variable importance
    //                           4,       // number of variables randomly selected at node and used to find the best split(s).
    //                           100, // max number of trees in the forest
    //                           0.01f,// forrest accuracy
    //                           CV_TERMCRIT_ITER |CV_TERMCRIT_EPS // termination cirteria
    //    );
    
    // train random forest classifier (using training data)

    Ptr<RTrees> rtree = RTrees::create();

    //rtree->setActiveVarCount(int val);
    //rtree->setCalculateVarImportance(true);
    rtree->setTermCriteria(TermCriteria(TermCriteria::COUNT, 100, 0.1));
    //rtree->setCVFolds(int val);
    rtree->setMaxCategories(2);
    rtree->setMaxDepth(12);
    rtree->setMinSampleCount(100);
    //rtree->setPriors(priors);
    //rtree->setRegressionAccuracy(0);
    //rtree->setTruncatePrunedTree(false);
    //rtree->setUse1SERule(false);
    //rtree->setUseSurrogates(false);

    rtree->train(trainingData);

    rtree->save("toptagger.model");

    std::cout << "Processing validation data" << std::endl;
    
    auto validData = getDataset({"trainingTuple_division_1_TTbarSingleLep_TRF2.root"});

    Mat testdata = validData->getSamples();
    Mat testlabels = validData->getResponses();
    
    std::cout << "Predicting results" << std::endl;

    Ptr<RTrees> rtree2 = RTrees::load<RTrees>("toptagger.model");

    std::vector<double> prediction;
    for(int iSample = 0; iSample < testdata.rows; ++iSample)
    {
        // extract a row from the testing matrix
        Mat testSample = testdata.row(iSample);

        prediction.push_back(rtree2->predict(testSample));
    }

    //std::cout << "Calculating Error" << std::endl;
    
    std::cout << "Filling Histograms" << std::endl;
    
    TH1* hDiscMatch   = new TH1D("hDiscMatch",   "hDiscMatch",   20, 0, 1.0);
    TH1* hDiscNoMatch = new TH1D("hDiscNoMatch", "hDiscNoMatch", 20, 0, 1.0);
    
    for(int i = 0; i < prediction.size(); ++i)
    {
        const bool matched = testlabels.at<float>(i, 0);
        if(matched) hDiscMatch->Fill(prediction[i]);
        else        hDiscNoMatch->Fill(prediction[i]);
    }
    
    std::cout << "Drawing Histograms" << std::endl;
    
    TCanvas *c1 = new TCanvas("c1", "c1", 800, 800);
    hDiscMatch->SetLineColor(kRed);
    hDiscNoMatch->SetLineColor(kBlue);
    
    hDiscMatch->Scale(1.0/hDiscMatch->Integral());
    hDiscNoMatch->Scale(1.0/hDiscNoMatch->Integral());

    hDiscMatch->GetYaxis()->SetRangeUser(0, 1.3 * std::max(hDiscMatch->GetMaximum(), hDiscNoMatch->GetMaximum()));
    
    hDiscMatch->Draw();
    hDiscNoMatch->Draw("same");
    
    c1->Print("discriminator.png");
}
