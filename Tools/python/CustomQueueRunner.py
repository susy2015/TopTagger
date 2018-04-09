import tensorflow as tf
import threading
import Queue

from DataGetter import DataGetter

class CustomQueueRunner(object):
    """
    This class manages the background threads needed to fill
        a queue full of data.
    """
    def __init__(self, batchSize, variables, fileQueue, inputDataQueue, signal, background, ptReweight=False):

        self.fileQueue = fileQueue
        # The actual queue of data. 
        self.queueX = inputDataQueue

        self.signal = signal
        self.background = background

        # place holders to enqueue data with 
        self.dataX = tf.placeholder(dtype=tf.float32, shape=[None, self.queueX.shapes[0][0]])
        self.dataY = tf.placeholder(dtype=tf.float32, shape=[None, self.queueX.shapes[1][0]])
        self.dataW = tf.placeholder(dtype=tf.float32, shape=[None, self.queueX.shapes[2][0]])

        #feature names to select
        self.variables = variables

        # enqueue batch size - NOT the dequeue batch for training
        self.batch_size = batchSize

        # The symbolic operation to add data to the queue
        self.enqueue_opX = self.queueX.enqueue_many([self.dataX, self.dataY, self.dataW])

        # additional options 
        self.ptReweight = ptReweight

    def fileName_iterator(self):
        #Simple iterator to get new filename from file queue
        while True:
            try:
                yield self.fileQueue.get()
            except Queue.Empty:
                return

    def thread_readFile(self, dg, fname, dataOut):
      dataOut.append( dg.importData([fname]) )

    def data_iterator(self):
      """ A simple data iterator """

      #filename iterator 
      fIter = self.fileName_iterator()

      #private data getter object 
      dg = DataGetter(self.variables, signal=self.signal, background=self.background, bufferData = False)
            
      #loop until there are no more files to get from the queue
      for fileName in fIter:
        #read next file
        data = dg.importData(fileName, ptReweight=self.ptReweight)
        batch_idx = 0
        while batch_idx + self.batch_size <= data["data"].shape[0]:
            yield data["data"][batch_idx:batch_idx+self.batch_size], data["labels"][batch_idx:batch_idx+self.batch_size], data["weights"][batch_idx:batch_idx+self.batch_size]
            batch_idx += self.batch_size

      return

#    def enqueueBatch(self, sess):
#        #Enqueue one batch into the main queue
#        try:
#            dataX, dataY, dataW = next(self.data_iterator())
#            sess.run([self.enqueue_opX], feed_dict={self.dataX:dataX, self.dataY:dataY, self.dataW:dataW})
#            return True
#        except StopIteration:
#            return False

    def thread_main(self, sess, coord):
      """
      Function run on alternate thread. Basically, keep adding data to the queue.
      """
      for dataX, dataY, dataW in self.data_iterator():
        if coord.should_stop():
          break
        sess.run([self.enqueue_opX], feed_dict={self.dataX:dataX, self.dataY:dataY, self.dataW:dataW})

      sess.run(self.queueX.close())

    def start_threads(self, sess, coord, n_threads):
      """ Start background threads to feed queue """
      threads = []
      for n in range(n_threads):
        t = threading.Thread(target=self.thread_main, args=(sess, coord, ))
        #p.daemon = True # thread will close when parent quits
        t.start()
        threads.append(t)

      #pass threads to FileQueue for management
      #self.fileQueue.addCustomRunnerThreads(threads)
      return threads

