import numpy
import math
import tensorflow as tf
import threading
import Queue
from time import sleep

from DataSample import DataSample

class DataManager:
  
    def __init__(self, variables, nEpoch, nFeatures, nLabels, nWeigts, ptReweight, signalDataSets, backgroundDataSets):
        self.nEpoch = nEpoch

        #Define input data queue
        self.inputDataQueue = tf.RandomShuffleQueue(capacity=65536*2, min_after_dequeue=65536*2 - 65536/2, shapes=[[nFeatures], [nLabels], [nWeigts]], dtypes=[tf.float32, tf.float32, tf.float32])

        #Add DataSample objects for each data set used 
        self.sigScaleSum = 0
        for dataSet in signalDataSets:
            self.sigScaleSum += dataSet.xsec*dataSet.rescale*dataSet.kFactor/dataSet.Nevts

        batchSize = 65536 / 4
        self.sigDataSamples = []
        for dataSet in signalDataSets:
            self.sigDataSamples.append(DataSample(dataSet, nEpoch, batchSize, variables, self.inputDataQueue, self.sigScaleSum, signal=True, background=False, ptReweight=ptReweight))

        self.bgScaleSum = 0
        for dataSet in signalDataSets:
            self.bgScaleSum += dataSet.xsec*dataSet.rescale*dataSet.kFactor/dataSet.Nevts

        self.bgDataSamples = []
        for dataSet in backgroundDataSets:
            self.bgDataSamples.append(DataSample(dataSet, nEpoch, batchSize, variables, self.inputDataQueue, self.bgScaleSum, signal=False, background=True, ptReweight=ptReweight))

    def startFileQueues(self, coord):
        threads = []
        for ds in self.sigDataSamples:
            threads.append(ds.fileQueue.startQueueProcess(coord))

        for ds in self.bgDataSamples:
            threads.append(ds.fileQueue.startQueueProcess(coord))

        return threads

    def launchQueueThreads(self, sess, coordS1, coordS2):
        threadsS2 = self.startFileQueues(coordS2)

        sleep(2)

        enqueueOps = []
        for ds in self.sigDataSamples:
            threadsS2 += ds.start_threads(sess, coordS2)
            enqueueOps += ds.getEnqueueOp(2048)
            
        for ds in self.bgDataSamples:
            threadsS2 += ds.start_threads(sess, coordS2)
            enqueueOps += ds.getEnqueueOp(2048)

        self.qr = tf.train.QueueRunner(self.inputDataQueue, enqueueOps, queue_closed_exception_types=(tf.errors.OutOfRangeError, tf.errors.CancelledError))

        threadsS1 = self.qr.create_threads(sess, coord=coordS1, start=True)

        return threadsS1, threadsS2

#    def startSampleQueue(self, sess, coord):
#        self.p = threading.Thread(target=self.queueProcess, args=(sess,coord,))
#        self.p.daemon = True # thread will close when parent quits
#        self.p.start()
#        return self.p
#
#    def join(self):
#        self.p.join()
#
#    def thread_main(self, sess):
#      """
#      Function run on alternate thread. Basically, keep adding data to the queue.
#      """
#      for dataX, dataY, dataW in self.data_iterator():
#        sess.run([self.enqueue_opX], feed_dict={self.dataX:dataX, self.dataY:dataY, self.dataW:dataW})
#
#    def start_threads(self, sess, n_threads=1):
#      """ Start background threads to feed queue """
#      threads = []
#      for n in range(n_threads):
#        t = threading.Thread(target=self.thread_main, args=(sess,))
#        #p.daemon = True # thread will close when parent quits
#        t.start()
#        threads.append(t)
#
#      #pass threads to FileQueue for management
#      self.fileQueue.addCustomRunnerThreads(threads)
#      return thread
