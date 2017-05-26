import matplotlib.pyplot as plt
import pickle
import numpy as np

files =  ["roc_rf_nominal.pkl", "roc_rf_csvSort.pkl", "roc_rf_csvSort_dRPt.pkl", "roc_rf_csvSort_dRPt_n2.pkl", "roc_rf_TeamAVars.pkl"]#, "roc_mlp_TeamAVars.pkl", "roc_mlp_nominal.pkl"]
labels = ["RF Nominal",         "RF CSV Sort",        "RF CSV Sort + dRPt",      "RF CSV Sort + dRPt + n2",    "RF Team A vars"]#,       "MLP Team A vars",       "MLP Nominal"]

colors = ["red", "blue", "green", "orange", "black", "purple", "yellow"]

plt.clf()
ax = plt.figure()

for label, file, color in zip(labels, files, colors):
    f1 = open(file, "rb")
    
    TPR = pickle.load(f1)
    FPR = pickle.load(f1)
    FPRZ = pickle.load(f1)

    TPRPtCut = pickle.load(f1)
    FPRPtCut = pickle.load(f1)
    FPRZPtCut = pickle.load(f1)

    plt.plot(FPR, TPR, label=label, color=color)
    plt.plot(FPRPtCut, TPRPtCut, label=label+" Pt cut", linestyle="dotted", color=color)

plt.legend(loc="lower right")
plt.xlabel("FPR (ttbar)")
plt.ylabel("TPR (ttbar)")
plt.savefig("roc_comp.png")
plt.close()

