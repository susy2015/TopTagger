import numpy
import pandas as pd
from DataGetter import DataGetter
from math import sqrt, exp
from time import sleep
from glob import glob

dataPath = "/cms/data/cmadrid/EventShapeTrainingData_V2"

def mainSKL(options):

  from sklearn.ensemble import RandomForestClassifier
  from sklearn.neural_network import MLPClassifier
  #import xgboost as xgb
  import pickle

  print "PROCESSING TRAINING DATA"

  from taggerOptions import StandardVariables, getJetVarNames

  #get variables 
  globalVars, jetVars = StandardVariables(options.variables)
  allVars = globalVars + getJetVarNames(jetVars)

  print allVars

  # Import data
  #dg = DataGetter(allVars)
  dgSig = DataGetter.DefinedVariables(allVars, signal = True,  background = False)
  dgBg = DataGetter.DefinedVariables(allVars,  signal = False, background = True)
  dataFiles = []
  dataFiles += glob(dataPath + "/trainingTuple_*_division_*_rpv_stop_*_training_0.h5")
  dataFiles2 =glob(dataPath + "/trainingTuple_*_division_0_TT_training_0.h5")

  dataSig = dgSig.importData(samplesToRun = tuple(dataFiles), prescale=True, ptReweight=False)
  dataBg = dgBg.importData(samplesToRun = tuple(dataFiles2), prescale=True, ptReweight=False)
  
  minLen = min(len(dataSig["data"]),len(dataBg["data"]))

  trainDataArray = [dataSig,dataBg]
  trainData = {}
  for data in trainDataArray:
    for key in data:
      if key in trainData:
        trainData[key] = numpy.vstack([trainData[key], data[key][:minLen]])
      else:
        trainData[key] = data[key][:minLen]


  perms = numpy.random.permutation(trainData["data"].shape[0])
  for key in trainData:
    trainData[key] = trainData[key][perms]

  # Create random forest
  #clf = xgb.XGBClassifier(base_score=0.5, colsample_bylevel=1, colsample_bytree=1,
  #                        gamma=0, learning_rate=0.001, max_delta_step=0, max_depth=6,
  #                        min_child_weight=0.1, missing=None, n_estimators=2000, nthread=28,
  #                        objective='binary:logistic', reg_alpha=0, reg_lambda=0.01,
  #                        scale_pos_weight=1, seed=0, silent=False, subsample=1 )
  #clf = RandomForestClassifier(n_estimators=500, max_depth=10, n_jobs = 28, verbose = True)
  clf = MLPClassifier(hidden_layer_sizes=(20))

  print "TRAINING RF"
  
  # Train random forest 
  clf = clf.fit(trainData["data"], trainData["labels"][:,0])#, sample_weight=trainData["weights"][:,0])
  
  #Dump output from training
  fileObject = open(options.directory + "/" + "TrainingOutput.pkl",'wb')
  out = pickle.dump(clf, fileObject)
  fileObject.close()
      
  #output = clf.predict_proba(trainData["data"])[:,1]

def mainXGB(options):

  import xgboost as xgb
  from taggerOptions import StandardVariables, getJetVarNames

  print "PROCESSING TRAINING DATA"

  #get variables 
  globalVars, jetVars = StandardVariables(options.variables)
  allVars = globalVars + getJetVarNames(jetVars)

  # Import data
  dg = DataGetter(allVars)
  dataFiles = []
  dataFiles += glob(options.dataFilePath + "/")
  dataFiles += glob(options.dataFilePath + "/")
  dataFiles += glob(options.dataFilePath + "/")
  dataFiles += glob(options.dataFilePath + "/")
  trainData = dg.importData(samplesToRun = tuple(dataFiles), prescale=True, ptReweight=options.ptReweight)
  #= dg.importData(samplesToRun = tuple(glob(options.dataFilePath + "/trainingTuple_TTbarSingleLepT*_0_division_0_TTbarSingleLepT*_training_[0].h5")), prescale=True, ptReweight=options.ptReweight)

  print "TRAINING XGB"

  # Create xgboost classifier
  # Train random forest 
  xgData = xgb.DMatrix(trainData["data"], label=trainData["labels"][:,0])#, weight=trainData["weights"][:,0])
  param = {'max_depth':6, 'eta':0.03, 'objective':'binary:logistic', 'eval_metric':['error', 'auc', 'logloss'], 'nthread':28 }
  gbm = xgb.train(param, xgData, num_boost_round=2000)
  
  #Dump output from training
  gbm.save_model(options.directory + "/" + 'TrainingModel.xgb')

  #output = gbm.predict(xgData)


def getValidData(dg, validDataFiles, options):
  validDataArray = []

  #nSamples = 0
  #for dsn in validDataFiles:
  #  nSamples += dsn[1]

  minValidDataSize = 999999999
  for dsn in validDataFiles:
    validDataArray.append(dg.importData(samplesToRun = tuple(dsn[0]), ptReweight=options.runOp.ptReweight))

    arrayLen = len(validDataArray[-1]["data"])
    dataMultiplier = dsn[1]
    validDataSize = arrayLen/dataMultiplier
    if validDataSize < minValidDataSize:
      minValidDataSize = validDataSize
  
  validData = {}
  for data in validDataArray:
    for key in data:
      if key in validData:
        validData[key] = numpy.vstack([validData[key], data[key][:minValidDataSize*dataMultiplier]])
      else:
        validData[key] = data[key][:minValidDataSize*dataMultiplier]
  
  perm = numpy.random.permutation(validData["data"].shape[0])

  for key in validData:
    validData[key] = validData[key][perm]

  return validData

def combineValidationData(validDataSig, validDataBg):
  minNumalidData = min(len(validDataSig["data"]), len(validDataBg["data"]))
  
  validData = {}
  for key in validDataSig:
    validData[key] = numpy.vstack([validDataBg[key][:minNumalidData], validDataSig[key][:minNumalidData]])

  return validData

def mainTF(options):

  import tensorflow as tf
  from CreateModel import CreateModel
  from DataManager import DataManager
  from DataSet import DataSet

  print "PROCESSING VALIDATION DATA"

  dgSig = DataGetter.DefinedVariables(options.netOp.vNames, signal = True, background = False)
  dgBg = DataGetter.DefinedVariables(options.netOp.vNames,  signal = False, background = True)

  validDataSig = [((dataPath + "/trainingTuple_0_division_1_rpv_stop_850_validation_0.h5", ), 2),]

  validDataSig2 = [((dataPath + "/trainingTuple_0_division_1_stealth_stop_350_SHuHd_validation_0.h5", ), 2),]

  validDataSig3 = [((dataPath + "/trainingTuple_0_division_1_rpv_stop_350_validation_0.h5", ), 2),]

  validDataBgTTbar = [((dataPath + "/trainingTuple_20_division_1_TT_validation_0.h5", ), 1),
                      ((dataPath + "/trainingTuple_2110_division_1_TT_validation_0.h5", ), 1),]
  
  print "Input Variables: ",len(dgSig.getList())

  # Import data
  #print options.runOp.validationSamples
  
  validDataSig =       getValidData(dgSig, validDataSig,      options)
  validDataSig2 =      getValidData(dgSig, validDataSig2,     options)
  validDataSig3 =      getValidData(dgSig, validDataSig3,     options)
  validDataBgTTbar =   getValidData(dgBg,  validDataBgTTbar,  options)

  validDataTTbar = combineValidationData(validDataSig, validDataBgTTbar)
  validDataQCDMC = combineValidationData(validDataSig2, validDataBgTTbar)
  validDataQCDData = combineValidationData(validDataSig3, validDataBgTTbar)

  #get input/output sizes
  #print validData["data"].shape
  nFeatures = validDataTTbar["data"].shape[1]
  nLabels = validDataTTbar["labels"].shape[1]
  nWeights = validDataTTbar["weights"].shape[1]
  nDomain = validDataSig["domain"].shape[1]

  #Training parameters
  l2Reg = options.runOp.l2Reg
  MiniBatchSize = options.runOp.minibatchSize
  nEpoch = options.runOp.nepoch
  ReportInterval = options.runOp.reportInterval
  validationCount = min(options.runOp.nValidationEvents, validDataTTbar["data"].shape[0])

  #scale data inputs to mean 0, stddev 1
  categories = numpy.array(options.netOp.vCategories)
  mins = numpy.zeros(categories.shape, dtype=numpy.float32)
  ptps = numpy.zeros(categories.shape, dtype=numpy.float32)
  for i in xrange(categories.max()):
    selectedCategory = categories == i
    mins[selectedCategory] = validDataTTbar["data"][:,selectedCategory].mean()
    ptps[selectedCategory] = validDataTTbar["data"][:,selectedCategory].std()
  ptps[ptps < 1e-10] = 1.0

  ##Create data manager, this class controls how data is fed to the network for training
  #                 DataSet(fileGlob, xsec, Nevts, kFactor, sig, prescale, rescale)
  signalDataSets = [
                    #DataSet(dataPath + "/trainingTuple_*_division_*_rpv_stop_350_training_0.h5",      365.4,  61878989, 1.0, True,  0, 1.0, 1.0, 1),
                    #DataSet(dataPath + "/trainingTuple_*_division_*_rpv_stop_450_training_0.h5",      365.4,  61878989, 1.0, True,  0, 1.0, 1.0, 1),
                    DataSet(dataPath + "/trainingTuple_*_division_*_rpv_stop_550_training_0.h5",      365.4,  61878989, 1.0, True,  0, 1.0, 1.0, 1),
                    DataSet(dataPath + "/trainingTuple_*_division_*_rpv_stop_650_training_0.h5",      365.4,  61878989, 1.0, True,  0, 1.0, 1.0, 1),
                    DataSet(dataPath + "/trainingTuple_*_division_*_rpv_stop_750_training_0.h5",      365.4,  61878989, 1.0, True,  0, 1.0, 1.0, 1),
                    DataSet(dataPath + "/trainingTuple_*_division_*_rpv_stop_850_training_0.h5",      365.4,  61878989, 1.0, True,  0, 1.0, 1.0, 1),

                    #DataSet(dataPath + "/trainingTuple_*_division_*_stealth_stop_350_SHuHd_training_0.h5",      365.4,  61878989, 1.0, True,  0, 1.0, 1.0, 1),
                    #DataSet(dataPath + "/trainingTuple_*_division_*_stealth_stop_450_SHuHd_training_0.h5",      365.4,  61878989, 1.0, True,  0, 1.0, 1.0, 1),
                    DataSet(dataPath + "/trainingTuple_*_division_*_stealth_stop_550_SHuHd_training_0.h5",      365.4,  61878989, 1.0, True,  0, 1.0, 1.0, 1),
                    DataSet(dataPath + "/trainingTuple_*_division_*_stealth_stop_650_SHuHd_training_0.h5",      365.4,  61878989, 1.0, True,  0, 1.0, 1.0, 1),
                    DataSet(dataPath + "/trainingTuple_*_division_*_stealth_stop_750_SHuHd_training_0.h5",      365.4,  61878989, 1.0, True,  0, 1.0, 1.0, 1),
                    DataSet(dataPath + "/trainingTuple_*_division_*_stealth_stop_850_SHuHd_training_0.h5",      365.4,  61878989, 1.0, True,  0, 1.0, 1.0, 1),

                    #DataSet(dataPath + "/trainingTuple_*_division_*_stealth_stop_350_SYY_training_0.h5",      365.4,  61878989, 1.0, True,  0, 1.0, 1.0, 1),
                    #DataSet(dataPath + "/trainingTuple_*_division_*_stealth_stop_450_SYY_training_0.h5",      365.4,  61878989, 1.0, True,  0, 1.0, 1.0, 1),
                    DataSet(dataPath + "/trainingTuple_*_division_*_stealth_stop_550_SYY_training_0.h5",      365.4,  61878989, 1.0, True,  0, 1.0, 1.0, 1),
                    DataSet(dataPath + "/trainingTuple_*_division_*_stealth_stop_650_SYY_training_0.h5",      365.4,  61878989, 1.0, True,  0, 1.0, 1.0, 1),
                    DataSet(dataPath + "/trainingTuple_*_division_*_stealth_stop_750_SYY_training_0.h5",      365.4,  61878989, 1.0, True,  0, 1.0, 1.0, 1),
                    DataSet(dataPath + "/trainingTuple_*_division_*_stealth_stop_850_SYY_training_0.h5",      365.4,  61878989, 1.0, True,  0, 1.0, 1.0, 1),

                    #DataSet(dataPath + "/trainingTuple_*_division_*_rpv_stop_*_training_0.h5",      365.4,  61878989, 1.0, True,  0, 1.0, 1.0, 1),
                    #DataSet(dataPath + "/trainingTuple_*_division_*_stealth_stop_*_SHuHd_training_0.h5",   365.4,  61901450, 1.0, True,  0, 1.0, 1.0, 1),
                    #DataSet(dataPath + "/trainingTuple_*_division_*_stealth_stop_*_SYY_training_0.h5",   365.4,  61901450, 1.0, True,  0, 1.0, 1.0, 1),
  ]

  backgroundDataSets = [DataSet(dataPath + "/trainingTuple_*_division_0_TT_training_0.h5",    365.4,  61878989, 1.0, False, 0, 1.0, 1.0, len(signalDataSets)),]

  dm = DataManager(options.netOp.vNames, nEpoch, nFeatures, nLabels, nDomain, nWeights, options.runOp.ptReweight, signalDataSets, backgroundDataSets)

  # Build the graph
  denseNetwork = [nFeatures]+options.netOp.denseLayers+[nLabels]
  convLayers = options.netOp.convLayers
  rnnNodes = options.netOp.rnnNodes
  rnnLayers = options.netOp.rnnLayers
  mlp = CreateModel(options, denseNetwork, convLayers, rnnNodes, rnnLayers, dm.inputDataQueue, MiniBatchSize, mins, 1.0/ptps)

  #summary writer
  summary_writer = tf.summary.FileWriter(options.runOp.directory + "log_graph", graph=tf.get_default_graph())

  print "TRAINING NETWORK"

  with tf.Session(config=tf.ConfigProto(intra_op_parallelism_threads=8) ) as sess:
    sess.run(tf.global_variables_initializer())

    #start queue runners
    dm.launchQueueThreads(sess)

    print "Reporting validation loss every %i batches with %i events per batch for %i epochs"%(ReportInterval, MiniBatchSize, nEpoch)

    #preload the first data into staging area
    sess.run([mlp.stagingOp], feed_dict={mlp.reg: l2Reg, mlp.keep_prob:options.runOp.keepProb})

    i = 0
    N_TRAIN_SUMMARY = 10

    #flush queue until the sample fraction is approximately equal 
    while dm.continueTrainingLoop():
      result = sess.run(dm.inputDataQueue.dequeue_many(MiniBatchSize))
      signalFraction =  result[1][:,0].sum()/MiniBatchSize
      #the first this fraction drops below 0.5 means we are close enough to equal signal/bg fraction 
      if signalFraction < 0.5:
        break

    try:
      while dm.continueTrainingLoop():
        grw = 1.0#1*(2/(1+exp(-i/10000.0)) - 1) #2/(1+exp(-i/10000.0)) - 1 #1000000000000.0*(2/(1+exp(-i/500000.0)) - 1)

        #run validation operations 
        if i == 0 or not i % ReportInterval:
          #run validation operations 
          validation_loss, accuracy, summary_vl = sess.run([mlp.loss_ph, mlp.accuracy, mlp.merged_valid_summary_op], feed_dict={mlp.x_ph: validDataTTbar["data"][:validationCount], mlp.y_ph_: validDataTTbar["labels"][:validationCount], mlp.p_ph_: validDataTTbar["domain"][:validationCount], mlp.reg: l2Reg, mlp.gradientReversalWeight:grw, mlp.wgt_ph: validDataTTbar["weights"][:validationCount]})
          summary_writer.add_summary(summary_vl, i/N_TRAIN_SUMMARY)
        
          print('Interval %d, validation accuracy %0.6f, validation loss %0.6f' % (i/ReportInterval, accuracy, validation_loss))
        
          validation_loss, accuracy, summary_vl_QCDMC = sess.run([mlp.loss_ph, mlp.accuracy, mlp.merged_valid_QCDMC_summary_op], feed_dict={mlp.x_ph: validDataQCDMC["data"][:validationCount], mlp.y_ph_: validDataQCDMC["labels"][:validationCount], mlp.p_ph_: validDataQCDMC["domain"][:validationCount], mlp.reg: l2Reg, mlp.gradientReversalWeight:grw, mlp.wgt_ph: validDataQCDMC["weights"][:validationCount]})
          summary_writer.add_summary(summary_vl_QCDMC, i/N_TRAIN_SUMMARY)
        
          validation_loss, accuracy, summary_vl_QCDData = sess.run([mlp.loss_ph, mlp.accuracy, mlp.merged_valid_QCDData_summary_op], feed_dict={mlp.x_ph: validDataQCDData["data"][:validationCount], mlp.y_ph_: validDataQCDData["labels"][:validationCount], mlp.p_ph_: validDataQCDData["domain"][:validationCount], mlp.reg: l2Reg, mlp.gradientReversalWeight:grw, mlp.wgt_ph: validDataQCDData["weights"][:validationCount]})
          summary_writer.add_summary(summary_vl_QCDData, i/N_TRAIN_SUMMARY)

          #print(sess.run(mlp.x))

        #run training operations 
        if i % N_TRAIN_SUMMARY == 0:
          _, _, summary = sess.run([mlp.stagingOp, mlp.train_step, mlp.merged_train_summary_op], feed_dict={mlp.reg: l2Reg, mlp.keep_prob:options.runOp.keepProb, mlp.training: True, mlp.gradientReversalWeight:grw})
          summary_writer.add_summary(summary, i/N_TRAIN_SUMMARY)
        else:
          sess.run([mlp.stagingOp, mlp.train_step], feed_dict={mlp.reg: l2Reg, mlp.keep_prob:options.runOp.keepProb, mlp.training: True})
        i += 1

      #Should fix bad end of training state
      while dm.continueFlushingQueue():
        sess.run(dm.inputDataQueue.dequeue_many(MiniBatchSize))

    except Exception, e:
      # Report exceptions to the coordinator.
      dm.requestStop(e)
    finally:
      # Terminate as usual. It is safe to call `coord.request_stop()` twice.
      dm.requestStop()
      dm.join()

    mlp.saveCheckpoint(sess, options.runOp.directory)
    mlp.saveModel(sess, options.runOp.directory)

