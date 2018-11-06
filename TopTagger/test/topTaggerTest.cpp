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

int main()
{
    //Open input ntuple file 
    TFile *tf = TFile::Open("exampleInputs.root");

    //Get tree from file
    TTree *tree = (TTree*)tf->Get("slimmedTuple");

    //Deactivate all branches, then activate the branches of interest
    tree->SetBranchStatus("*", 0);

    //Variables to hold inputs
    //AK4 jet variables
    //Each entry in these vectors refers to information for 1 AK4 jet
    std::vector<TLorentzVector>** AK4JetLV = new std::vector<TLorentzVector>*();
    std::vector<float>** AK4JetBtag = new std::vector<float>*();
    std::vector<float>** AK4qgMult = new std::vector<float>*();
    std::vector<float>** AK4qgPtD = new std::vector<float>*();
    std::vector<float>** AK4qgAxis1 = new std::vector<float>*();
    std::vector<float>** AK4qgAxis2 = new std::vector<float>*();
    std::vector<float>** AK4recoJetschargedHadronEnergyFraction = new std::vector<float>*();
    std::vector<float>** AK4recoJetschargedEmEnergyFraction = new std::vector<float>*();
    std::vector<float>** AK4recoJetsneutralEmEnergyFraction = new std::vector<float>*();
    std::vector<float>** AK4ElectronEnergyFraction = new std::vector<float>*();
    std::vector<float>** AK4PhotonEnergyFraction = new std::vector<float>*();
    std::vector<float>** AK4recoJetsneutralEnergyFraction = new std::vector<float>*();
    std::vector<float>** AK4recoJetsHFHadronEnergyFraction = new std::vector<float>*();
    std::vector<float>** AK4recoJetsmuonEnergyFraction = new std::vector<float>*();
    std::vector<float>** AK4recoJetsHFEMEnergyFraction = new std::vector<float>*();
    std::vector<float>** AK4NeutralHadronMultiplicity = new std::vector<float>*();
    std::vector<float>** AK4ChargedHadronMultiplicity = new std::vector<float>*();
    std::vector<float>** AK4ElectronMultiplicity = new std::vector<float>*();
    std::vector<float>** AK4MuonMultiplicity = new std::vector<float>*();
    std::vector<float>** AK4PhotonMultiplicity = new std::vector<float>*();
    std::vector<float>** AK4DeepCSVbb = new std::vector<float>*();
    std::vector<float>** AK4DeepCSVb = new std::vector<float>*();
    std::vector<float>** AK4DeepCSVc = new std::vector<float>*();
    std::vector<float>** AK4DeepCSVcc = new std::vector<float>*();
    std::vector<float>** AK4DeepCSVl = new std::vector<float>*();

    //AK8 jet varaibles
    //The elements of each vector refer to one AK8 jet
    std::vector<TLorentzVector>** AK8JetLV = new std::vector<TLorentzVector>*();
    std::vector<float>** AK8JetSoftdropMass = new std::vector<float>*();
    std::vector<float>** AK8JetDeepAK8Top = new std::vector<float>*();
    std::vector<float>** AK8JetDeepAK8W = new std::vector<float>*();
    std::vector<std::vector<TLorentzVector>>** AK8SubjetLV = new std::vector<std::vector<TLorentzVector>>*();

    //Activate branches of interest
    //AK4 jet lorentz vectors
    tree->SetBranchStatus( "ak4jetsLVec", 1);
    tree->SetBranchAddress("ak4jetsLVec", AK4JetLV);
    
    //AK4 jet b-tag values (0 not a b, 1 is a b)
    tree->SetBranchStatus( "ak4recoJetsBtag", 1);
    tree->SetBranchAddress("ak4recoJetsBtag", AK4JetBtag);

    //AK4 qg jet multiplicity
    tree->SetBranchStatus( "ak4qgMult", 1);
    tree->SetBranchAddress("ak4qgMult", AK4qgMult);

    //AK4 qg PtD
    tree->SetBranchStatus( "ak4qgPtD", 1);
    tree->SetBranchAddress("ak4qgPtD", AK4qgPtD);

    //AK4 qg jet semimajor axis
    tree->SetBranchStatus( "ak4qgAxis1", 1);
    tree->SetBranchAddress("ak4qgAxis1", AK4qgAxis1);

    //AK4 qg jet semiminor axis
    tree->SetBranchStatus( "ak4qgAxis2", 1);
    tree->SetBranchAddress("ak4qgAxis2", AK4qgAxis2);

    //AK4 jet charged hadronic energy fraction 
    tree->SetBranchStatus( "ak4recoJetschargedHadronEnergyFraction", 1);
    tree->SetBranchAddress("ak4recoJetschargedHadronEnergyFraction", AK4recoJetschargedHadronEnergyFraction);

    //AK4 jet charged electromagnetic energy fraction 
    tree->SetBranchStatus( "ak4recoJetschargedEmEnergyFraction", 1);
    tree->SetBranchAddress("ak4recoJetschargedEmEnergyFraction", AK4recoJetschargedEmEnergyFraction);

    //AK4 jet neutral hadronic energy fraction 
    tree->SetBranchStatus( "ak4recoJetsneutralEmEnergyFraction", 1);
    tree->SetBranchAddress("ak4recoJetsneutralEmEnergyFraction", AK4recoJetsneutralEmEnergyFraction);

    //AK4 electron energy fraction
    tree->SetBranchStatus( "ak4ElectronEnergyFraction", 1);
    tree->SetBranchAddress("ak4ElectronEnergyFraction", AK4ElectronEnergyFraction);

    //AK4 photon energy fraction 
    tree->SetBranchStatus( "ak4PhotonEnergyFraction", 1);
    tree->SetBranchAddress("ak4PhotonEnergyFraction", AK4PhotonEnergyFraction);

    //AK4 neutral energy fraction
    tree->SetBranchStatus( "ak4recoJetsneutralEnergyFraction", 1);
    tree->SetBranchAddress("ak4recoJetsneutralEnergyFraction", AK4recoJetsneutralEnergyFraction);

    //AK4 HF hadronic energy fraction 
    tree->SetBranchStatus( "ak4recoJetsHFHadronEnergyFraction", 1);
    tree->SetBranchAddress("ak4recoJetsHFHadronEnergyFraction", AK4recoJetsHFHadronEnergyFraction);

    //AK4 muon energy fraction 
    tree->SetBranchStatus( "ak4recoJetsmuonEnergyFraction", 1);
    tree->SetBranchAddress("ak4recoJetsmuonEnergyFraction", AK4recoJetsmuonEnergyFraction);

    //AK4 HF electron energy fraction 
    tree->SetBranchStatus( "ak4recoJetsHFEMEnergyFraction", 1);
    tree->SetBranchAddress("ak4recoJetsHFEMEnergyFraction", AK4recoJetsHFEMEnergyFraction);

    //AK4 neutral hadronic particle multiplicity
    tree->SetBranchStatus( "ak4NeutralHadronMultiplicity", 1);
    tree->SetBranchAddress("ak4NeutralHadronMultiplicity", AK4NeutralHadronMultiplicity);

    //AK4 charged hadronic particle multiplicity
    tree->SetBranchStatus( "ak4ChargedHadronMultiplicity", 1);
    tree->SetBranchAddress("ak4ChargedHadronMultiplicity", AK4ChargedHadronMultiplicity);

    //AK4 electron multiplicity
    tree->SetBranchStatus( "ak4ElectronMultiplicity", 1);
    tree->SetBranchAddress("ak4ElectronMultiplicity", AK4ElectronMultiplicity);

    //AK4 muon multiplicity
    tree->SetBranchStatus( "ak4MuonMultiplicity", 1);
    tree->SetBranchAddress("ak4MuonMultiplicity", AK4MuonMultiplicity);

    //AK4 photon multiplicity
    tree->SetBranchStatus( "ak4PhotonMultiplicity", 1);
    tree->SetBranchAddress("ak4PhotonMultiplicity", AK4PhotonMultiplicity);

    //AK4 deepCSV bb discriminator
    tree->SetBranchStatus( "ak4DeepCSVbb", 1);
    tree->SetBranchAddress("ak4DeepCSVbb", AK4DeepCSVbb);

    //AK4 deepCSV b discriminator
    tree->SetBranchStatus( "ak4DeepCSVb", 1);
    tree->SetBranchAddress("ak4DeepCSVb", AK4DeepCSVb);

    //AK4 deepCSV charm discriminator
    tree->SetBranchStatus( "ak4DeepCSVc", 1);
    tree->SetBranchAddress("ak4DeepCSVc", AK4DeepCSVc);

    //AK4 deepCSV double charm discriminator
    tree->SetBranchStatus( "ak4DeepCSVcc", 1);
    tree->SetBranchAddress("ak4DeepCSVcc", AK4DeepCSVcc);

    //AK4 deepCSV light discriminator
    tree->SetBranchStatus( "ak4DeepCSVl", 1);
    tree->SetBranchAddress("ak4DeepCSVl", AK4DeepCSVl);

    
    //AK8 jet lorentz vectors
    tree->SetBranchStatus( "ak8JetsLVec", 1);
    tree->SetBranchAddress("ak8JetsLVec", AK8JetLV);
    
    //AK8 subjet lorentz vectors (soft drop algo produces 2 subjets for each AK8 jet)
    tree->SetBranchStatus( "ak8SubJetsLVec", 1);
    tree->SetBranchAddress("ak8SubJetsLVec", AK8SubjetLV);

    //AK8 jet deepAK8 top discriminator variable
    tree->SetBranchStatus( "ak8DeepAK8Top", 1);
    tree->SetBranchAddress("ak8DeepAK8Top", AK8JetDeepAK8Top);
    
    //AK8 jet deepAK8 W discriminator variable
    tree->SetBranchStatus( "ak8DeepAK8W", 1);
    tree->SetBranchAddress("ak8DeepAK8W", AK8JetDeepAK8W);
    
    //AK8 jet softdrop mass
    tree->SetBranchStatus( "ak8softDropMass", 1);
    tree->SetBranchAddress("ak8softDropMass", AK8JetSoftdropMass);

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
            ttUtility::ConstAK4Inputs<float> AK4Inputs(**AK4JetLV, **AK4JetBtag);
            AK4Inputs.addSupplamentalVector("qgPtD",                                **AK4qgPtD);
            AK4Inputs.addSupplamentalVector("qgAxis1",                              **AK4qgAxis1);
            AK4Inputs.addSupplamentalVector("qgAxis2",                              **AK4qgAxis2);
            AK4Inputs.addSupplamentalVector("qgMult",                               **AK4qgMult);
            AK4Inputs.addSupplamentalVector("recoJetschargedHadronEnergyFraction",  **AK4recoJetschargedHadronEnergyFraction);
            AK4Inputs.addSupplamentalVector("recoJetschargedEmEnergyFraction",      **AK4recoJetschargedEmEnergyFraction);
            AK4Inputs.addSupplamentalVector("recoJetsneutralEmEnergyFraction",      **AK4recoJetsneutralEmEnergyFraction);
            AK4Inputs.addSupplamentalVector("recoJetsmuonEnergyFraction",           **AK4recoJetsmuonEnergyFraction);
            AK4Inputs.addSupplamentalVector("recoJetsHFHadronEnergyFraction",       **AK4recoJetsHFHadronEnergyFraction);
            AK4Inputs.addSupplamentalVector("recoJetsHFEMEnergyFraction",           **AK4recoJetsHFEMEnergyFraction);
            AK4Inputs.addSupplamentalVector("recoJetsneutralEnergyFraction",        **AK4recoJetsneutralEnergyFraction);
            AK4Inputs.addSupplamentalVector("PhotonEnergyFraction",                 **AK4PhotonEnergyFraction);
            AK4Inputs.addSupplamentalVector("ElectronEnergyFraction",               **AK4ElectronEnergyFraction);
            AK4Inputs.addSupplamentalVector("ChargedHadronMultiplicity",            **AK4ChargedHadronMultiplicity);
            AK4Inputs.addSupplamentalVector("NeutralHadronMultiplicity",            **AK4NeutralHadronMultiplicity);
            AK4Inputs.addSupplamentalVector("PhotonMultiplicity",                   **AK4PhotonMultiplicity);
            AK4Inputs.addSupplamentalVector("ElectronMultiplicity",                 **AK4ElectronMultiplicity);
            AK4Inputs.addSupplamentalVector("MuonMultiplicity",                     **AK4MuonMultiplicity);
            AK4Inputs.addSupplamentalVector("DeepCSVb",                             **AK4DeepCSVb);
            AK4Inputs.addSupplamentalVector("DeepCSVc",                             **AK4DeepCSVc);
            AK4Inputs.addSupplamentalVector("DeepCSVl",                             **AK4DeepCSVl);
            AK4Inputs.addSupplamentalVector("DeepCSVbb",                            **AK4DeepCSVbb);
            AK4Inputs.addSupplamentalVector("DeepCSVcc",                            **AK4DeepCSVcc);

            
            //Create AK8 inputs object
            ttUtility::ConstAK8Inputs<float> AK8Inputs(
                **AK8JetLV,
                **AK8JetDeepAK8Top,
                **AK8JetDeepAK8W,
                **AK8JetSoftdropMass,
                **AK8SubjetLV
                );
            
            //Create jets constituents list combining AK4 and AK8 jets, these are used to construct top candiates
            //The vector of input constituents can also be constructed "by hand"
            std::vector<Constituent> constituents = ttUtility::packageConstituents(AK4Inputs, AK8Inputs);

            //run the top tagger
            tt.runTagger(std::move(constituents));

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
                printf("\tTop properties: Type: %3d,   Pt: %6.1lf,   Eta: %7.3lf,   Phi: %7.3lf,   M: %7.3lf\n", static_cast<int>(top->getType()), top->p().Pt(), top->p().Eta(), top->p().Phi(), top->p().M());

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
            //const TopObject& rsys = ttr.getRsys();
            //printf("\tRsys properties: N constituents: %3d,   Pt: %6.1lf,   Eta: %7.3lf,   Phi: %7.3lf\n", rsys.getNConstituents(), rsys.p().Pt(), rsys.p().Eta(), rsys.p().Phi());
        
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
    delete AK4qgMult;
    delete AK4qgPtD;
    delete AK4qgAxis1;
    delete AK4qgAxis2;
    delete AK4recoJetschargedHadronEnergyFraction;
    delete AK4recoJetschargedEmEnergyFraction;
    delete AK4recoJetsneutralEmEnergyFraction;
    delete AK4ElectronEnergyFraction;
    delete AK4PhotonEnergyFraction;
    delete AK4recoJetsneutralEnergyFraction;
    delete AK4recoJetsHFHadronEnergyFraction;
    delete AK4recoJetsmuonEnergyFraction;
    delete AK4recoJetsHFEMEnergyFraction;
    delete AK4NeutralHadronMultiplicity;
    delete AK4ChargedHadronMultiplicity;
    delete AK4ElectronMultiplicity;
    delete AK4MuonMultiplicity;
    delete AK4PhotonMultiplicity;
    delete AK4DeepCSVbb;
    delete AK4DeepCSVb;
    delete AK4DeepCSVc;
    delete AK4DeepCSVcc;
    delete AK4DeepCSVl;

    delete AK8JetLV;
    delete AK8SubjetLV;
    delete AK8JetDeepAK8Top;
    delete AK8JetSoftdropMass;

    exit(0);
}
