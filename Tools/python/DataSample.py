import tensorflow as tf
import numpy as np

from glob import glob

from CustomQueueRunner import CustomQueueRunner
from FileNameQueue import FileNameQueue

class DataSample:
    def __init__(self, dataSet, nEpoch, batchSize, variables, inputDataQueue, sumScaleFactor, ptReweight=False):

        self.dataSet = dataSet

        #enqueue delay
        self.scaleFactor = self.dataSet.xsec*self.dataSet.rescale*self.dataSet.kFactor/self.dataSet.Nevts
        self.batchSize = batchSize

        #create file name list form file glob
        self.fileList = glob(self.dataSet.fileGlob)
        #Create file queue of input files 
        self.fileQueue = FileNameQueue(self.fileList, nEpoch)

        #create CustomRunner for this dataset 
        self.customRunner = CustomQueueRunner(self.batchSize, variables, self.fileQueue, inputDataQueue, ptReweight)


    def enqueueBatch(self, sess):
        return self.customRunner.enqueueBatch(sess)

    def abort(self):
        self.fileQueueThread = self.fileQueue.fileQueue.close()
