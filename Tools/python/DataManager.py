import numpy
import math
import tensorflow as tf
import threading
import Queue
from time import sleep

from DataSample import DataSample

class DataManager:
  
    def __init__(self, variables, nEpoch, nFeatures, nLabels, nDomains, nWeigts, ptReweight, signalDataSets, backgroundDataSets):
        self.nEpoch = nEpoch

        #Define input data queue
        self.inputDataQueue = tf.RandomShuffleQueue(capacity=65536*2, min_after_dequeue=65536*2 - 65536/2, shapes=[[nFeatures], [nLabels], [nDomains], [nWeigts]], dtypes=[tf.float32, tf.float32, tf.float32, tf.float32])

        #Add DataSample objects for each data set used 
        self.sigScaleSum = 0
        for dataSet in signalDataSets:
            self.sigScaleSum += dataSet.xsec*dataSet.rescale*dataSet.kFactor/dataSet.Nevts

        batchSize = 65536 / 4
        self.sigDataSamples = []
        for dataSet in signalDataSets:
            self.sigDataSamples.append(DataSample(dataSet, nEpoch, batchSize, variables, self.inputDataQueue, self.sigScaleSum, signal=True, background=False, domain=dataSet.domain, ptReweight=ptReweight))

        self.bgScaleSum = 0
        for dataSet in signalDataSets:
            self.bgScaleSum += dataSet.xsec*dataSet.rescale*dataSet.kFactor/dataSet.Nevts

        self.bgDataSamples = []
        for dataSet in backgroundDataSets:
            self.bgDataSamples.append(DataSample(dataSet, nEpoch, batchSize, variables, self.inputDataQueue, self.bgScaleSum, signal=False, background=True, domain=dataSet.domain, ptReweight=ptReweight))

    def startFileQueues(self, coord):
        threads = []
        for ds in self.sigDataSamples:
            threads.append(ds.fileQueue.startQueueProcess(coord))

        for ds in self.bgDataSamples:
            threads.append(ds.fileQueue.startQueueProcess(coord))

        return threads

    def startDataQueues(self, sess, coord):
        enqueueOps = []
        threads = []
        for ds in self.sigDataSamples:
            threads += ds.start_threads(sess, coord)
            enqueueOps += ds.getEnqueueOp(2048, ds.dataSet.nEnqueueThreads)
            
        for ds in self.bgDataSamples:
            threads += ds.start_threads(sess, coord)
            enqueueOps += ds.getEnqueueOp(2048, ds.dataSet.nEnqueueThreads)

        return threads, enqueueOps

    def launchQueueThreads(self, sess):

        # Create a coordinator, launch the queue runner threads.
        #stage 1 data cooridnator manages final random shuffle queue
        self.coordS1 = tf.train.Coordinator()
        #stage 2 data cooridnator manages FIFO queues, custom runner queues, and file name queues 
        self.coordS2 = tf.train.Coordinator()

        self.threadsS2 = self.startFileQueues(self.coordS2)

        #ensure that the filename queues are all populated before continuing 
        sleep(2)

        dataThreads, enqueueOps = self.startDataQueues(sess, self.coordS2)
        self.threadsS2 += dataThreads

        #create tf queue runner to manage the final random shuffle queue
        self.qr = tf.train.QueueRunner(self.inputDataQueue, enqueueOps, queue_closed_exception_types=(tf.errors.OutOfRangeError, tf.errors.CancelledError))

        self.threadsS1 = self.qr.create_threads(sess, coord=self.coordS1, start=True)

    def continueTrainingLoop(self):
        try:
            return not self.coordS2.should_stop()
        except AttributeError:
            print "Run launchQueueThreads before starting the training loop"

    def continueFlushingQueue(self):
        try:
            return not self.coordS1.should_stop()
        except AttributeError:
            print "Run launchQueueThreads before starting the training loop"

    def requestStop(self, e = None):
        try:
            if e == None:
                self.coordS1.request_stop()
                self.coordS2.request_stop()
            else:
                self.coordS1.request_stop(e)
                self.coordS2.request_stop(e)
        except AttributeError:
            print "Run launchQueueThreads before starting the training loop"

    def join(self):
        try:
            self.coordS1.join(self.threadsS1)
            self.coordS2.join(self.threadsS2)
        except AttributeError:
            print "Run launchQueueThreads before starting the training loop"
        

