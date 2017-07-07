import numpy as np
import pandas as pd
import ROOT
import optparse
import tensorflow as tf


parser = optparse.OptionParser("usage: %prog [options]\n")

parser.add_option ('-f', "--file",      dest='fileNum', action='store',      help="Input file")
parser.add_option ('-g', "--genPtOnly", dest='gptOnly', action='store_true', help="Only produce the gen pt file")
parser.add_option ('-p', "--pandas",    dest='pandas',  action='store_true', help="Make pandas files")
parser.add_option ('-d', "--hdf5",      dest='hdf5',    action='store_true', help="Save output as hdf5")
parser.add_option ('-e', "--events",    dest='events',  action='store',      type=int, default="-1", help="Save output as hdf5")

options, args = parser.parse_args()

def _float_feature(value):
    return tf.train.Feature(float_list=tf.train.FloatList(value=[value]))

def mainPDF():
    datasetFile = options.fileNum
    dataset = ROOT.TFile.Open(datasetFile)

    data = []
    genData = []

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
        if not options.gptOnly:
            for i in xrange(0, ncands):
                data.append( eval(evalStr%{"i":i, "nevt":nevt, "ncand":ncands}) )
            if ncands == 0:
                data.append( eval(evalStrNoCand%{"i":0, "nevt":nevt, "ncand":ncands}) )
        
        for i in xrange(0, len(event.genTopPt)):
            genData.append({"eventNum":nevt, "candNum":i, "genTopPt":event.genTopPt[i], "sampleWgt":event.sampleWgt, "Njet":event.Njet})

    if not options.gptOnly:
        pdData = pd.DataFrame(data)

        indices = [pdData.as_matrix(["eventNum"]).reshape([-1]), pdData.as_matrix(["candNum"]).reshape([-1])]
    
        fullBranchNames = np.hstack([["ncand"], branchNames] )
        pdData = pd.DataFrame(pdData.as_matrix(fullBranchNames), index=pd.MultiIndex.from_arrays(indices), columns=fullBranchNames)

        #pdData.to_csv(datasetFile[0:-5] + ".csv")
        pdData.to_pickle(datasetFile[0:-5] + ".pkl.gz")

    pdGenData = pd.DataFrame(genData)

    genIndices = [pdGenData.as_matrix(["eventNum"]).reshape([-1]), pdGenData.as_matrix(["candNum"]).reshape([-1])]

    fullBranchNames = ["genTopPt", "sampleWgt", "Njet"]
    pdGenData = pd.DataFrame(pdGenData.as_matrix(fullBranchNames), index=pd.MultiIndex.from_arrays(genIndices), columns=fullBranchNames)
    pdGenData.to_pickle(datasetFile[0:-5] + "_gen.pkl.gz")


def mainHDF5():
    import h5py

    datasetFile = options.fileNum
    dataset = ROOT.TFile.Open(datasetFile)

    data = []
    genData = []

    branchNames = [b.GetName() for b in dataset.slimmedTuple.GetListOfBranches()]
    branchNamesUsed = []
    evalStr = ""
    nevt  =0
    iFile = 0
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
                branchNamesUsed.append(bn)
                if nbranch < 0:
                    bnList.append("event.%(bname)s"%{"bname":bn})
                    bnListNoCand.append("event.%(bname)s"%{"bname":bn})
                elif nbranch >= 0:
                    bnList.append("event.%(bname)s[%%(i)i]"%{"bname":bn})
                    bnListNoCand.append("0.0"%{"bname":bn})
            evalStr       = "[%(nevt)i, %(i)i, %(ncand)i, " + ",".join(bnList) + "]"
            evalStrNoCand = "[%(nevt)i, %(i)i, %(ncand)i, " + ",".join(bnListNoCand) + "]"

        ncands = len(dataset.slimmedTuple.cand_m)
        for i in xrange(0, ncands):
            data.append( eval(evalStr%{"i":i, "nevt":nevt, "ncand":ncands}) )
        if ncands == 0:
            data.append( eval(evalStrNoCand%{"i":0, "nevt":nevt, "ncand":ncands}) )
        
        for i in xrange(0, len(event.genTopPt)):
            genData.append([nevt, i, event.genTopPt[i], event.sampleWgt, event.Njet])

        if options.events > 0 and not nevt%options.events:
            f = h5py.File(datasetFile[0:-5] + "_%i.h5"%iFile, "w")
            iFile += 1
            
            fullBranchNames = np.hstack([["eventNum", "candNum", "ncand"], branchNamesUsed] )
            
            dataArray = np.array(data)
            dsReco = f.create_dataset("reco_candidates", dataArray.shape, h5py.h5t.NATIVE_FLOAT)
            dsReco[:] = dataArray
            dsReco.attrs.create("column_headers", fullBranchNames, fullBranchNames.shape)
            
            genFullBranchNames = np.array(["eventNum", "candNum", "genTopPt", "sampleWgt", "Njet"])
            
            genDataArray = np.array(genData)
            dsGen = f.create_dataset("gen_tops", genDataArray.shape, h5py.h5t.NATIVE_FLOAT)
            dsGen[:] = genDataArray
            dsGen.attrs.create("column_headers", genFullBranchNames, genFullBranchNames.shape)
            
            f.close()

            data = []
            genData = []

    if options.events <= 0:
        f = h5py.File(datasetFile[0:-5] + "_%i.h5"%iFile, "w")
        iFile += 1
        
        fullBranchNames = np.hstack([["eventNum", "candNum", "ncand"], branchNamesUsed] )
        
        dataArray = np.array(data)
        dsReco = f.create_dataset("reco_candidates", dataArray.shape, h5py.h5t.NATIVE_FLOAT)
        dsReco[:] = dataArray
        dsReco.attrs.create("column_headers", fullBranchNames, fullBranchNames.shape)
        
        genFullBranchNames = np.array(["eventNum", "candNum", "genTopPt", "sampleWgt", "Njet"])
        
        genDataArray = np.array(genData)
        dsGen = f.create_dataset("gen_tops", genDataArray.shape, h5py.h5t.NATIVE_FLOAT)
        dsGen[:] = genDataArray
        dsGen.attrs.create("column_headers", genFullBranchNames, genFullBranchNames.shape)
        
        f.close()

        data = []
        genData = []

def mainTFR():

    datasetFile = options.fileNum
    dataset = ROOT.TFile.Open(datasetFile)

    writer = tf.python_io.TFRecordWriter(datasetFile[0:-5] + ".tfr")

    branchNames = [b.GetName() for b in dataset.slimmedTuple.GetListOfBranches()]
    #print branchNames
    evalStr = ""
    nevt  =0
    for event in dataset.slimmedTuple:
        if nevt % 1000 == 0:
            print nevt
        if nevt == 3000:
            break
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
                    bnList.append("\"%(bname)s\":_float_feature(event.%(bname)s)"%{"bname":bn})
                    #bnListNoCand.append("\"%(bname)s\":event.%(bname)s"%{"bname":bn})
                elif nbranch >= 0:
                    bnList.append("\"%(bname)s\":_float_feature(event.%(bname)s[%%(i)i])"%{"bname":bn})
                    #bnListNoCand.append("\"%(bname)s\":0.0"%{"bname":bn})
            evalStr       = "{\"eventNum\":_float_feature(%(nevt)i), \"candNum\":_float_feature(%(i)i), \"ncand\":_float_feature(%(ncand)i), " + ",".join(bnList) + "}"
            #evalStrNoCand = "{\"eventNum\":%(nevt)i, \"candNum\":%(i)i, \"ncand\":%(ncand)i, " + ",".join(bnListNoCand) + "}"
            #print evalStr
        ncands = len(dataset.slimmedTuple.cand_m)
        if not options.gptOnly:
            for i in xrange(0, ncands):
                example = tf.train.Example(features = tf.train.Features(feature=eval(evalStr%{"i":i, "nevt":nevt, "ncand":ncands}) ) )
                writer.write(example.SerializeToString())
            #if ncands == 0:
        
        #for i in xrange(0, len(event.genTopPt)):
        #    genData.append({"eventNum":nevt, "candNum":i, "genTopPt":event.genTopPt[i], "sampleWgt":event.sampleWgt, "Njet":event.Njet})
    writer.close()

if __name__ == '__main__':
    if options.pandas:
        mainPDF()
    elif options.hdf5:
        mainHDF5()
    else:
        mainTFR()
