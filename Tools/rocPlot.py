import matplotlib.pyplot as plt
import pickle
import numpy as np

files =  ["roc_nom.pkl", "roc_csvSort.pkl", "roc.pkl"]
labels = ["Nominal",     "CSV Sort",        "Cand mass only"]
plt.clf()
ax = plt.figure()

for label, file in zip(labels, files):
    f1 = open(file, "rb")
    
    TPR = pickle.load(f1)
    FPR = pickle.load(f1)
    FPRZ = pickle.load(f1)
    
    plt.plot(FPR,TPR, label=label)

plt.legend(loc="lower right")
plt.xlabel("FPR (ttbar)")
plt.ylabel("TPR (ttbar)")
plt.savefig("roc_comp.png")
plt.close()

