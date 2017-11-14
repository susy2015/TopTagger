#This is used to evaulate and compare batchRuns.
#We will rank the runs


import integrate as int
import pickle
import optparse
import os
import numpy as np
import operator
import json
import sys

def get_directories(a_dir):
   return [name for name in os.listdir(a_dir) if os.path.isdir(os.path.join(a_dir, name))]

parser = optparse.OptionParser()

parser.add_option('-d',"--directory",dest="dir",action="store",default=".",help="Base directory, we will look for subdirectories rooted here for roc pickles.")

options, _ = parser.parse_args()

scores = {}

runMaps = {}

for dir in get_directories(options.dir):
   
   try:
      file = open(os.path.join(options.dir,dir,"roc.pkl"),"rb")
   except IOError:
      continue

   PtCutMap = pickle.load(file)

   runMaps[dir] = PtCutMap

   for cut in sorted(PtCutMap.iteritems()):
      PtCutData = PtCutMap[cut[0]]
      TPR  = PtCutData["TPR"]
      FPR  = PtCutData["FPR"]
      FPRZ = PtCutData["FPRZ"]

      PtCutData["ttbarScore"] = int.intWindow(list(zip(FPR,TPR)))
      PtCutData["znunuScore"] = int.intWindow(list(zip(FPRZ,TPR)))


   file.close()

cutKeys = []

for key in runMaps.keys():
   for cutKey in runMaps[key].keys():
      if cutKey not in cutKeys: cutKeys.append(cutKey)

for cutKey in cutKeys:
   for key in runMaps.keys():
      scores[key] = runMaps[key][cutKey]["ttbarScore"]

   trials = sorted(scores.items(), key=operator.itemgetter(1))
   print cutKey
   for trial in trials: print trial[0], trial[1] 

print cutKeys

modules = list(set(sys.modules) & set(globals()))
for module_name in modules:
    module = sys.modules[module_name]
    print module_name, getattr(module, '__file__', 'unknown')

exit()

if True:


   scores[dir] = (ttbarScore,znunuScore)
 
ttbarList, znunuList =  zip(*list(scores.values()))

numTTbar = np.array(list(filter(lambda a: a != 0.,ttbarList)))
numZnunu = np.array(list(filter(lambda a: a != 0.,znunuList)))

ttbar_mu = np.mean(numTTbar)
ttbar_std = np.std(numTTbar)

znunu_mu = np.mean(numZnunu)
znunu_std = np.std(numZnunu)

ttbarDict = {x:(scores[x][0]-ttbar_mu)/ttbar_std for x in scores.keys() if scores[x][0] > 0.}
znunuDict = {x:(scores[x][1]-znunu_mu)/znunu_std for x in scores.keys() if scores[x][1] > 0.}

topTTBar, _ = zip(*sorted(ttbarDict.items(), key=operator.itemgetter(1))[-3:])
topZnunu, _ = zip(*sorted(znunuDict.items(), key=operator.itemgetter(1))[-3:])

bottomTTbar, _ = zip(*sorted(ttbarDict.items(), key=operator.itemgetter(1))[:1])
bottomZnunu, _ = zip(*sorted(znunuDict.items(), key=operator.itemgetter(1))[:1])

plotItems1 =  list(set(topTTBar).union(set(topZnunu)).union(set(bottomTTbar)).union(set(bottomZnunu)))
plotItems2 = sorted([(x,scores[x][0]) for x in plotItems1],key=operator.itemgetter(1),reverse=True)
plotItems, _ = list(zip(*plotItems2))

print plotItems

rocJSON = open(os.path.join(options.dir,"rocPlots.json"),"w")
print [x for (x,y) in sorted(ttbarDict.items(), key=operator.itemgetter(1),reverse=True)]
rocInfo = {"mlpTest":{"files":[os.path.abspath(os.path.join(options.dir,plotItem,"roc.pkl")) for plotItem in plotItems],"labels":[str([x for (x,y) in sorted(ttbarDict.items(), key=operator.itemgetter(1),reverse=True)].index(plotItem)+1)+": "+plotItem for plotItem in plotItems]}}

json.dump(rocInfo, rocJSON, sort_keys=True, indent=4)
