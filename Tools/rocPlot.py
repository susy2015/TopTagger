import matplotlib.pyplot as plt
import pickle
import numpy as np

#files =  ["roc_rf_nominal.pkl", "roc_rf_csvSort.pkl", "roc_rf_csvSort_dRPt.pkl", "roc_rf_csvSort_dRPt_n2.pkl", "roc_rf_TeamAVars.pkl"]#, "roc_mlp_TeamAVars.pkl", "roc_mlp_nominal.pkl"]
#labels = ["RF Nominal",         "RF CSV Sort",        "RF CSV Sort + dRPt",      "RF CSV Sort + dRPt + n2",    "RF Team A vars"]#,       "MLP Team A vars",       "MLP Nominal"]

#files =  ["roc_rf_nominal.pkl", "roc_rf_csvSort_dRPt_n2.pkl", "roc_rf_TeamAVars.pkl", "roc_mlp_nominal.pkl", "roc_mlp_csvSort_dRPt_n2.pkl", "roc_mlp_TeamAVars.pkl"]
#labels = ["RF Nominal",         "RF CSV Sort + dRPt + n2",    "RF Team A vars",       "MLP Nominal",         "MLP CSV Sort + dRPt + n2",    "MLP Team A vars"]

#files =  ["roc_mlp_nominal.pkl", "roc_mlp_nominal_2.pkl", "roc_mlp_nominal_3.pkl"]
#labels = ["MLP Nominal 1",       "MLP Nominal 2",         "MLP Nominal 3"]

#files =  ["roc_mlp_TeamAVars.pkl", "roc_mlp_TeamAVars_2.pkl", "roc_mlp_TeamAVars_3.pkl"]
#labels = ["MLP Team A 1",         "MLP Team A 2", "MLP TeamA 3"]

#files =  ["roc_mlp_nominal_dRVars.pkl", "roc_mlp_nominal_dRVars_2.pkl", "roc_mlp_nominal_dRVars_3.pkl", "roc_mlp_nominal_dRVars_4.pkl"]
#labels = ["MLP Nom + dR Vars",          "MLP Nom + dR Vars 2",          "MLP Nom + dR Vars 3",          "MLP Nom + dR Vars 4"]

#files =  ["roc_mlp_nominal_2.pkl", "roc_mlp_TeamAVars_3.pkl", "roc_mlp_nominal_dRVars_3.pkl", "roc.pkl"]
#labels = ["MLP Nominal 2",         "MLP TeamA 3",             "MPL Nominal + dR Vars 3",      "New"]

inputs = {"rf_2bseed": {"files":  ["RF_TeamAlpha_2BSelection/roc.pkl", "RF_TeamA_2BSelection/roc.pkl", "RF_Mixed_2BSelection/roc.pkl"],
                        "labels": ["Team Alpha",                       "Team A",                  "Mixed",                     ],
                    },
#          "xgb_2bseed": {"files":  ["roc_xgb_2bseed_TeamAlpha.pkl", "roc_xgb_2bseed_TeamA.pkl"],# "roc_xgb_2bseed_TeamA_noPtWgt.pkl", "roc_xgb_2bseed_TeamAlpha_noPtWgt.pkl"],
#                         "labels": ["Team Alpha",                   "Team A",                 ],#  "Team Alpha no pT wgt",             "Team A no pT wgt"]
#                     },
          "MVAcomp": {"files":  ["XGB_TeamAlpha_Nominal/roc.pkl", "RF_TeamAlpha_Nominal/roc.pkl",   "MLP_TeamAlpha_Nominal/roc.pkl"],
                      "labels": ["xgboot",      "random forest", "Multi-layer perceptron"]
                  },
          "MVAcompWithZ": {"files":  ["XGB_TeamAlpha_NominalWithZ/roc.pkl", "RF_TeamAlpha_NominalWithZ/roc.pkl",   "MLP_TeamAlpha_NominalWithZ/roc.pkl"],
                           "labels": ["xgboot",      "random forest", "Multi-layer perceptron"]
                  },
          "rf_nomSel": {"files":  ["RF_TeamAlpha_Nominal/roc.pkl","RF_TeamA_Nominal/roc.pkl", "RF_Mixed_Nominal/roc.pkl"],
                        "labels": ["Team Alpha",  "Team A",         "Mixed Variables"]
                    },
          "rf_moreQGL": {"files":  ["RF_TeamAlpha_Nominal/roc.pkl", "RF_TeamA_Nominal/roc.pkl", "RF_TeamAlphaMoreQGL_Nominal/roc.pkl", "RF_TeamAMoreQGL_Nominal/roc.pkl"],
                         "labels": ["Team Alpha", "Team A",   "Alpha + QGL Vars",                   "A + QGL vars"],
                    },
          "rf_withZ": {"files":  ["RF_TeamAlpha_Nominal/roc.pkl", "RF_TeamA_Nominal/roc.pkl", "RF_TeamAlpha_NominalWithZ/roc.pkl", "RF_TeamA_NominalWithZ/roc.pkl"],
                       "labels": ["Team Alpha top only", "Team A top only",                  "Team Alpha top + Z",   "Team A top + Z"]
                    },
          "rf_teamAsel": {"files":  ["RF_TeamAlpha_TeamASel/roc.pkl", "RF_TeamA_TeamASel/roc.pkl", "RF_Mixed_TeamASel/roc.pkl"],
                          "labels": ["Team Alpha",                       "Team A",                  "Mixed",                     ],
                    },
          "rf_pt20NoCone": {"files":  ["RF_TeamAlpha_pt20NoCone/roc.pkl", "RF_TeamA_pt20NoCone/roc.pkl"],
                          "labels": ["Team Alpha",                       "Team A"]
                    },
          "rfvsmlp_pt20NoCone": {"files":  ["RF_TeamAlpha_pt20NoCone/roc.pkl", "MLP_TeamAlpha_pt20NoCone/roc.pkl", "MLP_TeamAlphaWithCandPt_pt20NoCone/roc.pkl"],
                                 "labels": ["Random Forest",                   "Multi-layer perceptron",           "MLP with candidate pt"]
                    },
#          "mlp_comp": {"files":  ["MLP_TeamAlpha_100_50_50_1M/roc.pkl", "MLP_TeamAlphaWithCandPt_100_50_50_1M/roc.pkl", "MLP_TeamAlphaMoreQGL_100_50_50_1M/roc.pkl", "MLP_TeamAlpha_50_50_1M/roc.pkl", "MLP_Mixed_100_50_50_1M/roc.pkl", "MLP_MixedWithCandPt_100_50_50_1M/roc.pkl", "MLP_MixedWithCandPtAndMoreQGL_100_50_50_1M/roc.pkl"],
#                       "labels": ["Team Alpha", "Team Alpha + cand pt", "Team Alpha + More QGL", "Team Alpha 50,50", "Mixed", "Mixed + cand Pt", "Mixed + cand pt + more QGL"]
#                    },
#          "mlp_comp": {"files":  ["RF_TeamAlphaMoreQGL_Nominal/roc.pkl", "MLP_TeamAlphaMoreQGL_100_50_50_1M/roc.pkl", "MLP_TeamAlphaWithCandPtAndMoreQGL_100_50_50_1M/roc.pkl", "MLP_TeamAlphaMoreQGL_100_50_50_1MWithZ/roc.pkl", "MLP_TeamAlphaWithCandPtAndMoreQGL_100_50_50_1MWithZ/roc.pkl", "MLP_noPtWgt_TeamAlphaMoreQGL_100_50_50_1M/roc.pkl", "MLP_TeamAlphaWithCandPtAndMoreQGL_100_50_50_1M/roc.pkl"],
#                       "labels": ["RF More QGL", "More QGL", "Cand pt + More QGL", "More QGL With Z", "cand pt + More QGL With Z", "More QGL no wgt", "Cand pt + More QGL no wgt"]
#                    },
          "mlp_comp": {"files":  ["RF_TeamAlpha_Nominal/roc.pkl", "RF_TeamAlphaMoreQGL/roc.pkl", "RF_TeamAlphaMoreQGLCandPt/roc.pkl", "MLP_TeamAlphaMoreQGLCandPt_EPOCH_100/roc.pkl", "MLP_TeamAMoreQGLCandPt_EPOCH_100_RSQ/roc.pkl", "MLP_MixedMoreQGLCandPt_EPOCH_50_RSQ/roc.pkl"],
                       "labels": ["RF Alpha", "RF Alpha + QGL inputs", "RF Alpha + QGL inputs + Cand Pt", "MLP Alpha + QGL inputs + Cand Pt", "MLP A + QGL inputs + Cand Pt", "MLP Mixed + QGL inputs + Cand Pt"]
                    },
          "mlp_nepochs": {"files":  ["MLP_TeamAlphaMoreQGLCandPt_EPOCH_1/roc.pkl", "MLP_TeamAlphaMoreQGLCandPt_EPOCH_10/roc.pkl", "MLP_TeamAlphaMoreQGLCandPt_EPOCH_20/roc.pkl", "MLP_TeamAlphaMoreQGLCandPt_EPOCH_50/roc.pkl", "MLP_TeamAlphaMoreQGLCandPt_EPOCH_100/roc.pkl"],#, "MLP_TeamAlphaMoreQGLCandPt_EPOCH_50_RSQ/roc.pkl"],
                          "labels": ["1 Epoch", "10 Epoch", "20 Epoch", "50 Epoch", "100 Epoch"]#, "50 Epoch RSQ"]
                    },
          "mlp_test": {"files":  ["RF_TeamAlphaMoreQGLCandPt/roc.pkl", "TEST/roc.pkl", "TEST2/roc.pkl", "TEST3/roc.pkl", "TEST4/roc.pkl", "TEST5/roc.pkl", "TEST6/roc.pkl", "TEST7/roc.pkl"],
                       "labels": ["RF Alpha + More QGL + cand Pt", "TEST", "TEST2", "TEST3", "TEST4", "TEST5", "TEST6", "TEST7"]
                    },
          

}
          

colors = ["red", "blue", "green", "orange", "black", "purple", "yellow"]

for name, filelist in inputs.iteritems():
    plt.clf()
    ax = plt.figure()
    
    files = filelist["files"]
    labels = filelist["labels"]

    for label, file, color in zip(labels, files, colors):
        f1 = open(file, "rb")
        
        TPR = pickle.load(f1)
        FPR = pickle.load(f1)
        FPRZ = pickle.load(f1)
    
        TPRPtCut = pickle.load(f1)
        FPRPtCut = pickle.load(f1)
        FPRZPtCut = pickle.load(f1)
    
        plt.plot(FPR, TPR, label=label, color=color, alpha=1.0)
        plt.plot(FPRPtCut, TPRPtCut, label=label+" Pt > 200 GeV", linestyle="dotted", color=color, alpha=1.0)
    
    plt.legend(loc="lower right")
    plt.xlabel("FPR (ttbar)")
    plt.ylabel("TPR (ttbar)")
    plt.savefig("roc_%s.png"%name)
    plt.savefig("roc_%s.pdf"%name)
    plt.close()

    plt.clf()
    ax = plt.figure()
    
    files = filelist["files"]
    labels = filelist["labels"]

    for label, file, color in zip(labels, files, colors):
        f1 = open(file, "rb")
        
        TPR = pickle.load(f1)
        FPR = pickle.load(f1)
        FPRZ = pickle.load(f1)
    
        TPRPtCut = pickle.load(f1)
        FPRPtCut = pickle.load(f1)
        FPRZPtCut = pickle.load(f1)
    
        plt.plot(FPRZ, TPR, label=label, color=color, alpha=1.0)
        plt.plot(FPRZPtCut, TPRPtCut, label=label+" Pt > 200 GeV", linestyle="dotted", color=color, alpha=1.0)
    
    plt.legend(loc="lower right")
    plt.xlabel("FPR (Znunu)")
    plt.ylabel("TPR (ttbar)")
    plt.savefig("rocZ_%s.png"%name)
    plt.savefig("rocZ_%s.pdf"%name)
    plt.close()
    
