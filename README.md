# TopTagger


## Standalone installation instructions

### Installing prerequisites 

To compile the standalone library requires the ROOT 6 [1] and OpenCV 3.1 (or newer) [2] packages.  

[1] https://root.cern.ch/downloading-root

Follow the instructions to install ROOT on your system

[2] http://opencv.org/releases.html

Download the source tarball and unpack (or check the code out from github) and run the following inside the opencv source folder

```
cmake .
make -j4
#optional for system wide install (will require sudo rights)
make install
```

### Compiling the top tagger

After ROOT and OpenCV are installed, checkout the TopTagger repository and compile with the following command (with a terminal configured to use root)

```
git clone -b standaloneTesting git@github.com:susy2015/TopTagger.git
#Alternately the code may be downloaded as a tarball here INSERTLINK
cd TopTagger/TopTagger/test
./configure OPENCVDIR=/path/to/opencv
make -j4
#optional for system wide install (will require sudo rights)
#To install in a non-standard directory add PREFIX=/install/path to the configure command
make install
```

This command will produce a static and a shared library which contain all the necessary symbols to link the top tagger into other C++ code.  This will also compile a standalone example called ``topTaggerTest''.  

## Example code

A basic standalone example using the top tagging code is provided in "TopTagger/test/topTaggerTest.cpp".  This is a basic example program which reads in the necessary top tagger inputs from a file (``exampleInputs.root'') and runs the top tagger code.  As validation, this prints out the number of top quarks reconstructed in each event as well as some basic properties of each top quark reconstructed.  For reference the output of this script can be seen at the end of this readme.  

### Tagger input variables

The top tagging algorithm takes both a collection of all AK4 jets and all AK8 jets (passing basic pt and |eta| acceptance requirments) along with supporting variables.  The input variables are listed below

``` c++
//Variables to hold inputs
//AK4 jet variables
//Each entry in these vectors revers to information for 1 AK4 jet
std::vector<TLorentzVector>** AK4JetLV = new std::vector<TLorentzVector>*();
std::vector<double>** AK4JetBtag = new std::vector<double>*();

//AK8 jet varaibles
//The elements of each vector refer to one AK8 jet
std::vector<TLorentzVector>** AK8JetLV = new std::vector<TLorentzVector>*();
std::vector<TLorentzVector>** AK8SubjetLV = new std::vector<TLorentzVector>*();
std::vector<double>** AK8JetTau1 = new std::vector<double>*();
std::vector<double>** AK8JetTau2 = new std::vector<double>*();
std::vector<double>** AK8JetTau3 = new std::vector<double>*();
std::vector<double>** AK8JetSoftdropMass = new std::vector<double>*();

```

#### AK4JetLV

A vector of TLorentzVectors for each AK4 jet in the event.  This vector should contain all AK4 jets with pt > 30 GeV.  

#### AK4JetBtag 

A vector where each element corrosponds to a binary b-tag value for each jet in the "AK4JetLV" vector.  If the jet is b-tagged the value should be 1.0,  If it is not b-tagged it should be 0.0.  The CSVS ("medium") b-tag working point was used for this determination.  

#### AK8JetLV

A vector of TLorentzVectors for each AK8 jet in the event.  This vector should contain all AK8 jets with pt > 200 GeV.  

#### AK8SubjetLV

A vector of TLorentzVectors containg teh CA subjets (two per AK8 jet) produced when calculating the soft-drop mass correction, using angular exponent $\beta = 0$, soft cutoff threshold $z_{\mathrm{cut}}<0.1$, and characteristic radius $R_{0} = 0.8$.  All subjets are stored in this vector, and they are associated to the approperiate AK8 jet with dR matching.

#### AK8JetTau1, AK8JetTau2, AK8JetTau3

These three vectors hold the n-subjettiness variables for each jet in "AK8JetLV".  The ratios are calculated inside the top tagger.    

#### AK8JetSoftdropMass

The mass of each AK8 jet in "AK8JetLV" corrected with the soft-drop, using angular exponent $\beta = 0$, soft cutoff threshold $z_{\mathrm{cut}}<0.1$, and characteristic radius $R_{0} = 0.8$.


### Running the example

The example executable is compiled along with the standalone library as described above.  If either openCV or the top tagger was not installed system wide, then there is a script "taggerSetup.sh" which is produced by the configure command which must be sourced to set system variables appropriately.

```
#do once per terminal if opencv or the top tagger are not installed system wide
source taggerSetup.sh
#do once in any directory where you will run the top tagger example
../../Tools/getTaggerCfg.sh -t MVAAK8_Tight_noQGL_binaryCSV_v1.0.2
#run the example code
./topTaggerTest
``` 

### Output of example code

```
Event #: 1
      N tops: 0
      Rsys properties: N constituents:   2,   Pt:  150.7,   Eta:  -0.668,   Phi:   1.647

Event #: 2
      N tops: 1
      Top properties: N constituents:   2,   Pt:  432.9,   Eta:  -0.272,   Phi:   2.470
          Constituent properties: Constituent type:   3,   Pt:  392.2,   Eta:  -0.203,   Phi:   2.350
          Constituent properties: Constituent type:   1,   Pt:   63.9,   Eta:  -0.577,   Phi:  -2.992
      Rsys properties: N constituents:   1,   Pt:   35.0,   Eta:  -0.440,   Phi:   1.154

Event #: 3
      N tops: 0
      Rsys properties: N constituents:   2,   Pt:  680.1,   Eta:  -0.854,   Phi:  -0.440

Event #: 4
      N tops: 1
      Top properties: N constituents:   2,   Pt:  272.1,   Eta:   0.753,   Phi:   1.779
          Constituent properties: Constituent type:   3,   Pt:  222.3,   Eta:   0.894,   Phi:   1.906
          Constituent properties: Constituent type:   1,   Pt:   58.8,   Eta:  -0.026,   Phi:   1.278
      Rsys properties: N constituents:   2,   Pt:  315.2,   Eta:   0.413,   Phi:   0.252

Event #: 5
      N tops: 0
      Rsys properties: N constituents:   2,   Pt:  431.9,   Eta:   0.394,   Phi:  -1.831

Event #: 6
      N tops: 1
      Top properties: N constituents:   3,   Pt:  263.3,   Eta:   0.794,   Phi:   0.973
          Constituent properties: Constituent type:   1,   Pt:  170.3,   Eta:   0.837,   Phi:   1.376
          Constituent properties: Constituent type:   1,   Pt:  101.7,   Eta:   0.713,   Phi:   0.224
          Constituent properties: Constituent type:   1,   Pt:   32.3,   Eta:  -0.213,   Phi:   1.050
      Rsys properties: N constituents:   2,   Pt:  178.3,   Eta:  -0.254,   Phi:  -2.190

Event #: 7
      N tops: 1
      Top properties: N constituents:   3,   Pt:  304.1,   Eta:   1.307,   Phi:   2.279
          Constituent properties: Constituent type:   1,   Pt:  236.9,   Eta:   1.108,   Phi:   2.299
          Constituent properties: Constituent type:   1,   Pt:   44.2,   Eta:   2.120,   Phi:   2.638
          Constituent properties: Constituent type:   1,   Pt:   32.9,   Eta:   0.581,   Phi:   1.615
      Rsys properties: N constituents:   2,   Pt:  409.8,   Eta:   0.027,   Phi:  -0.537

Event #: 8
      N tops: 1
      Top properties: N constituents:   3,   Pt:  287.8,   Eta:  -0.986,   Phi:  -0.040
          Constituent properties: Constituent type:   1,   Pt:  180.6,   Eta:  -1.159,   Phi:  -0.052
          Constituent properties: Constituent type:   1,   Pt:   89.2,   Eta:  -0.561,   Phi:  -0.407
          Constituent properties: Constituent type:   1,   Pt:   41.7,   Eta:  -0.466,   Phi:   0.919
      Rsys properties: N constituents:   2,   Pt:  365.5,   Eta:  -1.888,   Phi:  -1.051

Event #: 9
      N tops: 0
      Rsys properties: N constituents:   2,   Pt:  433.1,   Eta:   0.221,   Phi:  -2.764

Event #: 10
      N tops: 1
      Top properties: N constituents:   1,   Pt:  595.5,   Eta:  -1.099,   Phi:  -2.877
          Constituent properties: Constituent type:   3,   Pt:  595.5,   Eta:  -1.099,   Phi:  -2.877
          Rsys properties: N constituents:   2,   Pt:  208.1,   Eta:   1.442,   Phi:   0.773

Event #: 11
      N tops: 0
      Rsys properties: N constituents:   2,   Pt:  566.9,   Eta:  -1.533,   Phi:   1.283

Event #: 12
      N tops: 0
      Rsys properties: N constituents:   2,   Pt:  835.7,   Eta:   0.842,   Phi:  -2.453

Event #: 13
      N tops: 2
      Top properties: N constituents:   1,   Pt:  842.7,   Eta:   1.076,   Phi:  -1.628
          Constituent properties: Constituent type:   3,   Pt:  842.7,   Eta:   1.076,   Phi:  -1.628
      Top properties: N constituents:   1,   Pt:  500.0,   Eta:   0.036,   Phi:   2.106
          Constituent properties: Constituent type:   3,   Pt:  500.0,   Eta:   0.036,   Phi:   2.106
      Rsys properties: N constituents:   2,   Pt:  199.9,   Eta:  -0.265,   Phi:   2.219

Event #: 14
      N tops: 0
      Rsys properties: N constituents:   2,   Pt:  595.3,   Eta:   0.750,   Phi:  -1.061

Event #: 15
      N tops: 1
      Top properties: N constituents:   3,   Pt:  270.5,   Eta:   1.075,   Phi:  -2.798
          Constituent properties: Constituent type:   1,   Pt:  198.3,   Eta:   0.922,   Phi:  -2.839
          Constituent properties: Constituent type:   1,   Pt:   65.8,   Eta:   1.397,   Phi:   2.961
          Constituent properties: Constituent type:   1,   Pt:   43.8,   Eta:   0.343,   Phi:  -1.584
      Rsys properties: N constituents:   2,   Pt:  304.7,   Eta:   0.156,   Phi:   1.485

Event #: 16
      N tops: 1
      Top properties: N constituents:   2,   Pt:  328.4,   Eta:   0.448,   Phi:   1.430
          Constituent properties: Constituent type:   3,   Pt:  274.4,   Eta:   0.552,   Phi:   1.261
          Constituent properties: Constituent type:   1,   Pt:   74.0,   Eta:  -0.094,   Phi:   2.101
      Rsys properties: N constituents:   2,   Pt:  118.4,   Eta:  -2.274,   Phi:  -0.948

Event #: 17
      N tops: 0
      Rsys properties: N constituents:   2,   Pt:  174.1,   Eta:  -1.441,   Phi:  -0.529

Event #: 18
      N tops: 0
      Rsys properties: N constituents:   1,   Pt:  202.0,   Eta:  -0.160,   Phi:  -2.976

Event #: 19
      N tops: 1
      Top properties: N constituents:   1,   Pt:  481.5,   Eta:   0.801,   Phi:  -1.754
          Constituent properties: Constituent type:   3,   Pt:  481.5,   Eta:   0.801,   Phi:  -1.754
          Rsys properties: N constituents:   1,   Pt:  640.5,   Eta:   0.126,   Phi:   1.530

Event #: 20
      N tops: 0
      Rsys properties: N constituents:   2,   Pt:  189.3,   Eta:  -0.715,   Phi:   1.101

Event #: 21
      N tops: 2
      Top properties: N constituents:   1,   Pt:  766.0,   Eta:   0.976,   Phi:   2.027
          Constituent properties: Constituent type:   3,   Pt:  766.0,   Eta:   0.976,   Phi:   2.027
          Top properties: N constituents:   3,   Pt:  276.6,   Eta:  -1.545,   Phi:  -3.123
              Constituent properties: Constituent type:   1,   Pt:  131.3,   Eta:  -1.386,   Phi:  -3.016
              Constituent properties: Constituent type:   1,   Pt:   87.8,   Eta:  -2.097,   Phi:   3.081
              Constituent properties: Constituent type:   1,   Pt:   59.0,   Eta:  -0.348,   Phi:   3.041
          Rsys properties: N constituents:   1,   Pt:   91.8,   Eta:   1.267,   Phi:   1.741

Event #: 22
      N tops: 0
      Rsys properties: N constituents:   2,   Pt:  302.5,   Eta:   1.296,   Phi:   1.864

Event #: 23
      N tops: 1
      Top properties: N constituents:   1,   Pt:  400.8,   Eta:  -0.146,   Phi:   0.975
          Constituent properties: Constituent type:   3,   Pt:  400.8,   Eta:  -0.146,   Phi:   0.975
      Rsys properties: N constituents:   2,   Pt:  225.2,   Eta:   0.035,   Phi:   0.980

Event #: 24
      N tops: 1
      Top properties: N constituents:   1,   Pt:  491.4,   Eta:  -0.034,   Phi:  -1.783
          Constituent properties: Constituent type:   3,   Pt:  491.4,   Eta:  -0.034,   Phi:  -1.783
      Rsys properties: N constituents:   1,   Pt:  588.5,   Eta:  -1.095,   Phi:   2.499

Event #: 25
      N tops: 0
      Rsys properties: N constituents:   2,   Pt:  200.5,   Eta:   2.130,   Phi:   2.462

Event #: 26
      N tops: 0
      Rsys properties: N constituents:   2,   Pt:  398.2,   Eta:   0.707,   Phi:  -2.205

Event #: 27
      N tops: 1
      Top properties: N constituents:   3,   Pt:  211.7,   Eta:   1.589,   Phi:  -2.508
          Constituent properties: Constituent type:   1,   Pt:   98.3,   Eta:   1.918,   Phi:  -2.720
          Constituent properties: Constituent type:   1,   Pt:   94.3,   Eta:   1.034,   Phi:  -1.840
          Constituent properties: Constituent type:   1,   Pt:   56.1,   Eta:   0.847,   Phi:   3.039
      Rsys properties: N constituents:   2,   Pt:  253.0,   Eta:  -0.113,   Phi:   1.831

Event #: 28
      N tops: 0
      Rsys properties: N constituents:   1,   Pt:  248.5,   Eta:  -0.485,   Phi:  -1.806

Event #: 29
      N tops: 0
      Rsys properties: N constituents:   1,   Pt:  145.2,   Eta:  -0.989,   Phi:  -1.624

```

