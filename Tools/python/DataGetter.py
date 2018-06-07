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
            vNames = ["cand_m", "j12_m", "j13_m", "j23_m", "j1_p", "j2_p", "j3_p", "dTheta12", "dTheta23", "dTheta13", "j1_CSV", "j2_CSV", "j3_CSV", "j1_QGL", "j2_QGL", "j3_QGL"]
        elif variables == "Mixed":
            vNames = ["cand_m", "j12_m_lab", "j13_m_lab", "j23_m_lab", "j1_p", "j2_p", "j3_p", "dTheta12", "dTheta23", "dTheta13", "j1_CSV_lab", "j2_CSV_lab", "j3_CSV_lab", "j1_QGL_lab", "j2_QGL_lab", "j3_QGL_lab", "dRPtTop", "dRPtW", "sd_n2"]
        elif variables == "TeamA":
            vNames = ["j1_m_lab", "j1_CSV_lab", "j2_CSV_lab", "j2_QGL_lab", "j3_CSV_lab", "j3_QGL_lab", "cand_m", "dRPtTop", "j23_m_lab", "dRPtW", "j12_m_lab", "j13_m_lab", "sd_n2"]
        elif variables == "TeamAlphaMoreQGL":
            vNames = ["cand_m", "j12_m", "j13_m", "j23_m", "j1_p", "j2_p", "j3_p", "dTheta12", "dTheta23", "dTheta13", "j1_CSV", "j2_CSV", "j3_CSV", "j1_qgAxis1_lab", "j1_qgMult_lab", "j1_qgPtD_lab", "j2_qgAxis1_lab", "j2_qgMult_lab", "j2_qgPtD_lab", "j3_qgAxis1_lab", "j3_qgMult_lab", "j3_qgPtD_lab"]
        elif variables == "TeamAMoreQGL":
            vNames = ["j1_m_lab", "j1_CSV_lab", "j2_CSV_lab", "j3_CSV_lab", "cand_m", "dRPtTop", "j23_m_lab", "dRPtW", "j12_m_lab", "j13_m_lab", "sd_n2", "j2_qgAxis1_lab", "j2_qgMult_lab", "j2_qgPtD_lab", "j3_qgAxis1_lab", "j3_qgMult_lab", "j3_qgPtD_lab"]
        elif variables == "MixedMoreQGLCandPt":
            vNames = ["cand_m", "cand_pt", "j12_m", "j13_m", "j23_m", "j1_p", "j2_p", "j3_p", "dTheta12", "dTheta23", "dTheta13", "j1_CSV_lab", "j2_CSV_lab", "j3_CSV_lab", "dRPtTop", "dRPtW", "sd_n2", "j2_qgAxis1_lab", "j2_qgMult_lab", "j2_qgPtD_lab", "j3_qgAxis1_lab", "j3_qgMult_lab", "j3_qgPtD_lab"]
        elif variables == "TeamAlphaMoreQGLCandPt":
            vNames = ["cand_m", "cand_pt", "j12_m", "j13_m", "j23_m", "j1_p", "j2_p", "j3_p", "dTheta12", "dTheta23", "dTheta13", "j1_CSV", "j2_CSV", "j3_CSV", "j1_qgAxis1_lab", "j1_qgMult_lab", "j1_qgPtD_lab", "j2_qgAxis1_lab", "j2_qgMult_lab", "j2_qgPtD_lab", "j3_qgAxis1_lab", "j3_qgMult_lab", "j3_qgPtD_lab"]
        elif variables == "TeamAMoreQGLCandPt":
            vNames = ["cand_pt", "j1_m_lab", "j1_CSV_lab", "j2_CSV_lab", "j3_CSV_lab", "cand_m", "dRPtTop", "j23_m_lab", "dRPtW", "j12_m_lab", "j13_m_lab", "sd_n2", "j2_qgAxis1_lab", "j2_qgMult_lab", "j2_qgPtD_lab", "j3_qgAxis1_lab", "j3_qgMult_lab", "j3_qgPtD_lab"]
        elif variables == "TeamAlpha1DConv":
            vNames = ["cand_m", "cand_pt", "j12_m", "j13_m", "j23_m", "dTheta12", "dTheta23", "dTheta13", "j1_p", "j1_CSV", "j1_qgAxis1_lab", "j1_qgMult_lab", "j1_qgPtD_lab", "j1_chargedHadEFrac","j1_chargedEmEFrac", "j1_neutralEmEFrac", "j1_muonEFrac", "j1_photonEFrac", "j1_elecEFrac", "j1_chargedHadMult", "j1_neutralHadMult", "j1_photonMult", "j1_elecMult", "j1_muonMult", "j1_jetCharge", "j2_p", "j2_CSV", "j2_qgAxis1_lab", "j2_qgMult_lab", "j2_qgPtD_lab", "j2_chargedHadEFrac","j2_chargedEmEFrac", "j2_neutralEmEFrac", "j2_muonEFrac", "j2_photonEFrac", "j2_elecEFrac", "j2_chargedHadMult", "j2_neutralHadMult", "j2_photonMult", "j2_elecMult", "j2_muonMult", "j2_jetCharge", "j3_p", "j3_CSV", "j3_qgAxis1_lab", "j3_qgMult_lab", "j3_qgPtD_lab", "j3_chargedHadEFrac","j3_chargedEmEFrac", "j3_neutralEmEFrac", "j3_muonEFrac", "j3_photonEFrac", "j3_elecEFrac", "j3_chargedHadMult", "j3_neutralHadMult", "j3_photonMult", "j3_elecMult", "j3_muonMult", "j3_jetCharge"];

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
        columnHeaders = f["reco_candidates"].attrs["column_headers"]
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
        dsets = [h5py.File(filename, mode='r')['reco_candidates'] for filename in samplesToRun]
        
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

