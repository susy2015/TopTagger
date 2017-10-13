import matplotlib.pyplot as plt
import matplotlib.lines as mlines
import pickle
import json
import numpy as np
import errno
import os
import optparse
from datetime import datetime

class Plotter:
    def __init__(self):
        self.outputDirectory = ""
        self.jsonFile = "rocPlots.json"
        self.colors = ["red", "blue", "green", "orange", "black", "purple", "yellow", "pink", "maroon", "xkcd:sky blue", "xkcd:violet", "xkcd:cerulean",
                       "xkcd:light red", "xkcd:sea blue", "xkcd:emerald", "xkcd:reddish purple", "xkcd:dark rose", "xkcd:aubergine", "xkcd:teal green", "xkcd:avocado"]


def main():
    parser = optparse.OptionParser("usage: %prog [options]\n")
    parser.add_option('-d', "--directory",   dest='directory',   action='store',      default="",               help="Directory to store outputs")
    parser.add_option('-j', "--jsonFile",    dest='jsonFile',    action='store',      default="rocPlots.json",  help="Json file defining files and labels")
    parser.add_option('-m', "--models",      dest='models',      action='store_true', default=False,            help="Split ROC curves into one plot per model")
    parser.add_option('-c', "--cuts",        dest='cuts',        action='store_true', default=False,            help="Split ROC curves into one plot per pt cut")
    
    options, args = parser.parse_args()

    p = Plotter()
    
    if len(options.directory):
      p.outputDirectory = options.directory
      if p.outputDirectory[-1] != "/":
          p.outputDirectory += "/"
      try:
          os.mkdir(p.outputDirectory)
      except OSError as exc:
          if exc.errno == errno.EEXIST and os.path.isdir(p.outputDirectory):
              pass
          else:
              raise
    
    print "LOADING JSON FILE"
    
    p.jsonFile = options.jsonFile
    
    with open(p.jsonFile, "r") as f:
        p.inputs = json.load(f)

    models = options.models
    cuts = options.cuts

    if models:
        # make one plot per model, multiple cuts per plot
        makeModelPlots(p) 
    elif cuts:
        # make one plot per cut, multiple models per plot
        makeCutPlots(p)
    else:
        makeCombinedPlots(p)

# make one plot per model with multiple pt cuts per plot
def makeModelPlots(plotter):
    print "MAKING MODEL PLOTS"
    p = plotter
    style = "solid"
    
    # get files
    for name, filelist in p.inputs.iteritems():
        files = filelist["files"]
        labels = filelist["labels"]
        
        # plot p_t cuts per file
        for file1, label in zip(files, labels):
            print "OPENING PICKLE; NAME: {0} FILE: {1}".format(name, file1)
            f1 = open(file1, "rb")
            PtCutMap = pickle.load(f1) 

            plotRoc  = plt.figure()
            plotRocZ = plt.figure()
            plotRocAx  = plotRoc.add_subplot(111)
            plotRocZAx = plotRocZ.add_subplot(111)
            rocs  = []
            rocsZ = []

            icut = 0
            for cut in sorted(PtCutMap.iteritems()):
                color = p.colors[icut]
                print "File: {0} Label: {1} Color: {2} Cut: {3}".format(file1, label, color, cut[0])
                PtCutData = PtCutMap[cut[0]]
                TPRPtCut  = PtCutData["TPR"]  
                FPRPtCut  = PtCutData["FPR"] 
                FPRZPtCut = PtCutData["FPRZ"] 
                pt_min = PtCutData["PtMin"]
                pt_max = PtCutData["PtMax"]
                
                lineLabel = ""
                if pt_max > 0:
                    lineLabel = r'${0}$ GeV $< p_T < {1}$ GeV'.format(pt_min, pt_max)
                else:
                    lineLabel = r'$p_T > {0}$ GeV'.format(pt_min)
                
                rocs.append(plotRocAx.plot(FPRPtCut,      TPRPtCut, label=lineLabel, linestyle=style, color=color, alpha=1.0)[0])
                rocsZ.append(plotRocZAx.plot(FPRZPtCut,   TPRPtCut, label=lineLabel, linestyle=style, color=color, alpha=1.0)[0])
                icut += 1
        
            # crate plot for each file
            first_legend = plotRocAx.legend(handles=rocs, loc="lower right")
            plotRoc.gca().add_artist(first_legend)
            
            plotRocAx.set_xlabel("FPR (ttbar)")
            plotRocAx.set_ylabel("TPR (ttbar)")
            plotRocAx.set_title("ROC Plot for TPR (ttbar) vs FPR (ttbar) "+label)
            plotRocAx.set_xlim(0.0, 0.5)
            plotRocAx.set_ylim(0.2, 1.0)
            plotRoc.savefig("{0}roc_{1}_{2}.png".format(p.outputDirectory, name, label))
            plotRoc.savefig("{0}roc_{1}_{2}.pdf".format(p.outputDirectory, name, label))
            plt.close(plotRoc)
    
            first_legend = plotRocZAx.legend(handles=rocsZ, loc="lower right")
            plotRocZ.gca().add_artist(first_legend)
    
            plotRocZAx.set_xlabel("FPR (Znunu)")
            plotRocZAx.set_ylabel("TPR (ttbar)")
            plotRocZAx.set_title("ROC Plot for TPR (ttbar) vs FPR (Znunu) "+label)
            plotRocZAx.set_xlim(0.0, 0.5)
            plotRocZAx.set_ylim(0.2, 1.0)
            plotRocZ.savefig("{0}rocZ_{1}_{2}.png".format(p.outputDirectory, name, label))
            plotRocZ.savefig("{0}rocZ_{1}_{2}.pdf".format(p.outputDirectory, name, label))
            plt.close(plotRocZ)

# make one plot per pt cut with multiple models per plot
def makeCutPlots(plotter):
    print "MAKING PT CUT PLOTS"
    p = plotter
    style = "solid"

    # get files
    for name, filelist in p.inputs.iteritems():
        files = filelist["files"]
        labels = filelist["labels"]
       
        PtCutList = []
        # get p_t cut maps from files
        for file1 in files:
            print "OPENING PICKLE; NAME: {0} FILE: {1}".format(name, file1)
            f1 = open(file1, "rb")
            PtCutMap = pickle.load(f1) 
            PtCutList.append(PtCutMap)
        
        # plot p_t cuts per file
        for cut in sorted(PtCutList[0].iteritems()):
            plotRoc  = plt.figure()
            plotRocZ = plt.figure()
            plotRocAx  = plotRoc.add_subplot(111)
            plotRocZAx = plotRocZ.add_subplot(111)
            rocs  = []
            rocsZ = []
            ifile = 0
            for file1, label in zip(files, labels):
                PtCutMap = PtCutList[ifile]
                color = p.colors[ifile]
                print "File: {0} Label: {1} Color: {2} Cut: {3}".format(file1, label, color, cut[0])
                PtCutData = PtCutMap[cut[0]]
                TPRPtCut  = PtCutData["TPR"]  
                FPRPtCut  = PtCutData["FPR"] 
                FPRZPtCut = PtCutData["FPRZ"] 
                pt_min = PtCutData["PtMin"]
                pt_max = PtCutData["PtMax"]
                
                rocs.append(plotRocAx.plot(FPRPtCut,      TPRPtCut, label=label, linestyle=style, color=color, alpha=1.0)[0])
                rocsZ.append(plotRocZAx.plot(FPRZPtCut,   TPRPtCut, label=label, linestyle=style, color=color, alpha=1.0)[0])
                ifile += 1
            
            fileLabel = cut[0]
            plotLabel = ""
            if pt_max > 0:
                plotLabel = r'${0}$ GeV $< p_T < {1}$ GeV'.format(pt_min, pt_max)
            else:
                plotLabel = r'$p_T > {0}$ GeV'.format(pt_min)
            
            # crate plot for each cut
            first_legend = plotRocAx.legend(handles=rocs, loc="lower right")
            plotRoc.gca().add_artist(first_legend)
            
            plotRocAx.set_xlabel("FPR (ttbar)")
            plotRocAx.set_ylabel("TPR (ttbar)")
            plotRocAx.set_title("ROC Plot for TPR (ttbar) vs FPR (ttbar) "+plotLabel)
            plotRocAx.set_xlim(0.0, 0.5)
            plotRocAx.set_ylim(0.2, 1.0)
            plotRoc.savefig("{0}roc_{1}_{2}.png".format(p.outputDirectory, name, fileLabel))
            plotRoc.savefig("{0}roc_{1}_{2}.pdf".format(p.outputDirectory, name, fileLabel))
            plt.close(plotRoc)
    
            first_legend = plotRocZAx.legend(handles=rocsZ, loc="lower right")
            plotRocZ.gca().add_artist(first_legend)
    
            plotRocZAx.set_xlabel("FPR (Znunu)")
            plotRocZAx.set_ylabel("TPR (ttbar)")
            plotRocZAx.set_title("ROC Plot for TPR (ttbar) vs FPR (Znunu) "+plotLabel)
            plotRocZAx.set_xlim(0.0, 0.5)
            plotRocZAx.set_ylim(0.2, 1.0)
            plotRocZ.savefig("{0}rocZ_{1}_{2}.png".format(p.outputDirectory, name, fileLabel))
            plotRocZ.savefig("{0}rocZ_{1}_{2}.pdf".format(p.outputDirectory, name, fileLabel))
            plt.close(plotRocZ)

# make one plot with multiple models and cuts combined in the same plot
def makeCombinedPlots(plotter):
    print "MAKING COMBINED PLOTS"
    p = plotter
    iname = 0
    for name, filelist in p.inputs.iteritems():
        print "BEGIN plt.clf()"
        t_a = datetime.now()
        plt.clf()
        t_b = datetime.now()
        print "END plt.clf()"
        print "Time for plt.clf(): {0}".format(t_b - t_a)
        
        files = filelist["files"]
        labels = filelist["labels"]
        
        plotRoc  = plt.figure()
        plotRocZ = plt.figure()
        plotRocAx  = plotRoc.add_subplot(111)
        plotRocZAx = plotRocZ.add_subplot(111)
    
        rocs  = []
        rocsZ = []
        fileLines = []
        cutLines = []
        
        ifile = 0
        
        # colors per file
        for file1, label, color in zip(files, labels, p.colors):
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
        plotRoc.savefig("{0}roc_{1}.png".format(p.outputDirectory, name))
        plotRoc.savefig("{0}roc_{1}.pdf".format(p.outputDirectory, name))
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
        plotRocZ.savefig("{0}rocZ_{1}.png".format(p.outputDirectory, name))
        plotRocZ.savefig("{0}rocZ_{1}.pdf".format(p.outputDirectory, name))
        plt.close(plotRocZ)
        
        iname += 1
        
if __name__ == "__main__":
    main()
