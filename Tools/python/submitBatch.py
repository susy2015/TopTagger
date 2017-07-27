from subprocess import call
from taggerOptions import *
import optparse
import os
import errno
#from generic import *
import imp

qsubStr = "qsub -q gpu -l nodes=1:ppn=36 "

#We pass a taggerOptions file, the workdir, and a job name
#This function will create the directory structure, the job, the pbs file, then submit the job
def submitJob(options,  workdir, jobName):

   if not isinstance(options, taggerOptions):
      print "The submitJob method requires an object of the taggerOptions type"
      return

   if not os.path.isdir(workdir):
      print "Invalid work directory:", workdir
      return

   if not isinstance(jobName, basestring):
      print "The submitJob method requires a valid jobName"
      return

   #Let's make sure that we are working with absolute paths
   workdir = os.path.abspath(workdir)

   #If the directory for the jobs already exists, we won't continue with the job submission
   if workdir[-1] != "/": workdir += "/"
   jobDir = os.path.join(workdir,jobName)
   if jobDir[-1] != "/": jobDir += "/"

   try:
      os.mkdir(jobDir)
   except OSError as exc:
      if exc.errno == errno.EEXIST:
         print jobDir, "already exists.", jobName, "job will not be submitted"
         return
      else:
         raise

   options.confName = jobName
   options.runOp.directory = jobDir
   options.runOp.runName = jobName
   options.cleanUp()

   jsonOptionsPath = options.runOp.directory+jobName+".json"

   saveOptionsToJSON(options,jsonOptionsPath)

   #Let's make the pbs file
   pbsName = options.runOp.directory+jobName+".pbs"   

   f = open(pbsName,"w+")

   pythonDir = (os.path.dirname(os.path.abspath(__file__)))

   f.write("cd "+options.runOp.directory+"\n")
   f.write("source /home/hatake/tensorflow/setup.sh\n")
   f.write("python "+os.path.join(pythonDir,"Training.py")+" -c "+jobName+".json\n")
   f.write("python "+os.path.join(pythonDir,"Validation.py")+" -j "+options.saveName+"\n")

   f.close()

   #submit the job
   os.chdir(options.runOp.directory)
   call(qsubStr+pbsName, shell=True)
   os.chdir(pythonDir[:-6])
   print qsubStr+pbsName
   print "Submitted", jobName


if __name__ == '__main__':

   #Option parsing
   parser = optparse.OptionParser("usage: %prog [options]\n")

   parser.add_option('-d', "--workdir", dest = 'workdir', action = 'store', default = "batch",                     help = "Base directory for batch processing")
   parser.add_option('-f', "--datadir", dest = 'datadir', action = 'store', default = '/data3/pastika/trainData/', help = "Directory with training and validation n-tuples")
   parser.add_option('-c', "--cfgfile", dest = 'cfgfile', action = 'store',                                        help = "Example configfile from which to draw default values")
   parser.add_option('-p', "--prefix" , dest = 'prefix',  action = 'store', default = "batch",                     help = "Prefix to use in config identification")
   parser.add_option('-m', "--makejob", dest = 'makejob', action = 'store',                                        help = "Code to produce the jobs in the batch")

   options, _ = parser.parse_args()   

   #We need to load the makeJobs code
   batchCode = imp.load_source("batchCode",options.makejob)

   #First check whether the data directory exists. If it doesn't let's abort now, it doesn't make sense to make the batch structure if the jobs are just going to fail.
   #We don't check at this point if the directory actually has data that we can use.
   if not os.path.isdir(options.datadir):
      print options.datadir,"is not a directory. Aborting batch submission"
      exit()

   #let's create the directory for this batch job. If it already exists, abort the job. We don't want to overwrite anything
   if options.workdir[-1] != "/": options.workdir += "/"
   try:
      os.mkdir(options.workdir)
   except OSError as exc:
      if exc.errno == errno.EEXIST:
         print options.workdir, "already exists. Please provide another directory for batch submission. Aborting."
         exit()
      else:
         raise

   #Let's save the batchJob code for posterity
   call("cp "+options.makejob+" "+options.workdir,shell=True)

   #Now let us set up the directory structure

   #First we want the path to the workdirectory and data directory, this will make it easier to work in the various nested directories
   workdir = os.path.abspath(options.workdir)
   datadir = os.path.abspath(options.datadir)

   print "Work directory:", workdir
   print "Data directory:", datadir

   #Now we set up an default tagger option file
   if isinstance(options.cfgfile, basestring):
      baseOptions = taggerOptions.loadJSON(option.cfgfile)
   else:
      baseOptions = taggerOptions.defaults()

   baseOptions.info.append("batch prefix: "+options.prefix)
   baseOptions.confName = options.prefix

   baseOptions.runOp.dataPath = datadir
   baseOptions.cleanUp()

   #submitJob(baseOptions,workdir,options.prefix+"_testjob")
   batchCode.makeJobs(submitJob,saveOptionsToJSON,baseOptions,workdir)
