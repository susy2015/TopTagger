import os
import errno
import optparse
from MVATrainers import mainSKL, mainXGB, mainTF
from taggerOptions import *

if __name__ == '__main__':

  #Option parsing 
  parser = optparse.OptionParser("usage: %prog [options]\n")

  parser.add_option ('-k', "--sklearnrf",         dest='sklearnrf',         action='store_true',                                     help="Run using sklearn RF")
  parser.add_option ('-x', "--xgboost",           dest='xgboost',           action='store_true',                                     help="Run using xgboost")
  parser.add_option ('-p', "--ptReweight",        dest="ptReweight",        action='store_true',                                     help="Reweight pt spectrum of events durring training")
  parser.add_option ('-d', "--directory",         dest='directory',         action='store',      default="",                         help="Directory to store outputs (default .)")
  parser.add_option ('-v', "--variables",         dest='variables',         action='store',      default="TeamAlpha",                help="Input features to use (default TeamAlpha)")
  parser.add_option ('-e', "--nepoch",            dest='nepoch',            action='store',      default=50,           type="int",   help="Number of training epoch (default 50)")
  parser.add_option ('-n', "--nReaders",          dest="nReaders",          action='store',      default=4,            type="int",   help="Number of file readers to use (default 4)")
  parser.add_option ('-q', "--nThreadperReader",  dest="nThreadperReader",  action='store',      default=1,            type="int",   help="Number of threads for each flie reader (default 1)")
  parser.add_option ('-b', "--minibatchSize",     dest="minibatchSize",     action='store',      default=512,          type="int",   help="Number of events per minibatch in training (default 512)")
  parser.add_option ('-r', "--reportInterval",    dest="reportInterval",    action='store',      default=1000,         type="int",   help="Number of training batches between each validation check (default 1000)")
  parser.add_option ('-l', "--nValidationEvents", dest="nValidationEvents", action='store',      default=10000,        type="int",   help="Number of validation events to use for each validation step (default 10000)")
  parser.add_option ('-g', "--l2Reg",             dest="l2Reg",             action='store',      default=0.0001,       type="float", help="Scale factor for the L2 regularization term of the loss (default 0.0001)")
  parser.add_option ('-m', "--modelCfg",          dest="modelJSON",         action='store',      default="model.json",               help="JSON with model definitions")
  parser.add_option ('-f', "--dataFilePath",      dest="dataFilePath",      action='store',      default="data",                     help="Path where the input datafiles are stored (default: \"data\")")

  options, args = parser.parse_args()

  #The following code will be enabled when taggerOptions is turned on 
  #parser = getParser() 

  #We add the following options in order to things other than tensorflow
  #parser.add_option ('-k', "--sklrf", dest='sklrf', action='store_true', help="Use skl random forest instead of tensorflow")
  #parser.add_option ('-x', "--xgboost", dest='xgboost', action='store_true', help="Run using xgboost")

  #cmdLineOptions, args = parser.parse_args()

  #if isinstance(cmdLineOptions.cfgFile, basestring): 
  #  options = taggerOptions.loadJSON(cmdLineOptions.cfgFile)
  #else:
  #  options = taggerOptions.defaults()

  #options = override(options,cmdLineOptions)

  #create output directory if it does not already exist 
  if len(options.directory):
    options.directory += "/"
    try:
      os.mkdir(options.directory)
    except OSError as exc:
      if exc.errno == errno.EEXIST and os.path.isdir(options.directory):
        pass
      else:
        raise

  #run the approperaite main function 
  if options.sklearnrf:
    mainSKL(options)
  elif options.xgboost:
    mainXGB(options)
  else:
    mainTF(options)

  print "TRAINING DONE!"
