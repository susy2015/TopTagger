We use scikit package for MVA based tagger. Its python based.

I. Samples for training and validation

Root files cantaining only relavant variables.
makeTrainingTuples.C produces this root file;

Run the following commands

make 

./makeTrainingTuples -D TTbarSingleLep -E 10000 -R 1:1

You can change the sample name, #events and ratio (training sample to validation sample)

For sample to be used for fakerate calculation, Run the following command

./makeTrainingTuples -f -D ZJetsToNuNu -E 10000 -R 1

'f' option enable slection cut (met, Njet and Nbjet) for fakerate sample

II. Running mva code

Its a python file: sklearnTest.py, Lates version is sklearnTest_v2.py (later it will be merged in one)

Run the following command

python sklearnTest_v2.py

It will create plots of Efficincy, Fakerate, Purity, input variables and also roc curve in png format. It will also creates a root file containing those distributions in histogram format.

Caveat: in sklearnTest_v2.py, input variables are not filling properly. It will be fixed soon. But sklearnTest.py does not have this issue.

