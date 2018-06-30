import numpy as np
import pandas as pd
import dask.array as da

class DataGetter:

    #The constructor simply takes in a list and saves it to self.list
    def __init__(self, variables, signal = True, background = True, domain = None, bufferData = False):
        self.list = variables
        self.signal = signal
        self.domain = domain
        self.background = background
        self.bufferData = bufferData
        self.dataMap = {}

    #This method accepts a string and will return a DataGetter object with the variable list defined in this method.
    @classmethod
    def StandardVariables(cls, variables):

        if variables == "TeamAlpha":
            vNames = ["EvtNum_double", "sampleWgt", "Weight", "fwm2_top6", "fwm3_top6", "fwm4_top6", "fwm5_top6", "fwm6_top6", "fwm7_top6", "fwm8_top6", "fwm9_top6", "fwm10_top6", "jmt_ev0_top6", "jmt_ev1_top6", "jmt_ev2_top6", "NGoodJets_double"]

        return cls(vNames)

    #Simply accept a list and pass it to the constructor
    @classmethod
    def DefinedVariables(cls, variables, signal = True, background = True, bufferData = False):
        return cls(variables, signal, background, bufferData)

    def getList(self):
        return self.list
    
    def prescaleBackground(self, input, answer, prescale):
      return np.vstack([input[answer == 1], input[answer != 1][::prescale]])
    
    def importData(self, samplesToRun, prescale = True, ptReweight=True, randomize = True):

      #check if this file was cached 
      if (samplesToRun, prescale, ptReweight) in self.dataMap:
        npyInputData, npyInputAnswers, npyInputWgts, npyInputSampleWgts = self.dataMap[samplesToRun, prescale, ptReweight]

      else:
        #variables to train
        vars = self.getList()
        
        inputData = np.empty([0])
        npyInputWgts = np.empty([0])
        
        import h5py

        variables = vars

        f = h5py.File(samplesToRun[0], "r")
        columnHeaders = f["EventShapeVar"].attrs["column_headers"]
        f.close()

        for v in variables:
            if not v in columnHeaders:
                print "Variable not found: %s"%v

        dataColumns = np.array([np.flatnonzero(columnHeaders == v)[0] for v in variables])
        
        labelColumnNames = ["genConstiuentMatchesVec", "genTopMatchesVec", "ncand"]
        labelColumns = np.array([np.flatnonzero(columnHeaders == v)[0] for v in labelColumnNames])
        
        wgtColumnNames = ["sampleWgt"]
        wgtColumns = np.array([np.flatnonzero(columnHeaders == v)[0] for v in wgtColumnNames])
        
        #load data files 
        dsets = [h5py.File(filename, mode='r')['EventShapeVar'] for filename in samplesToRun]
        
        arrays = [da.from_array(dset, chunks=(65536, 1024)) for dset in dsets]
        
        x = da.concatenate(arrays, axis=0)
        
        data = x[:,dataColumns]
        
        #remove partial tops 
        inputLabels = x[:,labelColumns]
        inputAnswer = (inputLabels[:,0] > 2.99) & (inputLabels[:,1] > 0.99)
    
        inputBackground = (inputLabels[:,0] == 0) & da.logical_not(inputLabels[:,1])
    
        filterArray = []
        if self.signal and self.background:
            filterArray = ((inputAnswer == 1) | (inputBackground == 1)) & (inputLabels[:,2] > 0)
        elif self.signal:
            filterArray = (inputAnswer == 1) & (inputLabels[:,2] > 0)
        elif self.background:
            filterArray = (inputBackground == 1) & (inputLabels[:,2] > 0)

        npyInputData = data[filterArray].compute()
        #npyInputLabels = inputData.as_matrix(["genConstiuentMatchesVec", "genTopMatchesVec"])
        npyInputAnswer = inputAnswer
        npyInputAnswers = da.vstack([npyInputAnswer,da.logical_not(npyInputAnswer)]).transpose()[filterArray].compute()
        npyInputSampleWgts = x[:,wgtColumns][filterArray].compute()
        npyInputWgts = npyInputSampleWgts

        d = np.zeros((npyInputData.shape[0], 2))
        if self.domain != None and self.domain > 0:
            d[:,self.domain - 1] = 1

        return {"data":npyInputData, "labels":npyInputAnswers, "domain":d, "weights":npyInputWgts, "":npyInputSampleWgts}

