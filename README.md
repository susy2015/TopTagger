# TopTagger


## Standalone installation instructions

### Installing prerequisites 

To compile the standalone library requires the python 2.7 [0], ROOT 6 [1], scipy/numpy [2], and Tensorflow 1.2 (or newer) [3] packages.

[0] https://www.python.org/downloads/

[1] https://root.cern.ch/downloading-root

Follow the instructions to install ROOT on your system

[2] https://www.scipy.org/scipylib/download.html

Follow the instructions here to install scipy and numpy if thehy are not already included in your python version.

[3] https://www.tensorflow.org/install/

Follow the instructions to install tensorflow for python.  GPU support is not necessary unless you will be training new models.  

### Compiling the top tagger

After all the necessary packages are installed, checkout the TopTagger repository and compile with the following command (with a terminal configured to use root/tensorflow)

```sh
#After downloading the code from github or untaring the standalone tarball 
cd TopTagger/TopTagger/test
./configure
make -j4
#the command below is optional for system wide install (will require sudo rights)
#To install in a non-standard directory add PREFIX=/install/path to the configure command
make install
```

This command will produce a static and a shared library which contain all the necessary symbols to link the top tagger into other C++ code.  This will also compile a standalone example called ``topTaggerTest''.  

## Example code

A basic standalone example using the top tagging code is provided in "TopTagger/test/topTaggerTest.cpp".  This is a basic example program which reads in the necessary top tagger inputs from a file (``exampleInputs.root'') and runs the top tagger code.  As validation, this prints out the number of top quarks reconstructed in each event as well as some basic properties of each top quark reconstructed.  For reference the output of this script can be seen at the end of this readme.  

### Running the example

The example executable is compiled along with the standalone library as described above.  If either openCV or the top tagger was not installed system wide, then there is a script "taggerSetup.sh" which is produced by the configure command which must be sourced to set system variables appropriately.

```
#do once per terminal if opencv or the top tagger are not installed system wide
source taggerSetup.sh
#do once in any directory where you will run the top tagger example
../../Tools/getTaggerCfg.sh -t Intermediate_Example_v1.0.0
#run the example code
./topTaggerTest
``` 

### Tagger input variables

The top tagging algorithm takes both a collection of all AK4 jets and all AK8 jets (passing basic pt and |eta| acceptance requirments) along with supporting variables.  The input variables are listed below

``` c++
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

```

#### AK4JetLV

A vector of TLorentzVectors for each AK4 jet in the event.  This vector should contain all AK4 jets with pt > 30 GeV.  

#### AK4JetBtag 

A vector where each element corrosponds to a binary b-tag value for each jet in the "AK4JetLV" vector.  If the jet is b-tagged the value should be 1.0,  If it is not b-tagged it should be 0.0.  The CSVS ("medium") b-tag working point was used for this determination.  

#### AK8JetLV

A vector of TLorentzVectors for each AK8 jet in the event.  This vector should contain all AK8 jets with pt > 200 GeV.  

#### AK8JetTau1, AK8JetTau2, AK8JetTau3

These three vectors hold the n-subjettiness variables for each jet in "AK8JetLV".  The ratios (tau3/tau2 or tau2/tau1) are calculated inside the top tagger.    

#### AK8JetSoftdropMass

The mass of each AK8 jet in "AK8JetLV" corrected with the soft-drop, using angular exponent beta = 0, soft cutoff threshold z_cut<0.1, and characteristic radius R_0 = 0.8.

#### AK8SubjetLV

A vector of TLorentzVectors containg the CA subjets (two per AK8 jet) produced when calculating the soft-drop mass correction, using angular exponent beta = 0, soft cutoff threshold z_cut<0.1, and characteristic radius R_0 = 0.8.  All subjets are stored in this vector, and they are associated to the approperiate AK8 jet with dR matching.

#### AK8SubjetBtag

The CSVv2 b-tag discriminator matching each entry of AK8SubjetLV.

#### AK8SubjetMult, AK8SubjetPtD, AK8SubjetAxis1, AK8SubjetAxis2

Jet shape variables  matching each entry of AK8SubjetLV .

### Output of example code

```
Event #: 1
      N tops: 1
      Top properties: N constituents:   1,   Pt:  710.0,   Eta:  -0.854,   Phi:  -2.378
          Constituent properties: Constituent type:   3,   Pt:  710.0,   Eta:  -0.854,   Phi:  -2.378
      Rsys properties: N constituents:   1,   Pt:   93.6,   Eta:   2.557,   Phi:   2.295

Event #: 2
      N tops: 1
      Top properties: N constituents:   2,   Pt:  295.5,   Eta:  -1.492,   Phi:  -1.597
          Constituent properties: Constituent type:   3,   Pt:  239.8,   Eta:  -1.468,   Phi:  -1.863
          Constituent properties: Constituent type:   1,   Pt:   89.9,   Eta:  -1.172,   Phi:  -0.819
      Rsys properties: N constituents:   2,   Pt:  139.9,   Eta:  -1.384,   Phi:   2.744

Event #: 3
      N tops: 1
      Top properties: N constituents:   3,   Pt:  398.1,   Eta:   1.120,   Phi:   0.697
          Constituent properties: Constituent type:   1,   Pt:  182.8,   Eta:   1.265,   Phi:   1.017
          Constituent properties: Constituent type:   1,   Pt:  125.9,   Eta:   1.138,   Phi:   0.159
          Constituent properties: Constituent type:   1,   Pt:  116.7,   Eta:   0.574,   Phi:   0.757
      Rsys properties: N constituents:   2,   Pt:  419.4,   Eta:   0.624,   Phi:  -2.227

Event #: 4
      N tops: 0
      Rsys properties: N constituents:   2,   Pt:  373.1,   Eta:   1.016,   Phi:  -0.775

Event #: 5
      N tops: 0
      Rsys properties: N constituents:   2,   Pt:  415.2,   Eta:  -0.204,   Phi:  -0.046

Event #: 6
      N tops: 1
      Top properties: N constituents:   3,   Pt:  331.2,   Eta:  -0.285,   Phi:   0.485
          Constituent properties: Constituent type:   1,   Pt:  183.1,   Eta:  -0.182,   Phi:   0.259
          Constituent properties: Constituent type:   1,   Pt:  137.4,   Eta:  -0.608,   Phi:   0.731
          Constituent properties: Constituent type:   1,   Pt:   20.9,   Eta:   1.066,   Phi:   0.864
      Rsys properties: N constituents:   1,   Pt:   37.2,   Eta:   1.449,   Phi:   0.335

Event #: 7
      N tops: 0
      Rsys properties: N constituents:   2,   Pt:  390.1,   Eta:   0.746,   Phi:   0.766

Event #: 8
      N tops: 1
      Top properties: N constituents:   1,   Pt:  642.3,   Eta:   0.162,   Phi:  -0.800
          Constituent properties: Constituent type:   3,   Pt:  642.3,   Eta:   0.162,   Phi:  -0.800
      Rsys properties: N constituents:   2,   Pt:  358.2,   Eta:   0.934,   Phi:   2.999

Event #: 9
      N tops: 1
      Top properties: N constituents:   1,   Pt:  692.9,   Eta:  -0.629,   Phi:   1.261
          Constituent properties: Constituent type:   3,   Pt:  692.9,   Eta:  -0.629,   Phi:   1.261
      Rsys properties: N constituents:   2,   Pt:  191.8,   Eta:   0.714,   Phi:  -1.667

Event #: 10
      N tops: 2
      Top properties: N constituents:   1,   Pt:  436.9,   Eta:  -0.414,   Phi:   1.431
          Constituent properties: Constituent type:   3,   Pt:  436.9,   Eta:  -0.414,   Phi:   1.431
      Top properties: N constituents:   3,   Pt:  218.4,   Eta:   0.521,   Phi:  -2.894
          Constituent properties: Constituent type:   1,   Pt:  184.1,   Eta:   0.489,   Phi:  -3.023
          Constituent properties: Constituent type:   1,   Pt:   65.0,   Eta:   0.418,   Phi:  -2.207
          Constituent properties: Constituent type:   1,   Pt:   22.6,   Eta:  -0.115,   Phi:   1.129
      Rsys properties: N constituents:   1,   Pt:   39.4,   Eta:   0.290,   Phi:   1.424

Event #: 11
      N tops: 0
      Rsys properties: N constituents:   2,   Pt:  338.2,   Eta:  -0.939,   Phi:  -0.948

Event #: 12
      N tops: 0
      Rsys properties: N constituents:   2,   Pt:  517.9,   Eta:   0.123,   Phi:  -1.041

Event #: 13
      N tops: 0
      Rsys properties: N constituents:   1,   Pt:   62.0,   Eta:  -0.654,   Phi:  -2.417

Event #: 14
      N tops: 1
      Top properties: N constituents:   1,   Pt:  545.4,   Eta:   0.299,   Phi:   0.223
          Constituent properties: Constituent type:   3,   Pt:  545.4,   Eta:   0.299,   Phi:   0.223
      Rsys properties: N constituents:   1,   Pt:   34.4,   Eta:   1.345,   Phi:   0.987

Event #: 15
      N tops: 1
      Top properties: N constituents:   1,   Pt:  654.6,   Eta:   0.561,   Phi:   0.650
          Constituent properties: Constituent type:   3,   Pt:  654.6,   Eta:   0.561,   Phi:   0.650
      Rsys properties: N constituents:   2,   Pt:  190.1,   Eta:   0.847,   Phi:   2.388

Event #: 16
      N tops: 1
      Top properties: N constituents:   1,   Pt:  479.7,   Eta:   0.684,   Phi:  -0.132
          Constituent properties: Constituent type:   3,   Pt:  479.7,   Eta:   0.684,   Phi:  -0.132
      Rsys properties: N constituents:   1,   Pt:   46.8,   Eta:   1.320,   Phi:   0.245

Event #: 17
      N tops: 1
      Top properties: N constituents:   1,   Pt:  623.4,   Eta:   0.526,   Phi:   2.167
          Constituent properties: Constituent type:   3,   Pt:  623.4,   Eta:   0.526,   Phi:   2.167
      Rsys properties: N constituents:   2,   Pt:  431.8,   Eta:  -0.217,   Phi:   2.929

Event #: 18
      N tops: 0
      Rsys properties: N constituents:   2,   Pt:  312.7,   Eta:   0.012,   Phi:  -2.259

Event #: 19
      N tops: 1
      Top properties: N constituents:   3,   Pt:  315.7,   Eta:   1.260,   Phi:  -2.003
          Constituent properties: Constituent type:   1,   Pt:  218.9,   Eta:   1.003,   Phi:  -1.888
          Constituent properties: Constituent type:   1,   Pt:   95.2,   Eta:   1.647,   Phi:  -2.475
          Constituent properties: Constituent type:   1,   Pt:   22.6,   Eta:   0.635,   Phi:  -1.069
      Rsys properties: N constituents:   1,   Pt:  108.6,   Eta:  -0.556,   Phi:   0.464

Event #: 20
      N tops: 1
      Top properties: N constituents:   3,   Pt:  594.5,   Eta:  -0.356,   Phi:   0.608
          Constituent properties: Constituent type:   1,   Pt:  285.8,   Eta:  -0.098,   Phi:   0.365
          Constituent properties: Constituent type:   1,   Pt:  232.5,   Eta:  -0.376,   Phi:   0.740
          Constituent properties: Constituent type:   1,   Pt:   94.8,   Eta:  -0.912,   Phi:   1.025
      Rsys properties: N constituents:   2,   Pt:  383.8,   Eta:   1.953,   Phi:  -1.400

Event #: 21
      N tops: 0
      Rsys properties: N constituents:   2,   Pt:  205.5,   Eta:  -0.632,   Phi:  -1.353

Event #: 22
      N tops: 0
      Rsys properties: N constituents:   2,   Pt:  420.4,   Eta:  -0.951,   Phi:   1.775

Event #: 23
      N tops: 1
      Top properties: N constituents:   1,   Pt:  569.0,   Eta:  -0.673,   Phi:  -0.555
          Constituent properties: Constituent type:   3,   Pt:  569.0,   Eta:  -0.673,   Phi:  -0.555
      Rsys properties: N constituents:   1,   Pt:   58.0,   Eta:  -0.847,   Phi:  -0.153

Event #: 24
      N tops: 1
      Top properties: N constituents:   1,   Pt:  748.5,   Eta:   0.071,   Phi:  -1.185
          Constituent properties: Constituent type:   3,   Pt:  748.5,   Eta:   0.071,   Phi:  -1.185
      Rsys properties: N constituents:   2,   Pt:  139.7,   Eta:   0.122,   Phi:   3.061

Event #: 25
      N tops: 1
      Top properties: N constituents:   3,   Pt:  457.8,   Eta:   0.328,   Phi:   1.719
          Constituent properties: Constituent type:   1,   Pt:  356.6,   Eta:   0.337,   Phi:   1.632
          Constituent properties: Constituent type:   1,   Pt:   57.3,   Eta:   0.822,   Phi:   1.855
          Constituent properties: Constituent type:   1,   Pt:   51.5,   Eta:  -0.422,   Phi:   2.190
      Rsys properties: N constituents:   1,   Pt:   55.7,   Eta:  -1.329,   Phi:   1.579

Event #: 26
      N tops: 1
      Top properties: N constituents:   3,   Pt:  254.0,   Eta:   0.264,   Phi:  -1.344
          Constituent properties: Constituent type:   1,   Pt:  225.5,   Eta:   0.293,   Phi:  -1.379
          Constituent properties: Constituent type:   1,   Pt:   48.8,   Eta:  -0.289,   Phi:  -1.865
          Constituent properties: Constituent type:   1,   Pt:   34.9,   Eta:   0.423,   Phi:   0.629
      Rsys properties: N constituents:   2,   Pt:  692.3,   Eta:   0.862,   Phi:  -2.357

Event #: 27
      N tops: 0
      Rsys properties: N constituents:   2,   Pt:  375.7,   Eta:  -0.123,   Phi:   1.623

Event #: 28
      N tops: 0
      Rsys properties: N constituents:   2,   Pt:  873.8,   Eta:   0.083,   Phi:   2.169

Event #: 29
      N tops: 1
      Top properties: N constituents:   3,   Pt:  413.3,   Eta:  -0.096,   Phi:   0.310
          Constituent properties: Constituent type:   1,   Pt:  309.4,   Eta:  -0.178,   Phi:   0.311
          Constituent properties: Constituent type:   1,   Pt:   82.6,   Eta:   0.037,   Phi:  -0.110
          Constituent properties: Constituent type:   1,   Pt:   43.9,   Eta:   0.279,   Phi:   1.176
      Rsys properties: N constituents:   2,   Pt:  448.4,   Eta:   0.880,   Phi:  -1.861

Event #: 30
      N tops: 2
      Top properties: N constituents:   1,   Pt:  463.2,   Eta:   0.731,   Phi:  -2.693
          Constituent properties: Constituent type:   3,   Pt:  463.2,   Eta:   0.731,   Phi:  -2.693
      Top properties: N constituents:   2,   Pt:  438.3,   Eta:   0.100,   Phi:  -1.179
          Constituent properties: Constituent type:   3,   Pt:  392.5,   Eta:   0.170,   Phi:  -1.111
          Constituent properties: Constituent type:   1,   Pt:   53.6,   Eta:  -0.420,   Phi:  -1.693
      Rsys properties: N constituents:   2,   Pt:  334.2,   Eta:   0.917,   Phi:   1.175

Event #: 31
      N tops: 1
      Top properties: N constituents:   3,   Pt:  512.3,   Eta:  -0.403,   Phi:  -2.865
          Constituent properties: Constituent type:   1,   Pt:  351.5,   Eta:  -0.336,   Phi:  -2.966
          Constituent properties: Constituent type:   1,   Pt:  119.9,   Eta:  -0.712,   Phi:  -2.655
          Constituent properties: Constituent type:   1,   Pt:   46.6,   Eta:   0.022,   Phi:  -2.639
      Rsys properties: N constituents:   2,   Pt:  261.2,   Eta:   0.003,   Phi:   2.125

Event #: 32
      N tops: 0
      Rsys properties: N constituents:   2,   Pt:  679.6,   Eta:  -0.108,   Phi:  -2.517

Event #: 33
      N tops: 1
      Top properties: N constituents:   3,   Pt:  762.3,   Eta:  -0.777,   Phi:  -1.122
          Constituent properties: Constituent type:   1,   Pt:  665.4,   Eta:  -0.714,   Phi:  -1.132
          Constituent properties: Constituent type:   1,   Pt:   70.1,   Eta:  -1.391,   Phi:  -1.099
          Constituent properties: Constituent type:   1,   Pt:   27.4,   Eta:  -0.186,   Phi:  -0.929
      Rsys properties: N constituents:   1,   Pt:   33.8,   Eta:   2.213,   Phi:   0.506

Event #: 34
      N tops: 0
      Rsys properties: N constituents:   2,   Pt:  222.0,   Eta:  -0.786,   Phi:  -3.057

Event #: 35
      N tops: 1
      Top properties: N constituents:   3,   Pt:  335.5,   Eta:  -0.205,   Phi:   0.284
          Constituent properties: Constituent type:   1,   Pt:  251.5,   Eta:  -0.280,   Phi:   0.410
          Constituent properties: Constituent type:   1,   Pt:   93.6,   Eta:  -0.017,   Phi:  -0.396
          Constituent properties: Constituent type:   1,   Pt:   30.5,   Eta:   0.115,   Phi:   1.404
      Rsys properties: N constituents:   2,   Pt:  412.7,   Eta:   0.880,   Phi:  -0.790

Event #: 36
      N tops: 2
      Top properties: N constituents:   1,   Pt:  507.5,   Eta:  -1.194,   Phi:   0.051
          Constituent properties: Constituent type:   3,   Pt:  507.5,   Eta:  -1.194,   Phi:   0.051
      Top properties: N constituents:   3,   Pt:  272.5,   Eta:   1.877,   Phi:   1.195
          Constituent properties: Constituent type:   1,   Pt:  129.1,   Eta:   2.131,   Phi:   0.973
          Constituent properties: Constituent type:   1,   Pt:  123.8,   Eta:   1.642,   Phi:   1.521
          Constituent properties: Constituent type:   1,   Pt:   31.3,   Eta:   0.748,   Phi:   0.832
      Rsys properties: N constituents:   2,   Pt:  110.7,   Eta:   0.951,   Phi:   2.227

Event #: 37
      N tops: 1
      Top properties: N constituents:   3,   Pt:  494.3,   Eta:   1.019,   Phi:  -2.

Event #: 38
      N tops: 0
      Rsys properties: N constituents:   2,   Pt:  455.1,   Eta:   1.811,   Phi:  -1.054

Event #: 39
      N tops: 0
      Rsys properties: N constituents:   2,   Pt:  351.8,   Eta:   0.072,   Phi:  -0.199

Event #: 40
      N tops: 0
      Rsys properties: N constituents:   2,   Pt:  425.0,   Eta:   0.540,   Phi:   2.347

Event #: 41
      N tops: 2
      Top properties: N constituents:   1,   Pt:  421.5,   Eta:  -0.392,   Phi:  -1.729
          Constituent properties: Constituent type:   3,   Pt:  421.5,   Eta:  -0.392,   Phi:  -1.729
      Top properties: N constituents:   3,   Pt:   34.3,   Eta:  -0.510,   Phi:  -1.721
          Constituent properties: Constituent type:   1,   Pt:   52.3,   Eta:  -0.501,   Phi:  -0.677
          Constituent properties: Constituent type:   1,   Pt:   49.5,   Eta:   0.505,   Phi:  -2.651
          Constituent properties: Constituent type:   1,   Pt:   22.3,   Eta:  -0.705,   Phi:   1.668
      Rsys properties: N constituents:   2,   Pt:  667.0,   Eta:  -0.273,   Phi:   0.541

Event #: 42
      N tops: 1
      Top properties: N constituents:   1,   Pt:  658.7,   Eta:   0.198,   Phi:  -2.461
          Constituent properties: Constituent type:   3,   Pt:  658.7,   Eta:   0.198,   Phi:  -2.461
      Rsys properties: N constituents:   1,   Pt:  195.3,   Eta:  -0.094,   Phi:   0.006

Event #: 43
      N tops: 2
      Top properties: N constituents:   1,   Pt:  688.9,   Eta:   1.042,   Phi:  -0.145
          Constituent properties: Constituent type:   3,   Pt:  688.9,   Eta:   1.042,   Phi:  -0.145
      Top properties: N constituents:   1,   Pt:  516.0,   Eta:   0.911,   Phi:   3.000
          Constituent properties: Constituent type:   3,   Pt:  516.0,   Eta:   0.911,   Phi:   3.000
      Rsys properties: N constituents:   1,   Pt:   57.1,   Eta:   0.302,   Phi:   2.767

Event #: 44
      N tops: 0
      Rsys properties: N constituents:   2,   Pt:  190.1,   Eta:  -1.062,   Phi:  -1.452

Event #: 45
      N tops: 1
      Top properties: N constituents:   3,   Pt:  414.3,   Eta:   1.099,   Phi:   3.077
          Constituent properties: Constituent type:   1,   Pt:  272.2,   Eta:   1.056,   Phi:   2.992
          Constituent properties: Constituent type:   1,   Pt:   97.1,   Eta:   0.863,   Phi:  -2.487
          Constituent properties: Constituent type:   1,   Pt:   81.1,   Eta:   1.143,   Phi:   2.550
      Rsys properties: N constituents:   1,   Pt:   90.0,   Eta:  -0.251,   Phi:   1.098

Event #: 46
      N tops: 0
      Rsys properties: N constituents:   2,   Pt:  308.8,   Eta:   0.109,   Phi:   0.541

Event #: 47
      N tops: 1
      Top properties: N constituents:   3,   Pt:  159.8,   Eta:   0.882,   Phi:   0.914
          Constituent properties: Constituent type:   1,   Pt:   95.9,   Eta:   0.096,   Phi:   0.777
          Constituent properties: Constituent type:   1,   Pt:   79.3,   Eta:   0.826,   Phi:   1.335
          Constituent properties: Constituent type:   1,   Pt:   20.6,   Eta:   2.034,   Phi:  -1.030
      Rsys properties: N constituents:   2,   Pt:  399.0,   Eta:   0.322,   Phi:   3.001

Event #: 48
      N tops: 1
      Top properties: N constituents:   1,   Pt:  589.6,   Eta:   0.518,   Phi:  -0.688
          Constituent properties: Constituent type:   3,   Pt:  589.6,   Eta:   0.518,   Phi:  -0.688
      Rsys properties: N constituents:   1,   Pt:   50.6,   Eta:   1.840,   Phi:  -0.368

Event #: 49
      N tops: 2
      Top properties: N constituents:   2,   Pt:  476.3,   Eta:   0.447,   Phi:   1.790
          Constituent properties: Constituent type:   3,   Pt:  435.1,   Eta:   0.416,   Phi:   1.852
          Constituent properties: Constituent type:   1,   Pt:   50.0,   Eta:   0.632,   Phi:   1.221
      Top properties: N constituents:   3,   Pt:   60.8,   Eta:   1.428,   Phi:   0.223
          Constituent properties: Constituent type:   1,   Pt:   56.5,   Eta:   0.948,   Phi:   0.299
          Constituent properties: Constituent type:   1,   Pt:   41.2,   Eta:  -0.314,   Phi:  -0.454
          Constituent properties: Constituent type:   1,   Pt:   35.0,   Eta:   1.452,   Phi:   2.705
      Rsys properties: N constituents:   1,   Pt:  144.2,   Eta:  -1.108,   Phi:  -1.851

```

