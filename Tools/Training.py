import sys
import ROOT
import numpy
import math
try:
    from sklearn.ensemble import RandomForestClassifier
    from sklearn.ensemble import RandomForestRegressor
    import sklearn.tree as tree
    from sklearn.tree import DecisionTreeClassifier
    from sklearn.tree import DecisionTreeRegressor
    from sklearn import svm
    from sklearn.ensemble import AdaBoostRegressor
    from sklearn.ensemble import GradientBoostingClassifier
    from sklearn.ensemble import GradientBoostingRegressor
    from sklearn.cross_validation import KFold
    from sklearn.neural_network import MLPClassifier 
    from sklearn.preprocessing import MinMaxScaler
except ImportError:
    print "SK-learn not found, hope you don't need it"
import pickle
from MVAcommon import *
try:
    import cv2
except ImportError:
    sys.path.append("../../opencv/lib/")
    import cv2
import optparse
from math import sqrt

parser = optparse.OptionParser("usage: %prog [options]\n")

parser.add_option ('-o', "--opencv", dest='opencv', action='store_true', help="Run using opencv RTrees")
parser.add_option ('-n', "--noRoc", dest='noROC', action='store_true', help="Do not calculate ROC to save time")

options, args = parser.parse_args()

dg = DataGetter()

print "PROCESSING TRAINING DATA"

samplesToRun = ["trainingTuple_division_0_TTbarSingleLep_training.root", "trainingTuple_division_0_ZJetsToNuNu_training.root"]


inputData = []
inputAnswer = []
inputWgts = []
inputSampleWgts = []

for datasetName in samplesToRun:
    dataset = ROOT.TFile.Open(datasetName)
    print datasetName

    hPtMatch   = ROOT.TH1D("hPtMatch" + datasetName, "hPtMatch", 50, 0.0, 2000.0)
    hPtMatch.Sumw2()
    hPtNoMatch = ROOT.TH1D("hPtNoMatch" + datasetName, "hPtNoMatch", 50, 0.0, 2000.0)
    hPtNoMatch.Sumw2()

    #Nevts = 0
    #for event in dataset.slimmedTuple:
    #    if Nevts >= NEVTS:
    #        break
    #    Nevts +=1
    #    for i in xrange(len(event.genConstiuentMatchesVec)):
    #        nmatch = event.genConstiuentMatchesVec[i]
    #        if (int(nmatch == 3) and event.genTopMatchesVec[i]) or (int(nmatch == 0) and not event.genTopMatchesVec[i]):
    #            if nmatch == 3 and event.genTopMatchesVec[i]:
    #                #            if event.genTopMatchesVec[i]:
    #                hPtMatch.Fill(event.cand_pt[i], event.sampleWgt)
    #            else:
    #                hPtNoMatch.Fill(event.cand_pt[i], event.sampleWgt)
    
    Nevts = 0
    count = 0
    for event in dataset.slimmedTuple:
        if Nevts >= NEVTS:
            break
        Nevts +=1
        for i in xrange(len(event.cand_m)):
            nmatch = event.genConstiuentMatchesVec[i]
            if (int(nmatch == 3) and event.genTopMatchesVec[i]) or (int(nmatch == 0) and not event.genTopMatchesVec[i]):
                if (int(nmatch == 0) and not event.genTopMatchesVec[i]):
                    count += 1
                    if count%10:
                        continue
                inputData.append(dg.getData(event, i))
                answer = nmatch == 3 and event.genTopMatchesVec[i]
                inputAnswer.append(answer)
                inputWgts.append(event.sampleWgt)
                inputSampleWgts.append(event.sampleWgt)
#                if int(nmatch == 3) and event.genTopMatchesVec[i]:
#                    if hPtMatch.GetBinContent(hPtMatch.FindBin(event.cand_pt[i])) > 10:
#                        inputWgts.append(1.0 / hPtMatch.GetBinContent(hPtMatch.FindBin(event.cand_pt[i])))
#                    else:
#                        inputWgts.append(0.0)
#                else:
#                    if hPtNoMatch.GetBinContent(hPtNoMatch.FindBin(event.cand_pt[i])) > 10:
#                        inputWgts.append(1.0 / hPtNoMatch.GetBinContent(hPtNoMatch.FindBin(event.cand_pt[i])))
#                    else:
#                        inputWgts.append(0.0)

                
npyInputData = numpy.array(inputData, numpy.float32)
npyInputAnswer = numpy.array(inputAnswer, numpy.float32)
npyInputWgts = numpy.array(inputWgts, numpy.float32)
npyInputSampleWgts = numpy.array(inputSampleWgts, numpy.float32)

#nSig = len(npyInputWgts[npyInputAnswer==1])#sum()
#nBg = len(npyInputWgts[npyInputAnswer==0])#sum()
#
#print nSig
#print nBg
#
##Equalize the relative weights of signal and bg
#for i in xrange(len(npyInputAnswer)):
#    if npyInputAnswer[i] == 0:
#        npyInputWgts[i] *= nSig/nBg
#
#nSig = npyInputWgts[npyInputAnswer==1].sum()
#nBg = npyInputWgts[npyInputAnswer==0].sum()
#
#print nSig
#print nBg

#randomize input data
perms = numpy.random.permutation(npyInputData.shape[0])
npyInputData = npyInputData[perms]
npyInputAnswer = npyInputAnswer[perms]
npyInputWgts = npyInputWgts[perms]
npyInputSampleWgts = npyInputSampleWgts[perms]

print "TRAINING MVA"

if options.opencv:
    #clf = cv2.ml.RTrees_create()
    #
    #n_estimators = 500
    #clf.setTermCriteria((cv2.TERM_CRITERIA_COUNT, n_estimators, 0.1))
    #clf.setMaxCategories(2)
    #clf.setMaxDepth(10)
    #clf.setMinSampleCount(2)

    clf = cv2.ml.ANN_MLP_create()
    clf.setLayerSizes(numpy.array([16, 20, 2], dtype=numpy.uint8))
    clf.setTrainMethod(cv2.ml.ANN_MLP_BACKPROP)

    #make opencv TrainData container
    dataScaler = MinMaxScaler()
    dataScaler.fit(npyInputData)
    npyInputData = dataScaler.transform(npyInputData)

    cvTrainData = cv2.ml.TrainData_create(npyInputData, cv2.ml.ROW_SAMPLE, npyInputAnswer)#, sampleWeights = npyInputWgts)

    clf.train(cvTrainData)

    clf.save("TrainingOutput.model")

    #output = [clf.predict(inputs)[0] for inputs in npyInputData]
    #
    #fout = ROOT.TFile("TrainingOut.root", "RECREATE")
    #hDiscNoMatch = ROOT.TH1D("discNoMatch", "discNoMatch", 100, 0, 1.0)
    #hDiscMatch   = ROOT.TH1D("discMatch",   "discMatch", 100, 0, 1.0)
    #for i in xrange(len(output)):
    #    if npyInputAnswer[i] == 1:
    #        hDiscMatch.Fill(output[i], npyInputSampleWgts[i])
    #    else:
    #        hDiscNoMatch.Fill(output[i], npyInputSampleWgts[i])
    #hDiscNoMatch.Write()
    #hDiscMatch.Write()
    #fout.Close()

else:
    clf = MLPClassifier(hidden_layer_sizes=(50, ), verbose=False)
    #clf = RandomForestClassifier(n_estimators=500, max_depth=10, n_jobs = 4, verbose = True)
    #clf = RandomForestRegressor(n_estimators=100, max_depth=10, n_jobs = 4)
    #clf = AdaBoostRegressor(n_estimators=100)
    #clf = GradientBoostingClassifier(n_estimators=100, max_depth=6, learning_rate=0.1, random_state=0)
    #clf = GradientBoostingRegressor(n_estimators=100, max_depth=10, learning_rate=0.1, random_state=0, loss='ls')
    #clf = DecisionTreeRegressor()
    #clf = DecisionTreeClassifier()
    #clf = svm.SVC()

    dataScaler = MinMaxScaler()
    dataScaler.fit(npyInputData)
    npyInputData = dataScaler.transform(npyInputData)
    clf = clf.fit(npyInputData, npyInputAnswer)#, sample_weight=npyInputWgts)
    
    #Dump data transformation
    fileObject = open("TrainingTransform.pkl",'wb')
    out = pickle.dump(dataScaler, fileObject)
    fileObject.close()

    #Dump output from training
    fileObject = open("TrainingOutput.pkl",'wb')
    out = pickle.dump(clf, fileObject)
    fileObject.close()

    try:
        # Plot feature importance
        feature_importance = clf.feature_importances_
        feature_names = numpy.array(dg.getList())
        feature_importance = 100.0 * (feature_importance / feature_importance.max())
        sorted_idx = numpy.argsort(feature_importance)
    except AttributeError:
        pass
        
    output = clf.predict_proba(npyInputData)[:,1]
    
    fout = ROOT.TFile("TrainingOut.root", "RECREATE")
    hDiscNoMatch = ROOT.TH1D("discNoMatch", "discNoMatch", 100, 0, 1.0)
    hDiscMatch   = ROOT.TH1D("discMatch",   "discMatch", 100, 0, 1.0)
    for i in xrange(len(output)):
        if npyInputAnswer[i] == 1:
            hDiscMatch.Fill(output[i], npyInputSampleWgts[i])
        else:
            hDiscNoMatch.Fill(output[i], npyInputSampleWgts[i])
    hDiscNoMatch.Write()
    hDiscMatch.Write()

    #cv = KFold(n=len(npyInputData), n_folds=10)
    #cv_disc = 0.65
    #cv_i = 1
    #for train, test in cv:#.split(npyInputData):
    #    cv_clf = MLPClassifier(hidden_layer_sizes=(50, ), verbose=False)
    #    npyInputDataTrain = dataScaler.transform(npyInputData[train])
    #    npyInputAnswerTrain = npyInputAnswer[train]
    #    npyInputDataValid = dataScaler.transform(npyInputData[test])
    #    npyInputAnswerValid = npyInputAnswer[test]
    #    cv_clf.fit(npyInputDataTrain, npyInputAnswerTrain)
    #    outTrain = cv_clf.predict_proba(npyInputDataTrain)[:,1]
    #    outValid = cv_clf.predict_proba(npyInputDataValid)[:,1]
    #
    #    print "fold: ", cv_i
    #    #train
    #    train_acc_num = (npyInputSampleWgts[train][outTrain>cv_disc]).sum()
    #    train_acc_num_err = sqrt(((npyInputSampleWgts[train][outTrain>cv_disc])**2).sum())/train_acc_num
    #    train_acc_den = npyInputSampleWgts[train].sum()
    #    train_acc_den_err = sqrt(((npyInputSampleWgts[train])**2).sum())/train_acc_den
    #    print "Train:", train_acc_num/train_acc_den, " +/- ", train_acc_num/train_acc_den * sqrt((train_acc_num_err/train_acc_num)**2)# + (train_acc_den_err/train_acc_num_err)**2)
    #    #valid
    #    valid_acc_num = (npyInputSampleWgts[test][outValid>cv_disc]).sum()
    #    valid_acc_num_err = sqrt(((npyInputSampleWgts[test][outValid>cv_disc])**2).sum())/valid_acc_num
    #    valid_acc_den = npyInputSampleWgts[test].sum()
    #    valid_acc_den_err = sqrt(((npyInputSampleWgts[test])**2).sum())/valid_acc_den
    #    print "Validation: ", valid_acc_num/valid_acc_den, " +/- ", valid_acc_num/valid_acc_den * sqrt((valid_acc_num_err/valid_acc_num)**2)# + (valid_acc_den_err/valid_acc_num_err)**2)
    #    
    #    hDiscNoMatchi = ROOT.TH1D("discNoMatch_%i"%cv_i, "discNoMatch_%i"%cv_i, 100, 0, 1.0)
    #    hDiscMatchi   = ROOT.TH1D("discMatch_%i"%cv_i,   "discMatch_%i"%cv_i, 100, 0, 1.0)
    #    for i in xrange(len(outValid)):
    #        if npyInputAnswerValid[i] == 1:
    #            hDiscMatchi.Fill(outValid[i], npyInputSampleWgts[test][i])
    #        else:
    #            hDiscNoMatchi.Fill(outValid[i], npyInputSampleWgts[test][i])
    #    hDiscNoMatchi.Write()
    #    hDiscMatchi.Write()
    #    cv_i += 1


    fout.Close()

    try:
       #try to plot it with matplotlib
        try:
            import matplotlib.pyplot as plt
            
            # make importances relative to max importance
            pos = numpy.arange(sorted_idx.shape[0]) + .5
            #plt.subplot(1, 2, 2)
            plt.barh(pos, feature_importance[sorted_idx], align='center')
            plt.yticks(pos, feature_names[sorted_idx])
            plt.xlabel('Relative Importance')
            plt.title('Variable Importance')
            #plt.show()
            plt.savefig("feature_importance.png")
        except ImportError:
            #I guess no matplotlib is installed, just print to screen?
            featureImportanceandNames = zip(feature_names, feature_importance)
            print [featureImportanceandNames[a] for a in sorted_idx].reverse()
    except AttributeError:
        pass
    except NameError:
        pass

print "TRAINING DONE!"
