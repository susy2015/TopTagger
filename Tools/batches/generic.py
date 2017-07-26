#This is an example fragment. Code like this will be imported by submitBatch.py in order to make the jobs that will be run


def makeJobs(submitJob,saveJSON,options,workdir):


   #This is a simple example, it will test a simple MLP with a single hidden layer with a variable number of nodes
   
   options.netOp.convLayers = []
   options.netOp.rnnLayers = 0

   options.netOp.networkName = "Simple MLP"
   options.confName = "Simple MLP"
   options.runOp.runName = "Simple MLP"

   options.netOp.denseLayers = [50]

   if workdir[-1] != "/": workdir += "/"

   saveJSON(options,workdir+"generic.json")

   for x in [x for x in range(20,500) if x % 50 == 0]:
      for y in [y for y in range(20,min(x,200)) if y % 40 == 0]:
         for z in [z for z in range(15,min(y,100)) if z % 30 == 0]:
            options.netOp.denseLayers = [x,y,z]
            options.netOp.networkName = "Simple MLP: "+str(x)+", "+str(y)+", "+str(z)
            options.netOp.runName = "Simple MLP: "+str(x)+", "+str(y)+", "+str(z)
            options.confName = "Simple MLP: "+str(x)+", "+str(y)+", "+str(z)
   
            options.cleanUp()
            submitJob(options,workdir, "simpleMLP_"+str(x)+"_"+str(y)+"_"+str(z))
