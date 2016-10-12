We use scikit package for MVA based tagger. Its python based.

I. Samples for training and validation

Root files cantaining only relavant variables.
makeTrainingTuples.C produces this root file;

Run the following commands

make 

./makeTrainingTuples -D TTbarSingleLep -E 10000 -R 1:1

You can change the sample name, #events and ratio (training sample to validation sample)

Rename the trainingTuple_division_0_TTbarSingleLep.root sample with an extra phase '_training'


Rename the trainingTuple_division_1_TTbarSingleLep.root sample with an extra	phase '_validation'

II. Running mva code

There are two python files: 

Training.py for training the mva

Validation.py to validate the mva training result

Run the following command

python Training.py

It will create a ouput file with training output (with .pkl extension). Then run the following

python Validation.py

It will create plots of Efficincy, Fakerate, Purity, input variables and also roc curve in png format. It will also creates a root file containing those distributions in histogram format.




