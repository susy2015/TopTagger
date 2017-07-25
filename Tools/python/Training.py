import os
import errno
import optparse
from MVATrainers import mainSKL, mainXGB, mainTF
from taggerOptions import *

if __name__ == '__main__':

  #Option parsing 
  parser = getParser() 

  #We add the following options in order to run things other than tensorflow
  parser.add_option ('-k', "--sklearnrf", dest='sklearnrf', action='store_true', help="Use skl random forest instead of tensorflow")
  parser.add_option ('-x', "--xgboost", dest='xgboost', action='store_true', help="Run using xgboost")

  cmdLineOptions, args = parser.parse_args()

  if isinstance(cmdLineOptions.cfgFile, basestring): 
    options = taggerOptions.loadJSON(cmdLineOptions.cfgFile)
  else:
    options = taggerOptions.defaults()

  options = override(options,cmdLineOptions)

  #create output directory if it does not already exist 
  if len(options.runOp.directory):
    if options.runOp.directory[-1] != "/": options.runOp.directory += "/"
    try:
      os.mkdir(options.runOp.directory)
    except OSError as exc:
      if exc.errno == errno.EEXIST and os.path.isdir(options.runOp.directory):
        pass
      else:
        raise

  #run the approperaite main function 
  if cmdLineOptions.sklearnrf:
    mainSKL(cmdLineOptions)
  elif cmdLineOptions.xgboost:
    mainXGB(cmdLineOptions)
  else:
    saveOptionsToJSON(options,options.runOp.directory+options.saveName)    
    mainTF(options)

  print "TRAINING DONE!"
