#This is an example fragment. Code like this will be imported by submitBatch.py in order to make the jobs that will be run


def makeJobs(submitJob,saveJSON,options,workdir):


   #This is simple test code
   
   options.netOp.convLayers = []
   options.netOp.rnnLayers = 0
   
   options.runOp.nepoch = 2

   options.confName = "Batch Job test"

   options.netOp.denseLayers = [50]

   options.netOp.loadStandardVariables("TeamAlpha1DConv")

   options.cleanUp()

   if workdir[-1] != "/": workdir += "/"

   saveJSON(options,workdir+"test.json")

   for x in [20,40,60]:
      options.netOp.denseLayers = [x]
      options.netOp.networkName = "Test MLP: "+str(x)
      options.confName = "Test MLP: "+str(x)
  
      options.cleanUp()
      submitJob(options,workdir, "testMLP_"+str(x))
