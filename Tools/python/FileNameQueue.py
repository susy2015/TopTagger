import numpy
import math
import tensorflow as tf
import threading
import Queue

class FileNameQueue:
    #This class is designed to store and randomize a filelist for use by several CustomRunner objects
    def __init__(self, files, nEpoch, nFeatures, nLabels, nWeigts, nEnqueuers = 4, batchSize = 128):
        self.files = numpy.array(files)
        self.nEpoch = nEpoch

        #filename queue
        self.fileQueue = Queue.Queue(self.files.shape[0])

        #Define input data queue
        #inputDataQueue = tf.FIFOQueue(capacity=512*32, shapes=[[16], [2], [1]], dtypes=[tf.float32, tf.float32, tf.float32])
        self.inputDataQueue = tf.RandomShuffleQueue(capacity=16384, min_after_dequeue=16384 - batchSize*(nEnqueuers + 2), shapes=[[nFeatures], [nLabels], [nWeigts]], dtypes=[tf.float32, tf.float32, tf.float32])

        #CustomRunner threads 
        self.customRunnerThreads = []

    def addCustomRunnerThreads(self, crs):
        try:
            for cr in crs:
                self.customRunnerThreads.append(cr)
        except TypeError:
            #it is not a list, so assume it is a simgle CustomRunner
            self.customRunnerThreads.append(crs)

    def getQueue(self):
        return self.fileQueue

    def get(self):
        return self.fileQueue.get(False)

    def queueProcess(self, sess):
        for i in xrange(self.nEpoch):
            perms = numpy.random.permutation(self.files.shape[0])
            self.files = self.files[perms]
            
            for fileName in self.files:
                self.fileQueue.put(tuple(fileName))

        #The files are exhausted, wait for enqueues to finish
        for cr in self.customRunnerThreads:
            cr.join()
        #and then close the queue 
        sess.run(self.inputDataQueue.close())        

    def startQueueProcess(self, sess):
        p = threading.Thread(target=self.queueProcess, args=(sess,))
        p.daemon = True # thread will close when parent quits
        p.start()
        return p

