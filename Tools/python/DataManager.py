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
        self.inputDataQueue = tf.RandomShuffleQueue(capacity=65536, min_after_dequeue=65536-16384, shapes=[[nFeatures], [nLabels], [nWeigts]], dtypes=[tf.float32, tf.float32, tf.float32])

        #Add DataSample objects for each data set used 
        self.sigScaleSum = 0
        for dataSet in signalDataSets:
            self.sigScaleSum += dataSet.xsec*dataSet.rescale*dataSet.kFactor/dataSet.Nevts

        self.sigDataSamples = []
        for dataSet in signalDataSets:
            batchSize = 1024
            self.sigDataSamples.append(DataSample(dataSet, nEpoch, batchSize, variables, self.inputDataQueue, ptReweight))

        self.bgScaleSum = 0
        for dataSet in signalDataSets:
            self.bgScaleSum += dataSet.xsec*dataSet.rescale*dataSet.kFactor/dataSet.Nevts

        self.bgDataSamples = []
        for dataSet in backgroundDataSets:
            self.bgDataSamples.append(DataSample(dataSet, nEpoch, batchSize, variables, self.inputDataQueue, ptReweight))

    def startFileQueues(self):
        for ds in self.sigDataSamples:
            ds.fileQueue.startQueueProcess()

        for ds in self.bgDataSamples:
            ds.fileQueue.startQueueProcess()

    def queueProcess(self, sess):
        self.startFileQueues()

        sleep(2)

        while True:
            status = True
            for ds in self.sigDataSamples:
                status = status and ds.enqueueBatch(sess)

            for ds in self.bgDataSamples:
                status = status and ds.enqueueBatch(sess)

            if not status:
                break

        sess.run(self.inputDataQueue.close())  

    def startSampleQueue(self, sess):
        self.p = threading.Thread(target=self.queueProcess, args=(sess,))
        self.p.daemon = True # thread will close when parent quits
        self.p.start()
        return self.p

    def join(self):
        self.p.join()

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
