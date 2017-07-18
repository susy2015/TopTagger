from optparse import OptionParser
import json

def StandardVariables(variables):
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

   return vNames

#The classes defined in this file are used to control the TopTagger
#It will define default values
#Allow those default values to be overwritten
#Load options from a json file
#Save the options used (The saved file can be reused to as a configuration file

class runOptions:
#This class will track the specific run options, these include things such as 
#ouput directory, input files, etc.
   def __init__(self, runName           = "Test Run Configurations (name not set)",
                      directory         = "",
                      nepoch            = 100,
                      nReaders          = 4,
                      nThreadperReader  = 1,
                      minibatchSize     = 512,
                      reportInterval    = 1000,
                      nValidationEvents = 10000,
                      l2Reg             = 0.0001,
                      dataPath          = "data",
                      sampleNames       = ["trainingTuple_division_1_TTbarSingleLep_validation_100k_0.h5"],
                      ptReweight        = True):

      self.runName           = runName
      self.directory         = directory
      self.nepoch            = nepoch
      self.nReaders          = nReaders
      self.nThreadperReader  = nThreadperReader
      self.minibatchSize     = minibatchSize
      self.reportInterval    = reportInterval
      self.nValidationEvents = nValidationEvents
      self.l2Reg             = l2Reg
      self.dataPath          = dataPath
      self.sampleNames       = sampleNames
      
      self.makeSamplesToRun()

      self.ptReweight        = ptReweight
      
   #This method uses the dataPath and the list of sampleNames to make a list of training files
   def makeSamplesToRun(self):
      
      self.samplesToRun = []
      for name in self.sampleNames:
         self.samplesToRun.append(self.dataPath+"/"+name)

      return

   #This method will add the command-line options related to the parameters stored in the runOptions object
   @classmethod
   def getParser(cls, parser):

      if not isinstance(parser, OptionParser):
         print "Object passed to runOptions.getParser is not an OptionParser object"
         parser = OptionsParser()

      parser.add_option ('-p', "--ptReweight",        dest="ptReweight",        action='store',               help="Reweight pt spectrum of events durring training")
      parser.add_option ('-d', "--directory",         dest='directory',         action='store',                    help="Directory to store outputs (default .)")
      parser.add_option ('-e', "--nepoch",            dest='nepoch',            action='store',      type="int",   help="Number of training epoch (default 50)")
      parser.add_option ('-n', "--nReaders",          dest="nReaders",          action='store',      type="int",   help="Number of file readers to use (default 4)")
      parser.add_option ('-q', "--nThreadperReader",  dest="nThreadperReader",  action='store',      type="int",   help="Number of threads for each flie reader (default 1)")
      parser.add_option ('-b', "--minibatchSize",     dest="minibatchSize",     action='store',      type="int",   help="Number of events per minibatch in training (default 512)")
      parser.add_option ('-r', "--reportInterval",    dest="reportInterval",    action='store',      type="int",   help="Number of training batches between each validation check (default 1000)")
      parser.add_option ('-l', "--nValidationEvents", dest="nValidationEvents", action='store',      type="int",   help="Number of validation events to use for each validation step (default 10000)")
      parser.add_option ('-f', "--dataFilePath",      dest="dataFilePath",      action='store',                    help="Path where the input datafiles are stored (default: \"data\")")
      parser.add_option ('-g', "--l2Reg",             dest="l2Reg",             action='store',      default=0.0001,       type="float", help="Scale factor for the L2 regularization term of the loss (default 0.0001)")

      return parser      

   #This methods will take options provided by the parser, and if it is not the default value, it will what is currently saved
   def override(self, cloptions):
      orList = []

      if cloptions.ptReweight != None: 
         self.ptReweight = cloptions.ptReWeight
         orList.append("ptReweight = "+str(cloptions.ptReweight))

      if cloptions.directory != None: 
         self.directory = cloptions.directory
         orList.append("directory = "+str(cloptions.directory))

      if cloptions.nepoch != None: 
         self.nepoch = cloptions.nepoch
         orList.append("nepoch = "+str(cloptions.nepoch))

      if cloptions.nReaders != None: 
         self.nReaders = cloptions.nReaders
         orList.append("nReaders = "+str(cloptions.nReaders))

      if cloptions.nThreadperReader != None: 
         self.nThreadperReader = cloptions.nThreadperReader
         orList.append("nThreadperReader = "+str(cloptions.nThreadperReader))

      if cloptions.minibatchSize != None: 
         self.minibatchSize = cloptions.minibatchSize
         orList.append("minibatchSize = "+str(cloptions.minibatchSize))

      if cloptions.reportInterval != None: 
         self.reportInterval = cloptions.reportInterval
         orList.append("reportInterval = "+str(cloptions.reportInterval))

      if cloptions.nValidationEvents != None: 
         self.nValidationEvents = cloptions.nValidationEvents
         orList.append("nValidationEvents = "+str(cloptions.nValidationEvents))

      if cloptions.l2Reg != None: 
         self.l2Reg = cloptions.l2Reg
         orList.append("l2Reg = "+str(cloptions.l2Reg))

      if cloptions.dataFilePath != None: 
         self.dataPath = cloptions.dataFilePath
         orList.append("dataPath = "+str(cloptions.dataFilePath))
         self.makeSamplesToRun()

      if len(orList):
         info = "runOptions overriden by command line:"
         for s in orList:
           info = info + " " + s
      else:
         info = "No runOptions overriden by command line"

      return info

   #This class method will return a runOptions object with the default values
   @classmethod
   def defaults(cls):
      return cls()

   #This method will accept a dict, and try to fill in the values, it will need to be modified if the class structure becomes more complicated
   @classmethod
   def makeFromDict(cls, runDict):
      rOptions = cls.defaults()

      if not isinstance(runDict, dict):
         print "runOptions.makeFromDict requires a dict object"
         return rOptions

      #This is the part that needs to be changed if the structure becomes more complicated
      for key, value in runDict.iteritems():
          setattr(rOptions, key, value)

      return rOptions

class networkOptions:
#This class defined the structure of the network.
#These include things such as the input variables, output variables,
#and the type and structure of the network
   def __init__(self, networkName    = "Network test Configuration (name not set)",
                      inputVariables = ["cand_m", "j12_m", "j13_m", "j23_m", "dTheta12", "dTheta23", "dTheta13"],
                      jetVariables   = ["p", "CSV", "QGL"]):

      self.networkName      = networkName
      self.inputVariables   = inputVariables
      self.jetVariables     = jetVariables

      self.jetVariablesList = [jet+var for var in jetVariables for jet in ["j1_","j2_","j3_"]]

      self.numPassThru      = len(inputVariables)
      self.vNames           = self.inputVariables+self.jetVariablesList      

   #This method will add the command-line options related to the parameters stored in the networkOptions object
   @classmethod
   def getParser(cls, parser):

      if not isinstance(parser, OptionParser):
         print "Object passed to runOptions.getParser is not an OptionParser object"
         parser = OptionsParser()

      parser.add_option ('-v', "--variables", dest='variables', action='store', help="Input features to use (default TeamAlpha)")
      parser.add_option ('-m', "--modelCfg",  dest="modelJSON", action='store', help="JSON with model definitions")

      return parser    

   #This methods will take options provided by the parser, and if it is not the default value, it will what is currently saved
   def override(self, cloptions):
    
      if cloptions.modelJSON != None:
         try:
            f = open(cloptions.modelJSON,"r")
            cfgs = json.load(f)
         except IOError:
            print "Unable to open", cloptions.modelJSON
            loaded = false
         else:
            print "Loadinging",cloptions.variables,"from",cloptions.modelJSON
            self.vNames = cfgs[cloptions.variables] #the json file is a dictionary, cloptions.variables specifies the key to use
            return "Loaded "+cloptions.variables+" from "+cloptions.modelJSON 

      elif cloptions.variables != None:
         self.vNames = StandardVariables(cloptions.variables)
         return "Loaded standard input variables named "+cloptions.variables

      return "No networkOptions overriden by command-line"    

   @classmethod
   def defaults(cls):
      return cls()

   #This method will accept a dict, and try to fill in the values, it will need to be modified if the class structure becomes more complicated
   @classmethod
   def makeFromDict(cls, netDict):
      nOptions = cls.defaults()

      if not isinstance(netDict, dict):
         print "networkOptions.makeFromDict requires a dict object"
         return nOptions

      #This is the part that needs to be changed if the structure becomes more complicated
      for key, value in netDict.iteritems():
          setattr(nOptions, key, value)

      return nOptions

class taggerOptions:
#This class will track and save the options used in a run of the TopTagger

   #The object is instantiated by passing to it a runOptions and a networkOptions objects
   def __init__(self, confName = "Default Configuration (no name set)", runOp=runOptions.defaults(), netOp=networkOptions.defaults(),saveName="config.json"):
      self.confName = confName
      self.runOp    = runOp
      self.netOp    = netOp
      self.saveName = saveName

      self.info     = ['Info List']

   #This class  method will generate a taggerOptions object from a given json file
   @classmethod
   def loadJSON(cls, fname = "taggerCfg.json"):
      if fname == "taggerCfg.json":
         print "No configuration file has been specified, using default (taggerCfg.json)"

      try:
         f           = open(fname,"r")
         jsonOptions = json.load(f)
      except IOError:
         print "Unable to open",fname,"default options will be used"
         return cls.defaults()

      rDict = jsonOptions['runOp']
      runOpJSON = runOptions.makeFromDict(rDict)

      nDict = jsonOptions['netOp']
      netOpJSON = networkOptions.makeFromDict(nDict)

      return cls(confName = jsonOptions['confName'], runOp = runOpJSON, netOp = netOpJSON) 
      
   #This class method will return a taggerOptions object with the default values
   @classmethod
   def defaults(cls):
      return cls(runOp=runOptions.defaults(),netOp=networkOptions.defaults())  

   #This method will setup the parser object with the ability to receive options from the command line
   @classmethod
   def getParser(cls, parser):

      parser.add_option('-c', "--cfg", dest='cfgFile', action='store', default = None, help="Load configuration from this file")

      parser = runOptions.getParser(parser)
      parser = networkOptions.getParser(parser)

      return parser

   def override(self, cloptions):
   
       #The override methods will return a string with info that will be saved in the info list.
       self.info.append(self.runOp.override(cloptions))
       self.info.append(self.netOp.override(cloptions))


   def serialize(self):
      if not isinstance(self, taggerOptions):
         print "Trying to convert non taggerOptions object with serializeTaggerOptions"
         return

      output = self.__dict__
      output["runOp"] = self.runOp.__dict__ #This assumes a simple structure for runOp, this needs to be rewritten is that assumtion is violated
      output["netOp"] = self.netOp.__dict__ #This assumes a simple structure for runOp, this needs to be rewritten is that assumtion is violated

      return output


def getParser():
   parser = OptionParser()
   taggerOptions.getParser(parser)

   return parser

def saveOptionsToJSON(options,fname):
   if not isinstance(options, taggerOptions):
      print "Incorrect object passed. saveOptionsToJSON works on taggerOptions objects. Nothing will be saved."
      return

   try:
      f = open(fname,"w")
      json.dump(options.serialize(), f, sort_keys=True, indent=4)
   except IOError:
      print "Unable to save configuration to",fname

   return

def override(options, cloptions):
   if not isinstance(options, taggerOptions):
      print "override only accepts taggerOptions objects"
   else:
      options.override(cloptions)

   return options 

