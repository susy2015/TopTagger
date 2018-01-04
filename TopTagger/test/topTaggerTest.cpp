#include <cstdio>
#include <vector>

#include "TFile.h"
#include "TTree.h"
#include "TLorentzVector.h"

//manditory includes to use top tagger
#include "TopTagger/TopTagger/include/TopTagger.h"
#include "TopTagger/TopTagger/include/TopTaggerResults.h"
//this include is useful to get the helper function to make the vector of constituents
#include "TopTagger/TopTagger/include/TopTaggerUtilities.h"

//this include is necessary to handle exceptions thrown by the top tagger code
#include "TopTagger/CfgParser/include/TTException.h"

#include "rootdict.h"

int main()
{
    //Open input ntuple file 
    TFile *tf = TFile::Open("exampleInputs.root");

    //Get tree from file
    TTree *tree = (TTree*)tf->Get("taggerInputs");

    //Deactivate all branches, then activate the branches of interest
    tree->SetBranchStatus("*", 0);

    //Variables to hold inputs
    //AK4 jet variables
    //Each entry in these vectors refers to information for 1 AK4 jet
    std::vector<TLorentzVector>** AK4JetLV = new std::vector<TLorentzVector>*();
    std::vector<double>** AK4JetBtag = new std::vector<double>*();
    std::vector<double>** AK4JetQGL = new std::vector<double>*();

    //AK8 jet varaibles
    //The elements of each vector refer to one AK8 jet
    std::vector<TLorentzVector>** AK8JetLV = new std::vector<TLorentzVector>*();
    std::vector<double>** AK8JetTau1 = new std::vector<double>*();
    std::vector<double>** AK8JetTau2 = new std::vector<double>*();
    std::vector<double>** AK8JetTau3 = new std::vector<double>*();
    std::vector<double>** AK8JetSoftdropMass = new std::vector<double>*();
    std::vector<TLorentzVector>** AK8SubjetLV = new std::vector<TLorentzVector>*();
    std::vector<double>** AK8SubjetBtag  = new std::vector<double>*();
    std::vector<double>** AK8SubjetMult  = new std::vector<double>*();
    std::vector<double>** AK8SubjetPtD   = new std::vector<double>*();
    std::vector<double>** AK8SubjetAxis1 = new std::vector<double>*();
    std::vector<double>** AK8SubjetAxis2 = new std::vector<double>*();

    //Activate branches of interest
    //AK4 jet lorentz vectors
    tree->SetBranchStatus( "jetsLVec", 1);
    tree->SetBranchAddress("jetsLVec", AK4JetLV);
    
    //AK4 jet b-tag values (0 not a b, 1 is a b)
    tree->SetBranchStatus( "recoJetsBtag_0", 1);
    tree->SetBranchAddress("recoJetsBtag_0", AK4JetBtag);

    //AK4 jet QGL
    tree->SetBranchStatus( "qgLikelihood", 1);
    tree->SetBranchAddress("qgLikelihood", AK4JetQGL);
    
    //AK8 jet lorentz vectors
    tree->SetBranchStatus( "puppiJetsLVec", 1);
    tree->SetBranchAddress("puppiJetsLVec", AK8JetLV);
    
    //AK8 subjet lorentz vectors (soft drop algo produces 2 subjets for each AK8 jet)
    //All are present in this list and dR matching associates them to the approperiate AK8 jet
    tree->SetBranchStatus( "puppiSubJetsLVec", 1);
    tree->SetBranchAddress("puppiSubJetsLVec", AK8SubjetLV);

    tree->SetBranchStatus( "puppiSubJetsBdisc", 1);
    tree->SetBranchAddress("puppiSubJetsBdisc", AK8SubjetBtag);

    tree->SetBranchStatus( "puppiSubJetstotalMult", 1);
    tree->SetBranchAddress("puppiSubJetstotalMult", AK8SubjetMult);

    tree->SetBranchStatus( "puppiSubJetsptD", 1);
    tree->SetBranchAddress("puppiSubJetsptD", AK8SubjetPtD);

    tree->SetBranchStatus( "puppiSubJetsaxis1", 1);
    tree->SetBranchAddress("puppiSubJetsaxis1", AK8SubjetAxis1);

    tree->SetBranchStatus( "puppiSubJetsaxis2", 1);
    tree->SetBranchAddress("puppiSubJetsaxis2", AK8SubjetAxis2);

    //AK8 jet tau1 variable
    tree->SetBranchStatus( "puppitau1", 1);
    tree->SetBranchAddress("puppitau1", AK8JetTau1);
    
    //AK8 jet tau2 variable
    tree->SetBranchStatus( "puppitau2", 1);
    tree->SetBranchAddress("puppitau2", AK8JetTau2);
    
    //AK8 jet tau3 variable
    tree->SetBranchStatus( "puppitau3", 1);
    tree->SetBranchAddress("puppitau3", AK8JetTau3);
    
    //AK8 jet softdrop mass
    tree->SetBranchStatus( "puppisoftDropMass", 1);
    tree->SetBranchAddress("puppisoftDropMass", AK8JetSoftdropMass);

    //Create top tagger object
    TopTagger tt;

    //try-catch on TTException which are thrown by the top tagger
    try
    {
        //Set top tagger cfg file
        tt.setCfgFile("TopTagger.cfg");

        //Loop over events
        int Nevt = 0;
        while(tree->GetEntry(Nevt))
        {
            //increment event number
            ++Nevt;

            //Print event number 
            printf("Event #: %i\n", Nevt);

            //Use helper function to create input list 
            //Create AK4 inputs object
            ttUtility::ConstAK4Inputs AK4Inputs = ttUtility::ConstAK4Inputs(**AK4JetLV, **AK4JetBtag, **AK4JetQGL);

            //Create AK8 inputs object
            ttUtility::ConstAK8Inputs AK8Inputs = ttUtility::ConstAK8Inputs(
                **AK8JetLV,
                **AK8JetTau1,
                **AK8JetTau2,
                **AK8JetTau3,
                **AK8JetSoftdropMass,
                **AK8SubjetLV,
                **AK8SubjetBtag,
                **AK8SubjetMult,
                **AK8SubjetPtD,
                **AK8SubjetAxis1,
                **AK8SubjetAxis2
                );

            //Create jets constituents list combining AK4 and AK8 jets, these are used to construct top candiates
            //The vector of input constituents can also be constructed "by hand"
            std::vector<Constituent> constituents = ttUtility::packageConstituents(AK4Inputs, AK8Inputs);

            //run the top tagger
            tt.runTagger(constituents);

            //retrieve the top tagger results object
            const TopTaggerResults& ttr = tt.getResults();

            //get reconstructed top
            const std::vector<TopObject*>& tops = ttr.getTops();

            //print the number of tops found in the event 
            printf("\tN tops: %ld\n", tops.size());

            //print top properties
            for(const TopObject* top : tops)
            {
                //print basic top properties (top->p() gives a TLorentzVector)
                //N constituents refers to the number of jets included in the top
                //3 for resolved tops 
                //2 for W+jet tops
                //1 for fully merged AK8 tops
                printf("\tTop properties: N constituents: %3d,   Pt: %6.1lf,   Eta: %7.3lf,   Phi: %7.3lf\n", top->getNConstituents(), top->p().Pt(), top->p().Eta(), top->p().Phi());

                //get vector of top constituents 
                const std::vector<Constituent const *>& constituents = top->getConstituents();

                //Print properties of individual top constituent jets 
                for(const Constituent* constituent : constituents)
                {
                    printf("\t\tConstituent properties: Constituent type: %3d,   Pt: %6.1lf,   Eta: %7.3lf,   Phi: %7.3lf\n", constituent->getType(), constituent->p().Pt(), constituent->p().Eta(), constituent->p().Phi());
                }                
            }

            //Print properties of the remaining system
            //the remaining system is used as the second portion of the visible system to calculate MT2 in the NT = 1 bin
            const TopObject& rsys = ttr.getRsys();
            printf("\tRsys properties: N constituents: %3d,   Pt: %6.1lf,   Eta: %7.3lf,   Phi: %7.3lf\n", rsys.getNConstituents(), rsys.p().Pt(), rsys.p().Eta(), rsys.p().Phi());
        
            printf("\n");
        }
    }
    catch(const TTException& e)
    {
        //Print exception message
        e.print();
        printf("Terminating run\n");
        fflush(stdout);

        exit(1);
    }
    
    //clean up pointers 
    delete AK4JetLV;
    delete AK4JetBtag;
    delete AK8JetLV;
    delete AK8SubjetLV;
    delete AK8JetTau1;
    delete AK8JetTau2;
    delete AK8JetTau3;
    delete AK8JetSoftdropMass;

    exit(0);
}
