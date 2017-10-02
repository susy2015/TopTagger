import matplotlib.pyplot as plt
import matplotlib.lines as mlines
import pickle
import json
import numpy as np
import errno
import os
import optparse

parser = optparse.OptionParser("usage: %prog [options]\n")
parser.add_option('-d', "--directory",  dest='directory',   action='store', default="",                 help="Directory to store outputs")
parser.add_option('-j', "--jsonFile",   dest='jsonFile',    action='store', default="rocPlots.json",    help="Json file defining files and labels.")

options, args = parser.parse_args()

outputDirectory = ""

if len(options.directory):
  outputDirectory = options.directory
  if outputDirectory[-1] != "/":
      outputDirectory += "/"
  try:
      os.mkdir(outputDirectory)
  except OSError as exc:
      if exc.errno == errno.EEXIST and os.path.isdir(outputDirectory):
          pass
      else:
          raise

print "LOADING JSON FILE"

jsonFile = options.jsonFile

with open(jsonFile, "r") as f:
    inputs = json.load(f)

colors = ["red", "blue", "green", "orange", "black", "purple", "yellow", "pink", "maroon"]

for name, filelist in inputs.iteritems():
    plt.clf()
    
    files = filelist["files"]
    labels = filelist["labels"]

    plotRoc = plt.figure()
    plotRocAx = plotRoc.add_subplot(111)
    plotRocZ = plt.figure()
    plotRocZAx = plotRocZ.add_subplot(111)

    rocs  = []
    rocsZ = []
    fileLines = []
    cutLines = []
    
    ifile = 0
    
    # colors per file
    for file1, label, color in zip(files, labels, colors):
        print "OPENING PICKLE; NAME: {0} FILE: {1}".format(name, file1)
        f1 = open(file1, "rb")
        PtCutMap = pickle.load(f1) 
        ncuts = len(PtCutMap)
        styles = list( (k, ((k+1), (k+1)) ) for k in xrange(ncuts) ) 
        if len(styles) > 0: styles[-1] = 'solid'
        if len(styles) > 1: styles[-2] = 'dashed'
        if len(styles) > 2: styles[-3] = 'dashdot'
        if len(styles) > 3: styles[-4] = 'dotted'
        
        icut = 0
        for cut in sorted(PtCutMap.iteritems()):
            # styles per cut
            style = styles[icut]
            print "File: {0} Label: {1} Color: {2} Style: {3} Cut: {4}".format(file1, label, color, style, cut[0])

            PtCutData = PtCutMap[cut[0]]
            TPRPtCut  = PtCutData["TPR"]  
            FPRPtCut  = PtCutData["FPR"] 
            FPRZPtCut = PtCutData["FPRZ"] 
            pt_min = PtCutData["PtMin"]
            pt_max = PtCutData["PtMax"]
            
            rocs.append(plotRocAx.plot(FPRPtCut,      TPRPtCut, label=label, linestyle=style, color=color, alpha=1.0)[0])
            rocsZ.append(plotRocZAx.plot(FPRZPtCut,   TPRPtCut, label=label, linestyle=style, color=color, alpha=1.0)[0])
            
            if ifile == 0:
                lineLabel = ""
                if pt_max > 0:
                    lineLabel = r'${0}$ GeV $< p_T < {1}$ GeV'.format(pt_min, pt_max)
                else:
                    lineLabel = r'$p_T > {0}$ GeV'.format(pt_min)
                cutLines.append(mlines.Line2D([], [], color='black', marker=None, linestyle=style, label=lineLabel))

            icut += 1

        fileLines.append(mlines.Line2D([], [], color=color, marker=None, linestyle='solid', label=label))
        ifile += 1

    first_legend = plotRocAx.legend(handles=fileLines, loc="lower right")
    plotRoc.gca().add_artist(first_legend)
    second_legend = plotRocAx.legend(handles=cutLines, loc=(0.2, 0.02))
    
    #plotRocAx.legend(loc="upper left")
    plotRocAx.set_xlabel("FPR (ttbar)")
    plotRocAx.set_ylabel("TPR (ttbar)")
    plotRocAx.set_title("ROC Plot for TPR (ttbar) vs FPR (ttbar)")
    plotRocAx.set_xlim(0.0, 0.5)
    plotRocAx.set_ylim(0.2, 1.0)
    #plotRocAx.xscale('log')
    #plotRocAx.yscale('log')
    plotRoc.savefig("roc_%s.png"%name)
    plotRoc.savefig("roc_%s.pdf"%name)
    plt.close(plotRoc)

    first_legend = plotRocZAx.legend(handles=fileLines, loc="lower right")
    plotRocZ.gca().add_artist(first_legend)
    second_legend = plotRocZAx.legend(handles=cutLines, loc=(0.2, 0.02))

    #plotRocZAx.legend(loc="lower right")
    plotRocZAx.set_xlabel("FPR (Znunu)")
    plotRocZAx.set_ylabel("TPR (ttbar)")
    plotRocZAx.set_title("ROC Plot for TPR (ttbar) vs FPR (Znunu)")
    plotRocZAx.set_xlim(0.0, 0.5)
    plotRocZAx.set_ylim(0.2, 1.0)
    plotRocZ.savefig("rocZ_%s.png"%name)
    plotRocZ.savefig("rocZ_%s.pdf"%name)
    plt.close(plotRocZ)
    
