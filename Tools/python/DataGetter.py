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
    #@classmethod
    #def StandardVariables(cls, variables):
    #
    #    if variables == "TeamAlpha":
    #        vNames = ["EvtNum_double", "sampleWgt", "Weight", "fwm2_top6", "fwm3_top6", "fwm4_top6", "fwm5_top6", "fwm6_top6", "fwm7_top6", "fwm8_top6", "fwm9_top6", "fwm10_top6", "jmt_ev0_top6", "jmt_ev1_top6", "jmt_ev2_top6", "NGoodJets_double"]
    #
    #    return cls(vNames)

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

        #load data files 
        dsets = [h5py.File(filename, mode='r')['EventShapeVar'] for filename in samplesToRun]
        arrays = [da.from_array(dset, chunks=(65536, 1024)) for dset in dsets]
        x = da.concatenate(arrays, axis=0)
         
        #setup and get data
        dataColumns = np.array([np.flatnonzero(columnHeaders == v)[0] for v in variables])
        data = x[:,dataColumns]
        npyInputData = data.compute()
        #print data.shape
        
        #setup and get labels
        npyInputAnswers = np.zeros((npyInputData.shape[0], 2))
        if self.signal:
            npyInputAnswers[:,0] = 1
        else:
            npyInputAnswers[:,1] = 1
        
        #setup and get domains
        domainColumnNames = ["NGoodJets_double"]
        domainColumns = np.array([np.flatnonzero(columnHeaders == v)[0] for v in domainColumnNames])
        inputDomains = x[:,domainColumns]
        maxNJetBin = 14
        tempInputDomains = inputDomains.astype(int)
        tempInputDomains = da.reshape(tempInputDomains, [-1])
        tempInputDomains[tempInputDomains > maxNJetBin] = maxNJetBin 
        numDomains = maxNJetBin + 1 - tempInputDomains.min().compute()
        tempInputDomains = tempInputDomains - tempInputDomains.min()
        d =  np.zeros((npyInputData.shape[0], numDomains))
        #d =  np.zeros((npyInputData.shape[0], tempInputDomains.max().compute() + 1))
        d[np.arange(d.shape[0]), tempInputDomains] = 1

        #setup and get weights
        wgtColumnNames = ["sampleWgt"]
        wgtColumns = np.array([np.flatnonzero(columnHeaders == v)[0] for v in wgtColumnNames])
        npyInputSampleWgts = x[:,wgtColumns].compute()
        npyInputWgts = npyInputSampleWgts

        return {"data":npyInputData, "labels":npyInputAnswers, "domain":d, "weights":npyInputWgts, "":npyInputSampleWgts}
