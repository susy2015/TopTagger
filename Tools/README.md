# Tagger development tools

We use Tensorflow package for MVA based tagger studies. The training/validation is python based.  The model is integrated into the c++ side of the framework via the Tensorflow c-api or through a call to python if the Tensorflow c-api is inavaliable.

## Samples for training and validation

Slimmed tuples containing only the information needed for training and validation can be made with "cpp/makeTrainingTuples.C".  The training and validation currently require input from several samples including semileptonic ttbar, QCD, and znunu.  The instructions for creating these files are found below.

Run the following commands (at the LPC)

~~~~~~~~~~~~~
make 
./makeTrainingTuples -D TTbarSingleLep -E 550000 -R 10:1
./makeTrainingTuples -D ZJetsToNuNu -E 200000 -R 1:1
~~~~~~~~~~~~~

You can change the input sample name with -D, #events with -E and ratio (training sample to validation sample) with -R.  Additional sample splits can be added with -R (i.e. -R 2:2:1 will create 3 sample files where the first 2 have twice the number of events as the 3rd).  To produce the training files for an entire dataset a condor submit script is provided in condor/condorSubmit.py.  This code requires the repository "susy2015/SusyAnaTools" to be checked out in the same directory as the TopTagger repository to compile and run.  


## Running ANN training code 

The following packages must be installed on your system if you wish to train a model with tensorflow

### Required packages 

The following packages much be installed to run the ANN training and validation code 

#### python 2.7

https://www.python.org/

#### virtualenv 

This package is not manditory, but is highly recomended to isolate all the packages you will install from your main python installation 

https://virtualenv.pypa.io/en/stable/

To install other packages in the virtual environment first source the activate script for the virtual environment of interest and then install the following packages via pip.

#### scipy & numpy

scipy and numpy are used for efficient manipulation of large datasets in python and are prerequisutes for most other packages required 

https://www.scipy.org/install.html

https://www.scipy.org/install.html

#### pandas

Pandas is a package which is designed to make manipulation of datasets in python more convinenent

http://pandas.pydata.org/pandas-docs/stable/

#### scikit-learn

Scikit learn is a python package which implements a wide array of easy to use MVA techniques

http://scikit-learn.org/stable/install.html

#### tensorflow

Tensorflow is the machine learning package developed by Google for implementing and training a wide range of advanced neural networks.  

https://www.tensorflow.org/install/

#### HDF5

HDF5 is a dataformat which we use to hold the training and validation format.  This package is a pain to install.  Unless you have a redhat based linux intall it must be installed from source

https://support.hdfgroup.org/HDF5/release/obtainsrc518.html

Then the python interface must be installed seperately 

http://docs.h5py.org/en/latest/build.html

When I installed the main HD5 package from source I found it named the libraries in a way that the python library couldn't find.  I was able to workaround this by creating the following softlinks

libhdf5.so -> libhdf5-shared.so
libhdf5_hl.so -> libhdf5_hl-shared.so

#### ROOT and PyROOT

https://root.cern.ch/downloading-root

### Training

The script "Training.py" is used to train produce and train the MVA algorithm.  The script is primarily intended to train neural networks with tensorflow, but it also is able to train random forests with scikit learn and boosted decision trees with the extreme gradient boost package.  To run a basic training using the kodiak gpu machines (gpu001 or gpu002) with the default varialbes the following commands can be run.

~~~~~~~~~~~~~
cd TopTagger/Tools/python
source source ~hatake/tensorflow/setup.sh
python Training.py -d DIRECTORY
~~~~~~~~~~~~~

This will train a tensorflow model with the default settings.  Note that in the case of a neural network training this can take many hours.  To do a "quick" (~15 to 30 min) test use the "-e 2" option to limit yourself to only two training epochs.  The results will appear in "DIRECTORY".  The immediate validation can be viewed during or after training by launching tensorboard as follows

~~~~~~~~~~~~~
tensorboard --logdir DIRECTORY
~~~~~~~~~~~~~

This can then be viewed by tuneling your browser to the machine where tensorboard is running and navigating to "localhost:6006".  You can also copy the directory "DIRECTORY" to your local machine and run the same tensorboard command to view this information on your local machine.  

Training parameters defaulta can be seen and modified by looking in the file "TopTagger/Tools/python/taggerOptions.py".  

### Validation 

The script "Validation.py" produces a series of validation plots including plots of all MVA input variables, the output discriminator, purity, fake rate, efficiency, and ROC curves.  It also produces a version of the ROC curve in a pkl file for reploting.  The validation code will look for the trained network file in the directory specified with the -d option, so this should be the same directory name given to the training script.  Additionally, the output files will b ewritten to this directoty.  

~~~~~~~~~~~~~
python Validation.py -d DIRECTORY
~~~~~~~~~~~~~

No other options are necessary as all settings needed are read from the configuration file written in DIRECTORY by the training script.  

### Combined ROC curve plots

The script "rocPlots.py" is provided to help many compairson ROC plots to draw multiple ROC curves on the same axis.  

