import numpy
import math
import tensorflow as tf
import threading
import Queue

class FileNameQueue:
    #This class is designed to store and randomize a filelist for use by several CustomRunner objects
    def __init__(self, files, nEpoch):
        self.files = numpy.array(files)
        self.nEpoch = nEpoch

        #filename queue
        self.fileQueue = Queue.Queue(self.files.shape[0])

    def getQueue(self):
        return self.fileQueue

    def get(self):
        return self.fileQueue.get(False)

    def queueProcess(self, coord):
        for i in xrange(self.nEpoch + 100):
            if coord.should_stop():
                break
            perms = numpy.random.permutation(self.files.shape[0])
            self.files = self.files[perms]
            
            for fileName in self.files:
                breakLoop = False
                while not breakLoop:
                    if coord.should_stop():
                        break
                    try:
                        self.fileQueue.put(tuple((fileName,)), timeout=10 )
                    except Queue.Full:
                        continue
                    breakLoop = True
            if i == self.nEpoch:
                coord.request_stop()

    def startQueueProcess(self, coord):
        p = threading.Thread(target=self.queueProcess, args=(coord,))
        p.daemon = True # thread will close when parent quits
        p.start()
        return p

