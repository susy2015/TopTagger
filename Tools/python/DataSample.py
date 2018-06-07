import tensorflow as tf
import numpy as np

from glob import glob

from CustomQueueRunner import CustomQueueRunner
from FileNameQueue import FileNameQueue

class DataSample:
    def __init__(self, dataSet, nEpoch, batchSize, variables, inputDataQueue, sumScaleFactor, signal = True, background = True, domain = 0, ptReweight=False):

        self.dataSet = dataSet

        self.inputDataQueue = inputDataQueue

        self.queue = tf.FIFOQueue(capacity = 32768, shapes = inputDataQueue.shapes, dtypes = inputDataQueue.dtypes)

        #enqueue delay
        self.scaleFactor = self.dataSet.xsec*self.dataSet.rescale*self.dataSet.kFactor/self.dataSet.Nevts
        self.batchSize = batchSize

        #create file name list form file glob
        self.fileList = glob(self.dataSet.fileGlob)
        #Create file queue of input files 
        self.fileQueue = FileNameQueue(self.fileList, nEpoch)

        #create CustomRunner for this dataset 
        self.customRunner = CustomQueueRunner(self.batchSize, variables, self.fileQueue, self.queue, signal, background, domain, ptReweight)


    def start_threads(self, sess, coord, n_threads=1):
        return self.customRunner.start_threads(sess, coord, n_threads)

    def getEnqueueOp(self, nSample, nQueue = 1):
        return [self.inputDataQueue.enqueue_many(self.queue.dequeue_many(nSample)),] * nQueue
