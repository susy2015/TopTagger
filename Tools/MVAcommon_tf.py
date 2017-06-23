import numpy
import math
import tensorflow as tf
import threading

class DataGetter:

    def __init__(self):
        self.list = ["cand_m", "j12_m", "j13_m", "j23_m", "j1_p", "j2_p", "j3_p", "dTheta12", "dTheta23", "dTheta13", "j1_CSV", "j2_CSV", "j3_CSV", "j1_QGL", "j2_QGL", "j3_QGL"]
        #self.list = ["cand_m", "j12_m_lab", "j13_m_lab", "j23_m_lab", "j1_p", "j2_p", "j3_p", "dTheta12", "dTheta23", "dTheta13", "j1_CSV_lab", "j2_CSV_lab", "j3_CSV_lab", "j1_QGL_lab", "j2_QGL_lab", "j3_QGL_lab", "dRPtTop", "dRPtW", "sd_n2"]
        #self.list = ["j1_m_lab", "j1_CSV_lab", "j2_CSV_lab", "j2_QGL_lab", "j3_CSV_lab", "j3_QGL_lab", "cand_m", "dRPtTop", "j23_m_lab", "dRPtW", "j12_m_lab", "j13_m_lab", "sd_n2"]
        #self.list = ["cand_m", "j12_m", "j13_m", "j23_m", "j1_p", "j2_p", "j3_p", "dTheta12", "dTheta23", "dTheta13", "j1_CSV", "j2_CSV", "j3_CSV", "j1_qgAxis1_lab", "j1_qgMult_lab", "j1_qgPtD_lab", "j2_qgAxis1_lab", "j2_qgMult_lab", "j2_qgPtD_lab", "j3_qgAxis1_lab", "j3_qgMult_lab", "j3_qgPtD_lab"]
        #self.list = ["j1_m_lab", "j1_CSV_lab", "j2_CSV_lab", "j3_CSV_lab", "cand_m", "dRPtTop", "j23_m_lab", "dRPtW", "j12_m_lab", "j13_m_lab", "sd_n2", "j2_qgAxis1_lab", "j2_qgMult_lab", "j2_qgPtD_lab", "j3_qgAxis1_lab", "j3_qgMult_lab", "j3_qgPtD_lab"]
        #self.list = ["cand_m", "cand_pt", "j12_m_lab", "j13_m_lab", "j23_m_lab", "j1_p", "j2_p", "j3_p", "dTheta12", "dTheta23", "dTheta13", "j1_CSV_lab", "j2_CSV_lab", "j3_CSV_lab", "dRPtTop", "dRPtW", "sd_n2", "j2_qgAxis1_lab", "j2_qgMult_lab", "j2_qgPtD_lab", "j3_qgAxis1_lab", "j3_qgMult_lab", "j3_qgPtD_lab"]
        #self.list = ["cand_m", "cand_pt", "j12_m", "j13_m", "j23_m", "j1_p", "j2_p", "j3_p", "dTheta12", "dTheta23", "dTheta13", "j1_CSV", "j2_CSV", "j3_CSV", "j1_qgAxis1_lab", "j1_qgMult_lab", "j1_qgPtD_lab", "j2_qgAxis1_lab", "j2_qgMult_lab", "j2_qgPtD_lab", "j3_qgAxis1_lab", "j3_qgMult_lab", "j3_qgPtD_lab"]

    def getList(self):
        return self.list


def weight_variable(shape, name):
    """weight_variable generates a weight variable of a given shape."""
    initial = tf.truncated_normal(shape, stddev=0.1, name=name)
    return tf.Variable(initial)


def bias_variable(shape, name):
    """bias_variable generates a bias variable of a given shape."""
    initial = tf.truncated_normal(shape, stddev=0.1, name=name)#tf.constant(0.1, shape=shape, name=name)
    return tf.Variable(initial)

### createMLP
# This fucntion is designed to create a MLP for classification purposes (using softmax_cross_entropy_with_logits)
# inputs 
#  nnStruct - a list containing the number of nodes in each layer, including the input and output layers 
#  offset_initial - a list of offsets which will be applied to the initial input features, they are stored in the tf model
#  scale_initial - a list of scales which will be applied to each input feature after the offsets are subtracted, they are stored in the tf model
# outputs
#  x - placeholder for inputs
#  y_ - placeholders for training answers 
#  y - scaled and normalized outputs for users 
#  yt - unscaled output for loss function
#  w_fc - dictionary containing all weight variables
#  b_fc - dictionary containing all bias variables 
def createMLP(nnStruct, inputDataQueue, nBatch, offset_initial, scale_initial):
    #constants 
    NLayer = len(nnStruct)

    if len(nnStruct) < 2:
        throw
    
    #Define inputs and training inputs
    x_ph = tf.placeholder(tf.float32, [None, nnStruct[0]], name="x")
    y_ph_ = tf.placeholder(tf.float32, [None, nnStruct[NLayer - 1]])
    x, y_ = inputDataQueue.dequeue_many(n=nBatch)
    #tf.train.shuffle_batch(inputDataQueue.dequeue_many(n=32), batch_size = 128, capacity = 1024, min_after_dequeue = 512, enqueue_many = True, shapes = [[16], [2]])    

    #variables for pre-transforming data
    offset = tf.constant(offset_initial, name="offest")
    scale = tf.constant(scale_initial, name="scale")

    #variables for weights and activation functions 
    w_fc = {}
    b_fc = {}
    h_fc = {}

    h_fc_ph = {}

    # Fully connected input layer
    w_fc[0] = weight_variable([nnStruct[0], nnStruct[1]], name="w_fc0")
    b_fc[0] = bias_variable([nnStruct[1]], name="b_fc0")
    h_fc[0] = tf.multiply(x-offset,scale)

    h_fc_ph[0] = tf.multiply(x_ph-offset,scale)
    
    # create hidden layers 
    for layer in xrange(1, NLayer - 1):
        #use relu for hidden layers as this seems to give best result
        h_fc[layer] = tf.nn.relu(tf.add(tf.matmul(h_fc[layer - 1], w_fc[layer - 1], name="z_fc%i"%(layer)),  b_fc[layer - 1], name="a_fc%i"%(layer)), name="h_fc%i"%(layer))
        h_fc_ph[layer] = tf.nn.relu(tf.add(tf.matmul(h_fc_ph[layer - 1], w_fc[layer - 1], name="z_fc%i"%(layer)),  b_fc[layer - 1], name="a_fc_ph%i"%(layer)), name="h_fc_ph%i"%(layer))
    
        # Map the features to next layer
        w_fc[layer] = weight_variable([nnStruct[layer], nnStruct[layer + 1]], name="w_fc%i"%(layer))
        b_fc[layer] = bias_variable([nnStruct[layer + 1]], name="b_fc%i"%(layer))
    
    #create yt for input to the softmax cross entropy for classification (this should not have softmax applied as the less function will do this)
    yt = tf.add(tf.matmul(h_fc[NLayer - 2], w_fc[NLayer - 2]),  b_fc[NLayer - 2], name="yt")
    yt_ph = tf.add(tf.matmul(h_fc_ph[NLayer - 2], w_fc[NLayer - 2]),  b_fc[NLayer - 2], name="yt_ph")
    #create output y which is conditioned to be between 0 and 1 and have nice distinct peaks for the end user
    #y = tf.multiply(tf.constant(0.5), (tf.nn.tanh(tf.constant(3.0)*yt)+tf.constant(1.0)), name="y")
    y = tf.nn.softmax(yt, name="y")
    y_ph = tf.nn.softmax(yt_ph, name="y_ph")

    return x, y_, y, yt, w_fc, b_fc, x_ph, y_ph_, yt_ph, y_ph
    

class CustomRunner(object):
    """
    This class manages the the background threads needed to fill
        a queue full of data.
    """
    def __init__(self, maxEpoch, batchSize, data, answers, queueX):
        self.dataX = tf.placeholder(dtype=tf.float32, shape=[None, 16])
        self.dataY = tf.placeholder(dtype=tf.float32, shape=[None, 2])

        self.data = data
        self.answers = answers
        self.maxEpochs = maxEpoch

        self.batch_size = batchSize
        self.length = len(self.data)
        # The actual queue of data. 
        self.queueX = queueX

        # The symbolic operation to add data to the queue
        self.enqueue_opX = self.queueX.enqueue_many([self.dataX, self.dataY])

    def data_iterator(self):
      """ A simple data iterator """
      batch_idx = 0
      nEpoch = 0
      while True:
        yield self.data[batch_idx:batch_idx+self.batch_size], self.answers[batch_idx:batch_idx+self.batch_size]
        batch_idx += self.batch_size
        if batch_idx + self.batch_size > self.length:
          batch_idx = 0
          nEpoch += 1
          if nEpoch >= self.maxEpochs:
            return

    def thread_main(self, sess):
      """
      Function run on alternate thread. Basically, keep adding data to the queue.
      """
      for dataX, dataY in self.data_iterator():
        sess.run([self.enqueue_opX], feed_dict={self.dataX:dataX, self.dataY:dataY})

      #The file is exhausted, close the queue 
      sess.run(self.queueX.close())

    def start_threads(self, sess, n_threads=1):
      qrx = tf.train.QueueRunner(self.queueX, [self.enqueue_opX] * n_threads)
      tf.train.add_queue_runner(qrx)
      
      """ Start background threads to feed queue """
      threads = []
      for n in range(n_threads):
        t = threading.Thread(target=self.thread_main, args=(sess,))
        t.daemon = True # thread will close when parent quits
        t.start()
        threads.append(t)
      return threads

    def queue_size_op(self):
      return self.queueX.size()
