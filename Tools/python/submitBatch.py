from subprocess import call
from taggerOptions import *
import optparse
import os
import errno
#from generic import *
import imp

qsubStr = "qsub -q gpu -l nodes=1:ppn=36 "

class SubmitJobs:

    def __init__(self, workdir, NGPU = 2):
        self.NGPU = NGPU
        self.workdir = workdir
        self.submitList = [[] for i in xrange(self.NGPU)]
        self.currentGPU = 0

    #We pass a taggerOptions file, the workdir, and a job name
    #This function will create the directory structure, the job, the pbs file, then submit the job
    def createJob(self, options,  workdir, jobName):
    
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
       shName = options.runOp.directory+jobName+".sh"   
    
       f = open(shName,"w+")
    
       pythonDir = (os.path.dirname(os.path.abspath(__file__)))
    
       f.write("export CUDA_DEVICE_ORDER=PCI_BUS_ID\n")
       f.write("export CUDA_VISIBLE_DEVICES=%i\n"%self.currentGPU)
       f.write("cd "+options.runOp.directory+"\n")
       f.write("source /home/hatake/tensorflow/setup.sh > log.log\n")
       f.write("python "+os.path.join(pythonDir,"Training.py")+" -c "+jobName+".json >> log.log\n")
       f.write("export CUDA_VISIBLE_DEVICES=\n")
       #f.write("python "+os.path.join(pythonDir,"Validation.py")+" -j "+options.saveName+" >> log.log\n")
    
       f.close()

       self.submitList[self.currentGPU].append({"shName":shName})
       self.currentGPU = (self.currentGPU + 1)%self.NGPU
    
    def __call__(self, options,  workdir, jobName):
        self.createJob(options,  workdir, jobName)

    def submitJobs(self):
        import itertools 
        os.chdir(self.workdir)
        for i, jobs in enumerate(itertools.izip_longest(*self.submitList)):
            #create pbs submit file
            fname = "%s/batchJob%i.pbs"%(self.workdir, i)
            with open(fname, "w") as f:
               for job in jobs:
                  if job != None:
                     print job["shName"]
                     f.write("bash %s &\n"%(job["shName"]))
               f.write("wait\n")
            #submit the job
            print qsubStr + fname
            call(qsubStr + fname, shell=True)


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

   #job submitter
   submitJob = SubmitJobs(workdir)

   #submitJob(baseOptions,workdir,options.prefix+"_testjob")
   batchCode.makeJobs(submitJob,saveOptionsToJSON,baseOptions,workdir)
   submitJob.submitJobs()
