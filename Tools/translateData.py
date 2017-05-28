import numpy as np
import pandas as pd
import ROOT
import optparse

parser = optparse.OptionParser("usage: %prog [options]\n")

parser.add_option ('-f', "--file", dest='fileNum', action='store', help="Input file")

options, args = parser.parse_args()

def main():
    datasetFile = options.fileNum
    dataset = ROOT.TFile.Open(datasetFile)

    data = []

    branchNames = [b.GetName() for b in dataset.slimmedTuple.GetListOfBranches()]
    #print branchNames
    evalStr = ""
    nevt  =0
    for event in dataset.slimmedTuple:
        if nevt % 1000 == 0:
            print nevt
        nevt += 1
        if evalStr == "":
            bnList = []
            bnListNoCand = []
            for bn in branchNames:
                try:
                    nbranch = eval("len(dataset.slimmedTuple.%(bname)s)"%{"bname":bn})
                except TypeError:
                    nbranch = -1
                if bn == "genTopPt":
                    continue
                if nbranch < 0:
                    bnList.append("\"%(bname)s\":event.%(bname)s"%{"bname":bn})
                    bnListNoCand.append("\"%(bname)s\":event.%(bname)s"%{"bname":bn})
                elif nbranch >= 0:
                    bnList.append("\"%(bname)s\":event.%(bname)s[%%(i)i]"%{"bname":bn})
                    bnListNoCand.append("\"%(bname)s\":0.0"%{"bname":bn})
            evalStr       = "{\"eventNum\":%(nevt)i, \"candNum\":%(i)i, \"ncand\":%(ncand)i, " + ",".join(bnList) + "}"
            evalStrNoCand = "{\"eventNum\":%(nevt)i, \"candNum\":%(i)i, \"ncand\":%(ncand)i, " + ",".join(bnListNoCand) + "}"
            #print evalStr
        ncands = len(dataset.slimmedTuple.cand_m)
        for i in xrange(0, ncands):
            data.append( eval(evalStr%{"i":i, "nevt":nevt, "ncand":ncands}) )
        if ncands == 0:
            data.append( eval(evalStrNoCand%{"i":0, "nevt":nevt, "ncand":ncands}) )

    pdData = pd.DataFrame(data)

    indices = [pdData.as_matrix(["eventNum"]).reshape([-1]), pdData.as_matrix(["candNum"]).reshape([-1])]
    
    fullBranchNames = np.hstack([["ncand"], branchNames] )
    pdData = pd.DataFrame(pdData.as_matrix(fullBranchNames), index=pd.MultiIndex.from_arrays(indices), columns=fullBranchNames)

    #pdData.to_csv(datasetFile[0:-5] + ".csv")
    pdData.to_pickle(datasetFile[0:-5] + ".pkl.gz")

if __name__ == '__main__':
    main()
