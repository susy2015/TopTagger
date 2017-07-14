import os
import errno
import optparse
from MVATrainers import mainSKL, mainXGB, mainTF

if __name__ == '__main__':

  #Option parsing 
  parser = optparse.OptionParser("usage: %prog [options]\n")

  parser.add_option ('-k', "--sklearnrf",         dest='sklearnrf',         action='store_true',                                  help="Run using sklearn RF")
  parser.add_option ('-x', "--xgboost",           dest='xgboost',           action='store_true',                                  help="Run using xgboost")
  parser.add_option ('-d', "--directory",         dest='directory',         action='store',      default="",                      help="Directory to store outputs (default .)")
  parser.add_option ('-v', "--variables",         dest='variables',         action='store',      default="TeamAlpha",             help="Input features to use (default TeamAlpha)")
  parser.add_option ('-e', "--nepoch",            dest='nepoch',            action='store',      default=100,         type="int", help="Number of training epoch (default 100)")
  parser.add_option ('-n', "--nReaders",          dest="nReaders",          action='store',      default=4,           type="int", help="Number of file readers to use (default 4)")
  parser.add_option ('-q', "--nThreadperReader",  dest="nThreadperReader",  action='store',      default=1,           type="int", help="Number of threads for each flie reader (default 1)")
  parser.add_option ('-p', "--ptReweight",        dest="ptReweight",        action='store_true',                                  help="Reweight pt spectrum of events durring training")
  parser.add_option ('-m', "--modelCfg",          dest="modelJSON",         action='store',      default="model.json",            help="JSON with model definitions")

  options, args = parser.parse_args()

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
