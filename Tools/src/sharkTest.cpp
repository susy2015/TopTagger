
#include "SusyAnaTools/Tools/NTupleReader.h"

#include <shark/Data/Dataset.h>
#include <shark/Data/Csv.h>
#include <shark/ObjectiveFunctions/Loss/ZeroOneLoss.h>
#include <shark/Data/Dataset.h>
#include <shark/Data/WeightedDataset.h>

#include <shark/Models/Trees/RFClassifier.h>
#include <shark/Algorithms/Trainers/RFTrainer.h>

#include <vector>
#include <string>

#include "TFile.h"
#include "TLegend.h"
#include "TH1.h"
#include "TCanvas.h"

using namespace shark;

//ClassificationDataset getDataset(std::string fname, bool doWeights = false)
WeightedLabeledData<RealVector, unsigned int> getDataset(std::string fname, bool doWeights = false)
{
    static const std::vector<std::string> variables = {"cand_m", "cand_dRMax", "cand_pt", "j12_m", "j13_m", "j23_m", "dR12", "dR23", "dR13", "j1_pt", "j2_pt", "j3_pt", "j1_CSV", "j2_CSV", "j3_CSV", "j1_QGL", "j2_QGL", "j3_QGL", "j12j3_dR", "j13j2_dR", "j23j1_dR"};

    TFile *f_train = TFile::Open(fname.c_str());
    TTree *t_train = (TTree*)f_train->Get("slimmedTuple");

    TH1 * hMatch   = new TH1D("hMatch",   "hMatch",   200, 0, 2000);
    TH1 * hNoMatch = new TH1D("hNoMatch", "hNoMatch", 200, 0, 2000);

    NTupleReader tr_pre(t_train);

    while(tr_pre.getNextEvent())
    {
        const std::vector<double>& cand_pt = tr_pre.getVec<double>("cand_pt");
        const std::vector<int>& genConstiuentMatchesVec = tr_pre.getVec<int>("genConstiuentMatchesVec");

        for(int i = 0; i < cand_pt.size(); ++i)
        {
            if(genConstiuentMatchesVec[i] == 3) hMatch->Fill(cand_pt[i]);
            else                                hNoMatch->Fill(cand_pt[i]);
        }
    }

    NTupleReader tr(t_train);

    std::vector<RealVector> inputs;
    std::vector<unsigned int> labels;
    std::vector<double> weights;

    std::vector<const std::vector<double>*> vecs;

    for(const std::string& var : variables)
    {
        vecs.push_back(nullptr);
    }

    while(tr.getNextEvent())
    {
        for(int i = 0; i < variables.size(); ++i)
        {
            vecs[i] = &tr.getVec<double>(variables[i]);
        }

        const std::vector<int>& genConstiuentMatchesVec = tr.getVec<int>("genConstiuentMatchesVec");

        for(int i = 0; i < vecs.front()->size(); ++i)
        {
            inputs.push_back(RealVector(vecs.size()));

            for(int j = 0; j < vecs.size(); ++j)
            {
                inputs.back()(j) = vecs[j]->at(i);
            }

            bool match = genConstiuentMatchesVec[i] == 3;

            labels.push_back(match);

            if(match) weights.push_back(1/hMatch->Integral());
            else      weights.push_back(1/hNoMatch->Integral());
        }
    }

    auto labeledData = createLabeledDataFromRange(inputs, labels);
    auto weightData = createDataFromRange(weights);
    
    return WeightedLabeledData<RealVector, unsigned int>(labeledData, weightData);
}

int main()
{
    TH1::AddDirectory(false);

    //ClassificationDataset traindata, testdata;
    WeightedLabeledData<RealVector, unsigned int> traindata, testdata;

    //import training data

    std::cout << "Processing training data" << std::endl;

    traindata = getDataset("trainingTuple_division_0_TTbarSingleLep.root");

    std::cout << "Training MVA" << std::endl;

    RFClassifier model;
    RFTrainer trainer;

    trainer.train(model, traindata);

    std::cout << "Processing validation data" << std::endl;

    testdata = getDataset("trainingTuple_division_1_TTbarSingleLep.root");

    std::cout << testdata << std::endl;

    std::cout << "Predicting results" << std::endl;

    Data<RealVector> prediction = model(testdata.inputs());

    std::cout << "Calculating Error" << std::endl;

    ZeroOneLoss<unsigned int, RealVector> loss;
    double error_rate = loss(testdata.labels(), prediction);

    std::cout << "Filling Histograms" << std::endl;

    TH1* hDiscMatch   = new TH1D("hDiscMatch",   "hDiscMatch",   20, 0, 1.0);
    TH1* hDiscNoMatch = new TH1D("hDiscNoMatch", "hDiscNoMatch", 20, 0, 1.0);

    for(int i = 0; i < prediction.elements().size(); ++i)
    {
        const bool matched = testdata.labels().elements()[i];
        if(matched) hDiscMatch->Fill(prediction.elements()[i](1));
        else        hDiscNoMatch->Fill(prediction.elements()[i](1));
    }

    std::cout << "Drawing Histograms" << std::endl;

    TCanvas *c1 = new TCanvas("c1", "c1", 800, 800);
    hDiscMatch->SetLineColor(kRed);
    hDiscNoMatch->SetLineColor(kBlue);

    hDiscMatch->Scale(1.0/hDiscMatch->Integral());
    hDiscNoMatch->Scale(1.0/hDiscNoMatch->Integral());

    hDiscMatch->Draw();
    hDiscNoMatch->Draw("same");

    c1->Print("discriminator.png");
        
    std::cout << "model: " << model.name() << std::endl
              << "trainer: " << trainer.name() << std::endl
              << "test error rate: " << error_rate << std::endl;
}
