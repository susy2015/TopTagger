# TopTagger


## Standalone installation instructions

### Installing prerequisites 

To compile the standalone library requires the ROOT 6 [1] and Tensorflow 1.2 (or newer) [2] packages.

[1] https://root.cern.ch/downloading-root

Follow the instructions to install ROOT on your system

[2] https://www.tensorflow.org/install/install_c

Follow the instructions to install tensorflow for C.  GPU support is not necessary unless you will be training new models.  For compatibility with the TopTagger model run the following download command in place of step 2 on this page to download tensorflow v1.3.0 instead of the lastest version 

~~~~~~~~~~~~~sh
TF_TYPE="cpu" # Change to "gpu" for GPU support
 OS="linux" # Change to "darwin" for macOS
 TARGET_DIRECTORY="/usr/local"
 curl -L \
   "https://storage.googleapis.com/tensorflow/libtensorflow/libtensorflow-${TF_TYPE}-${OS}-x86_64-1.3.0.tar.gz" |
   sudo tar -C $TARGET_DIRECTORY -xz
~~~~~~~~~~~~~

### Compiling the top tagger

After all the necessary packages are installed, checkout the TopTagger repository and compile with the following command (with a terminal configured to use root/tensorflow)

~~~~~~~~~~~~~sh
#After downloading the code from github or untaring the standalone tarball 
cd TopTagger/TopTagger/test
./configure
make -j4
#the command below is optional for system wide install (will require sudo rights)
#To install in a non-standard directory add PREFIX=/install/path to the configure command
make install
~~~~~~~~~~~~~

This command will produce a static and a shared library which contain all the necessary symbols to link the top tagger into other C++ code.  This will also compile a standalone example called ``topTaggerTest''.  

## Example code

A basic standalone example using the top tagging code is provided in "TopTagger/test/topTaggerTest.cpp".  This is a basic example program which reads in the necessary top tagger inputs from a file (``exampleInputs.root'') and runs the top tagger code.  As validation, this prints out the number of top quarks reconstructed in each event as well as some basic properties of each top quark reconstructed.  For reference the output of this script can be seen at the end of this readme.  

### Running the example

The example executable is compiled along with the standalone library as described above.  If the top tagger was not installed system wide, then there is a script "taggerSetup.sh" which is produced by the configure command which must be sourced to set system variables appropriately. A ".csh" version is also proveded"

~~~~~~~~~~~~~{sh}
#do once per terminal if opencv or the top tagger are not installed system wide
source taggerSetup.sh
#do once in any directory where you will run the top tagger example
getTaggerCfg.sh -t DeepCombined_Example_v1.0.1
#run the example code
./topTaggerTest
~~~~~~~~~~~~~ 

### Tagger input variables

The top tagging algorithm takes both a collection of all AK4 jets and all AK8 jets (passing basic pt and |eta| acceptance requirments) along with supporting variables.  The input variables used for the example program are listed below

~~~~~~~~~~~~~ c++
//Variables to hold inputs
//AK4 jet variables
//Each entry in these vectors refers to information for 1 AK4 jet
std::vector<TLorentzVector>** AK4JetLV = new std::vector<TLorentzVector>*();
std::vector<double>** AK4JetBtag = new std::vector<double>*();
std::vector<double>** AK4qgMult = new std::vector<double>*();
std::vector<double>** AK4qgPtD = new std::vector<double>*();
std::vector<double>** AK4qgAxis1 = new std::vector<double>*();
std::vector<double>** AK4qgAxis2 = new std::vector<double>*();
std::vector<double>** AK4recoJetschargedHadronEnergyFraction = new std::vector<double>*();
std::vector<double>** AK4recoJetschargedEmEnergyFraction = new std::vector<double>*();
std::vector<double>** AK4recoJetsneutralEmEnergyFraction = new std::vector<double>*();
std::vector<double>** AK4ElectronEnergyFraction = new std::vector<double>*();
std::vector<double>** AK4PhotonEnergyFraction = new std::vector<double>*();
std::vector<double>** AK4recoJetsneutralEnergyFraction = new std::vector<double>*();
std::vector<double>** AK4recoJetsHFHadronEnergyFraction = new std::vector<double>*();
std::vector<double>** AK4recoJetsmuonEnergyFraction = new std::vector<double>*();
std::vector<double>** AK4recoJetsHFEMEnergyFraction = new std::vector<double>*();
std::vector<double>** AK4NeutralHadronMultiplicity = new std::vector<double>*();
std::vector<double>** AK4ChargedHadronMultiplicity = new std::vector<double>*();
std::vector<double>** AK4ElectronMultiplicity = new std::vector<double>*();
std::vector<double>** AK4MuonMultiplicity = new std::vector<double>*();
std::vector<double>** AK4PhotonMultiplicity = new std::vector<double>*();
std::vector<double>** AK4DeepCSVbb = new std::vector<double>*();
std::vector<double>** AK4DeepCSVb = new std::vector<double>*();
std::vector<double>** AK4DeepCSVc = new std::vector<double>*();
std::vector<double>** AK4DeepCSVcc = new std::vector<double>*();

//AK8 jet varaibles
//The elements of each vector refer to one AK8 jet
std::vector<TLorentzVector>** AK8JetLV = new std::vector<TLorentzVector>*();
std::vector<double>** AK8JetTau1 = new std::vector<double>*();
std::vector<double>** AK8JetTau2 = new std::vector<double>*();
std::vector<double>** AK8JetTau3 = new std::vector<double>*();
std::vector<double>** AK8JetSoftdropMass = new std::vector<double>*();
std::vector<std::vector<TLorentzVector>>** AK8SubjetLV = new std::vector<std::vector<TLorentzVector>>*();
std::vector<std::vector<double>>** AK8SubjetBtag  = new std::vector<std::vector<double>>*();
std::vector<std::vector<double>>** AK8SubjetMult  = new std::vector<std::vector<double>>*();
std::vector<std::vector<double>>** AK8SubjetPtD   = new std::vector<std::vector<double>>*();
std::vector<std::vector<double>>** AK8SubjetAxis1 = new std::vector<std::vector<double>>*();
std::vector<std::vector<double>>** AK8SubjetAxis2 = new std::vector<std::vector<double>>*();

~~~~~~~~~~~~~

### Output of example code

In addition to the code output below you may also get some tensorflow warnings about tensorflow not being compiled to tsupport various special CPU instruction sets.  This is ok and these messages can be ignored.  

~~~~~~~~~~~~~
--- Reader                   : Booking "BDTG" of type "BDT" from weights-t2tt850-sm-baseline-nodphi-nomtb-hqu-08112016.xml.
--- MethodBase               : Reading weight file: weights-t2tt850-sm-baseline-nodphi-nomtb-hqu-08112016.xml
--- BDTG                     : Read method "BDTG" of type "BDT"
--- BDTG                     : MVA method was trained with TMVA Version: 4.2.1
--- BDTG                     : MVA method was trained with ROOT Version: 6.06/08
--- DataSetInfo              : Added class "Signal"    with internal class number 0
--- DataSetInfo              : Added class "Background"      with internal class number 1
--- Reader                   : Booked classifier "BDTG" of type: "BDT"
--- Reader                   : Booking "BDTG" of type "BDT" from sdWTag_ttbarTraining_v0.xml.
--- MethodBase               : Reading weight file: sdWTag_ttbarTraining_v0.xml
--- BDTG                     : Read method "BDTG" of type "BDT"
--- BDTG                     : MVA method was trained with TMVA Version: 4.2.1
--- BDTG                     : MVA method was trained with ROOT Version: 6.06/02
--- DataSetInfo              : Added class "Signal"    with internal class number 0
--- DataSetInfo              : Added class "Background"      with internal class number 1
--- Reader                   : Booked classifier "BDTG" of type: "BDT"
Event #: 1
      N tops: 1
      Top properties: N constituents:   3,   Pt:  436.5,   Eta:   1.398,   Phi:  -2.267
          Constituent properties: Constituent type:   1,   Pt:  227.8,   Eta:   1.278,   Phi:  -2.403
          Constituent properties: Constituent type:   1,   Pt:  157.5,   Eta:   1.708,   Phi:  -2.060
          Constituent properties: Constituent type:   1,   Pt:   56.7,   Eta:   0.547,   Phi:  -2.300

Event #: 2
      N tops: 2
      Top properties: N constituents:   3,   Pt:  474.9,   Eta:   1.177,   Phi:   1.374
          Constituent properties: Constituent type:   1,   Pt:  337.1,   Eta:   1.151,   Phi:   1.275
          Constituent properties: Constituent type:   1,   Pt:   90.4,   Eta:   1.486,   Phi:   1.778
          Constituent properties: Constituent type:   1,   Pt:   56.4,   Eta:   0.484,   Phi:   1.334
      Top properties: N constituents:   3,   Pt:  272.0,   Eta:   0.053,   Phi:  -0.078
          Constituent properties: Constituent type:   1,   Pt:  210.2,   Eta:  -0.008,   Phi:   0.092
          Constituent properties: Constituent type:   1,   Pt:   63.0,   Eta:   0.602,   Phi:  -0.334
          Constituent properties: Constituent type:   1,   Pt:   20.0,   Eta:  -1.014,   Phi:  -1.451

Event #: 3
      N tops: 0

Event #: 4
      N tops: 1
      Top properties: N constituents:   3,   Pt:  222.1,   Eta:  -0.617,   Phi:  -2.966
          Constituent properties: Constituent type:   1,   Pt:   87.0,   Eta:   0.195,   Phi:  -2.245
          Constituent properties: Constituent type:   1,   Pt:   86.7,   Eta:  -0.302,   Phi:   2.751
          Constituent properties: Constituent type:   1,   Pt:   84.2,   Eta:  -1.258,   Phi:  -3.096

Event #: 5
      N tops: 1
      Top properties: N constituents:   3,   Pt:  223.0,   Eta:  -0.556,   Phi:  -1.532
          Constituent properties: Constituent type:   1,   Pt:  135.7,   Eta:  -0.938,   Phi:  -1.428
          Constituent properties: Constituent type:   1,   Pt:   57.7,   Eta:  -0.136,   Phi:  -1.023
          Constituent properties: Constituent type:   1,   Pt:   56.5,   Eta:   0.418,   Phi:  -2.376

Event #: 6
      N tops: 1
      Top properties: N constituents:   1,   Pt:  628.1,   Eta:  -0.831,   Phi:   2.056
          Constituent properties: Constituent type:   3,   Pt:  628.1,   Eta:  -0.831,   Phi:   2.056

Event #: 7
      N tops: 0

Event #: 8
      N tops: 1
      Top properties: N constituents:   3,   Pt:  424.4,   Eta:   0.958,   Phi:   2.787
          Constituent properties: Constituent type:   1,   Pt:  261.0,   Eta:   0.838,   Phi:   2.855
          Constituent properties: Constituent type:   1,   Pt:  128.4,   Eta:   1.153,   Phi:   2.411
          Constituent properties: Constituent type:   1,   Pt:   53.4,   Eta:   0.738,   Phi:  -2.913

Event #: 9
      N tops: 0

Event #: 10
      N tops: 1
      Top properties: N constituents:   3,   Pt:  414.0,   Eta:  -0.250,   Phi:  -2.257
          Constituent properties: Constituent type:   1,   Pt:  256.7,   Eta:  -0.340,   Phi:  -2.015
          Constituent properties: Constituent type:   1,   Pt:  135.9,   Eta:   0.088,   Phi:  -2.681
          Constituent properties: Constituent type:   1,   Pt:   41.4,   Eta:  -0.626,   Phi:  -2.398

Event #: 11
      N tops: 1
      Top properties: N constituents:   3,   Pt:  444.4,   Eta:  -0.569,   Phi:   3.114
          Constituent properties: Constituent type:   1,   Pt:  367.1,   Eta:  -0.500,   Phi:   2.977
          Constituent properties: Constituent type:   1,   Pt:   73.6,   Eta:  -0.889,   Phi:  -2.445
          Constituent properties: Constituent type:   1,   Pt:   25.6,   Eta:  -0.055,   Phi:  -3.127

Event #: 12
      N tops: 1
      Top properties: N constituents:   3,   Pt:  180.0,   Eta:  -0.287,   Phi:   1.145
          Constituent properties: Constituent type:   1,   Pt:  141.7,   Eta:  -0.229,   Phi:   1.437
          Constituent properties: Constituent type:   1,   Pt:   37.9,   Eta:  -0.010,   Phi:   0.887
          Constituent properties: Constituent type:   1,   Pt:   32.0,   Eta:  -0.569,   Phi:  -0.184

Event #: 13
      N tops: 1
      Top properties: N constituents:   3,   Pt:  212.3,   Eta:   0.297,   Phi:  -1.735
          Constituent properties: Constituent type:   1,   Pt:  172.2,   Eta:   0.249,   Phi:  -1.297
          Constituent properties: Constituent type:   1,   Pt:   54.2,   Eta:  -0.497,   Phi:  -2.536
          Constituent properties: Constituent type:   1,   Pt:   38.9,   Eta:   1.049,   Phi:  -2.805

Event #: 14
      N tops: 1
      Top properties: N constituents:   3,   Pt:  164.5,   Eta:   1.983,   Phi:  -2.734
          Constituent properties: Constituent type:   1,   Pt:  135.4,   Eta:   1.743,   Phi:  -2.528
          Constituent properties: Constituent type:   1,   Pt:   48.5,   Eta:   1.023,   Phi:  -2.983
          Constituent properties: Constituent type:   1,   Pt:   21.8,   Eta:   2.641,   Phi:   1.216

Event #: 15
      N tops: 0

Event #: 16
      N tops: 2
      Top properties: N constituents:   3,   Pt:  232.8,   Eta:   0.381,   Phi:  -2.796
          Constituent properties: Constituent type:   1,   Pt:  218.7,   Eta:   0.454,   Phi:  -2.650
          Constituent properties: Constituent type:   1,   Pt:   43.7,   Eta:  -0.260,   Phi:   1.801
          Constituent properties: Constituent type:   1,   Pt:   24.4,   Eta:  -0.025,   Phi:  -2.305
      Top properties: N constituents:   3,   Pt:  219.0,   Eta:  -0.721,   Phi:  -1.208
          Constituent properties: Constituent type:   1,   Pt:  165.8,   Eta:  -0.590,   Phi:  -1.427
          Constituent properties: Constituent type:   1,   Pt:   54.8,   Eta:  -0.675,   Phi:  -0.903
          Constituent properties: Constituent type:   1,   Pt:   20.3,   Eta:  -1.137,   Phi:   0.122

Event #: 17
      N tops: 2
      Top properties: N constituents:   3,   Pt:  332.1,   Eta:  -0.661,   Phi:  -1.792
          Constituent properties: Constituent type:   1,   Pt:  172.8,   Eta:  -0.978,   Phi:  -1.621
          Constituent properties: Constituent type:   1,   Pt:  110.1,   Eta:  -0.073,   Phi:  -2.054
          Constituent properties: Constituent type:   1,   Pt:   55.5,   Eta:  -0.524,   Phi:  -1.810
      Top properties: N constituents:   3,   Pt:  194.3,   Eta:  -1.203,   Phi:   2.916
          Constituent properties: Constituent type:   1,   Pt:  157.4,   Eta:  -0.889,   Phi:   2.990
          Constituent properties: Constituent type:   1,   Pt:   36.0,   Eta:  -1.900,   Phi:  -3.015
          Constituent properties: Constituent type:   1,   Pt:   24.3,   Eta:  -0.674,   Phi:   1.492

Event #: 18
      N tops: 1
      Top properties: N constituents:   3,   Pt:  272.1,   Eta:   0.708,   Phi:   0.539
          Constituent properties: Constituent type:   1,   Pt:  131.5,   Eta:   0.935,   Phi:   0.947
          Constituent properties: Constituent type:   1,   Pt:  112.5,   Eta:   0.546,   Phi:  -0.084
          Constituent properties: Constituent type:   1,   Pt:   61.6,   Eta:   0.047,   Phi:   0.760

Event #: 19
      N tops: 2
      Top properties: N constituents:   3,   Pt:  481.0,   Eta:  -0.052,   Phi:  -0.334
          Constituent properties: Constituent type:   1,   Pt:  279.4,   Eta:  -0.122,   Phi:  -0.593
          Constituent properties: Constituent type:   1,   Pt:  122.6,   Eta:   0.468,   Phi:  -0.114
          Constituent properties: Constituent type:   1,   Pt:  101.5,   Eta:  -0.480,   Phi:   0.121
      Top properties: N constituents:   3,   Pt:  136.3,   Eta:  -0.450,   Phi:  -1.407
          Constituent properties: Constituent type:   1,   Pt:  114.3,   Eta:  -0.658,   Phi:  -1.665
          Constituent properties: Constituent type:   1,   Pt:   62.4,   Eta:   0.456,   Phi:  -0.719
          Constituent properties: Constituent type:   1,   Pt:   24.8,   Eta:  -0.469,   Phi:   2.172

Event #: 20
      N tops: 0

Event #: 21
      N tops: 2
      Top properties: N constituents:   3,   Pt:  344.7,   Eta:   0.010,   Phi:   2.483
          Constituent properties: Constituent type:   1,   Pt:  247.4,   Eta:   0.031,   Phi:   2.342
          Constituent properties: Constituent type:   1,   Pt:   76.6,   Eta:  -0.489,   Phi:   2.821
          Constituent properties: Constituent type:   1,   Pt:   29.1,   Eta:   1.013,   Phi:   2.817
      Top properties: N constituents:   3,   Pt:  206.9,   Eta:   0.677,   Phi:  -0.472
          Constituent properties: Constituent type:   1,   Pt:  131.2,   Eta:   0.623,   Phi:  -0.268
          Constituent properties: Constituent type:   1,   Pt:   57.4,   Eta:  -0.335,   Phi:  -0.762
          Constituent properties: Constituent type:   1,   Pt:   25.6,   Eta:   1.901,   Phi:  -0.877

Event #: 22
      N tops: 1
      Top properties: N constituents:   2,   Pt:  322.5,   Eta:   1.844,   Phi:   1.379
          Constituent properties: Constituent type:   3,   Pt:  282.1,   Eta:   1.779,   Phi:   1.597
          Constituent properties: Constituent type:   1,   Pt:   77.1,   Eta:   1.597,   Phi:   0.464

Event #: 23
      N tops: 0

Event #: 24
      N tops: 0

Event #: 25
      N tops: 0

Event #: 26
      N tops: 2
      Top properties: N constituents:   1,   Pt:  435.0,   Eta:  -0.313,   Phi:   1.447
          Constituent properties: Constituent type:   3,   Pt:  435.0,   Eta:  -0.313,   Phi:   1.447
          Top properties: N constituents:   3,   Pt:  172.6,   Eta:  -0.164,   Phi:   1.443
              Constituent properties: Constituent type:   1,   Pt:   95.7,   Eta:   0.046,   Phi:   2.006
              Constituent properties: Constituent type:   1,   Pt:   67.4,   Eta:  -0.459,   Phi:   0.837
              Constituent properties: Constituent type:   1,   Pt:   38.5,   Eta:  -0.023,   Phi:   1.104

Event #: 27
      N tops: 0

Event #: 28
      N tops: 1
      Top properties: N constituents:   1,   Pt:  614.9,   Eta:  -0.303,   Phi:   0.893
          Constituent properties: Constituent type:   3,   Pt:  614.9,   Eta:  -0.303,   Phi:   0.893

Event #: 29
      N tops: 1
      Top properties: N constituents:   3,   Pt:  322.5,   Eta:  -1.178,   Phi:  -3.138
          Constituent properties: Constituent type:   1,   Pt:  276.3,   Eta:  -1.035,   Phi:  -3.025
          Constituent properties: Constituent type:   1,   Pt:   46.2,   Eta:  -1.717,   Phi:  -2.991
          Constituent properties: Constituent type:   1,   Pt:   38.0,   Eta:  -0.260,   Phi:   1.633

Event #: 30
      N tops: 1
      Top properties: N constituents:   2,   Pt:  260.2,   Eta:   0.081,   Phi:   1.268
          Constituent properties: Constituent type:   3,   Pt:  218.5,   Eta:   0.083,   Phi:   1.508
          Constituent properties: Constituent type:   1,   Pt:   70.7,   Eta:   0.042,   Phi:   0.444
~~~~~~~~~~~~~

