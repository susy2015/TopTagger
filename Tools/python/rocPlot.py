import matplotlib.pyplot as plt
import matplotlib.lines as mlines
import pickle
import json
import numpy as np

with open("rocPlots.json", "r") as f:
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

    rocs = []
    rocsPt = []

    rocsZ = []
    rocsPtZ = []

    for label, file, color in zip(labels, files, colors):
        f1 = open(file, "rb")
        
        TPR = pickle.load(f1)
        FPR = pickle.load(f1)
        FPRZ = pickle.load(f1)
    
        TPRPtCut = pickle.load(f1)
        FPRPtCut = pickle.load(f1)
        FPRZPtCut = pickle.load(f1)
    
        rocs.append( plotRocAx.plot(FPR, TPR, label=label, color=color, alpha=1.0)[0])
        rocsPt.append(plotRocAx.plot(FPRPtCut, TPRPtCut, label=label+" Pt > 200 GeV", linestyle="dotted", color=color, alpha=1.0)[0])

        rocsZ.append(plotRocZAx.plot(FPRZ, TPR, label=label, color=color, alpha=1.0)[0])
        rocsPtZ.append(plotRocZAx.plot(FPRZPtCut, TPRPtCut, label=label+" Pt > 200 GeV", linestyle="dotted", color=color, alpha=1.0)[0])

    
    first_legend = plotRocAx.legend(handles=rocs, loc="lower right")
    plotRoc.gca().add_artist(first_legend)

    solidLine = mlines.Line2D([], [], color='black', marker=None, label='All events')
    dottedLine = mlines.Line2D([], [], color='black', marker=None, linestyle='dotted', label=r'$p_{T} > 200$ GeV')
    second_legend = plotRocAx.legend(handles=[solidLine, dottedLine], loc=(0.2, 0.02))

    #plotRocAx.legend(loc="upper left")
    plotRocAx.set_xlabel("FPR (ttbar)")
    plotRocAx.set_ylabel("TPR (ttbar)")
    #plotRocAx.xscale('log')
    #plotRocAx.yscale('log')
    plotRoc.savefig("roc_%s.png"%name)
    plotRoc.savefig("roc_%s.pdf"%name)
    plt.close(plotRoc)

    first_legend = plotRocZAx.legend(handles=rocsZ, loc="lower right")
    plotRocZ.gca().add_artist(first_legend)

    second_legend = plotRocZAx.legend(handles=[solidLine, dottedLine], loc=(0.2, 0.02))

    #plotRocZAx.legend(loc="lower right")
    plotRocZAx.set_xlabel("FPR (Znunu)")
    plotRocZAx.set_ylabel("TPR (ttbar)")
    plotRocZ.savefig("rocZ_%s.png"%name)
    plotRocZ.savefig("rocZ_%s.pdf"%name)
    plt.close(plotRocZ)
    
