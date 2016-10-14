# Tagger development tools

We use scikit package for MVA based tagger. Its python based.

## Samples for training and validation

Slimmed tuples containing only the information needed for training and validation can be made with "makeTrainingTuples.C".  The training and validation currently require input from the semileptonic ttbar and znunu smaples.  The instructions for creating these files are found below.

Run the following commands

```
make 
./makeTrainingTuples -D TTbarSingleLep -E 10000 -R 1:1
./makeTrainingTuples -D ZJetsToNuNu -E 10000 -R 1:1
```

You can change the input sample name with -D, #events with -E and ratio (training sample to validation sample) with -R.  Additional sample splits can be added with -R (i.e. -R 2:2:1 will create 3 sample files where the first 2 have twice the numbe rof events as the 3rd)

## Running sklearn mva code

There are two python files: 

Training.py for training the mva

Validation.py to validate the mva training result

Run the following command

```
python Training.py
```

It will create a ouput file with training output (with .pkl extension). Then run the following

```
python Validation.py
```

It will create plots of Efficincy, Fakerate, Purity, input variables and also roc curve in png format. It will also creates a root file containing those distributions in histogram format.

## OpenCV Instructions 

OpenCV provides a C++ interface to a random forest algorithm which can be integrated into our normal c++ workflow

### Installing openCV

In order to use sample weights we need the latest release of openCV 3.1.0.  You may either download the sources as a zip or downlod the repository from github. Downloading the zip will use less disk space.

The tarball may be downloaded from 

```
http://opencv.org/downloads.html
```

Ensure you download the version for 3.1.0

Unzip the archive in your "CMSSW_X_Y_Z/src" folder and rename the folder as follows

```
unzip opencv-3.1.0.zip
rm opencv-3.1.0.zip
mv opencv-3.1.0.zip opencv
cd opencv
```

rename the folder from "opencv-3.1.0" to "opencv"

To install from github do the following in your "CMSSW_X_Y_Z/src"

```
git clone git@github.com:opencv/opencv.git
cd opencv
git checkout 3.1.0
```

To compile openCV run the following commands in the "opencv" folder

```
cmake .
make -j4
```

### Running openCV test code

The openCV test code can be found in "TopTagger/Tools".  This code uses the same input tuples as the sklearn implementation.  The code can be compiled by running "make" in the "TopTagger/Tools" folder and this produes the executable "openCVTest".  To run the code the necessary library locations for opencv must be added to the LD_LIBRARY_PATH variable.  This will be done automatically by running the following

```
source opencvSetup.sh
```

The code may then be run with the command 

```
./openCVTest
```