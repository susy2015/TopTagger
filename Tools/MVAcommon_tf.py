import os
import numpy
import pandas as pd
import math
import tensorflow as tf
import threading

class DataGetter:

    def __init__(self, variables):
        if variables == "TeamAlpha":
            self.list = ["cand_m", "j12_m", "j13_m", "j23_m", "j1_p", "j2_p", "j3_p", "dTheta12", "dTheta23", "dTheta13", "j1_CSV", "j2_CSV", "j3_CSV", "j1_QGL", "j2_QGL", "j3_QGL"]
        elif variables == "Mixed":
            self.list = ["cand_m", "j12_m_lab", "j13_m_lab", "j23_m_lab", "j1_p", "j2_p", "j3_p", "dTheta12", "dTheta23", "dTheta13", "j1_CSV_lab", "j2_CSV_lab", "j3_CSV_lab", "j1_QGL_lab", "j2_QGL_lab", "j3_QGL_lab", "dRPtTop", "dRPtW", "sd_n2"]
        elif variables == "TeamA":
            self.list = ["j1_m_lab", "j1_CSV_lab", "j2_CSV_lab", "j2_QGL_lab", "j3_CSV_lab", "j3_QGL_lab", "cand_m", "dRPtTop", "j23_m_lab", "dRPtW", "j12_m_lab", "j13_m_lab", "sd_n2"]
        elif variables == "TeamAlphaMoreQGL":
            self.list = ["cand_m", "j12_m", "j13_m", "j23_m", "j1_p", "j2_p", "j3_p", "dTheta12", "dTheta23", "dTheta13", "j1_CSV", "j2_CSV", "j3_CSV", "j1_qgAxis1_lab", "j1_qgMult_lab", "j1_qgPtD_lab", "j2_qgAxis1_lab", "j2_qgMult_lab", "j2_qgPtD_lab", "j3_qgAxis1_lab", "j3_qgMult_lab", "j3_qgPtD_lab"]
        elif variables == "TeamAMoreQGL":
            self.list = ["j1_m_lab", "j1_CSV_lab", "j2_CSV_lab", "j3_CSV_lab", "cand_m", "dRPtTop", "j23_m_lab", "dRPtW", "j12_m_lab", "j13_m_lab", "sd_n2", "j2_qgAxis1_lab", "j2_qgMult_lab", "j2_qgPtD_lab", "j3_qgAxis1_lab", "j3_qgMult_lab", "j3_qgPtD_lab"]
        elif variables == "MixedMoreQGL":
            self.list = ["cand_m", "j12_m_lab", "j13_m_lab", "j23_m_lab", "j1_p", "j2_p", "j3_p", "dTheta12", "dTheta23", "dTheta13", "j1_CSV_lab", "j2_CSV_lab", "j3_CSV_lab", "dRPtTop", "dRPtW", "sd_n2", "j2_qgAxis1_lab", "j2_qgMult_lab", "j2_qgPtD_lab", "j3_qgAxis1_lab", "j3_qgMult_lab", "j3_qgPtD_lab"]
        elif variables == "TeamAlphaMoreQGLCandPt":
            self.list = ["cand_m", "cand_pt", "j12_m", "j13_m", "j23_m", "j1_p", "j2_p", "j3_p", "dTheta12", "dTheta23", "dTheta13", "j1_CSV", "j2_CSV", "j3_CSV", "j1_qgAxis1_lab", "j1_qgMult_lab", "j1_qgPtD_lab", "j2_qgAxis1_lab", "j2_qgMult_lab", "j2_qgPtD_lab", "j3_qgAxis1_lab", "j3_qgMult_lab", "j3_qgPtD_lab"]

    def getList(self):
        return self.list
    
    def prescaleBackground(self, input, answer, prescale):
      return numpy.vstack([input[answer == 1], input[answer != 1][::prescale]])
    
    def importData(self, samplesToRun = ["trainingTuple_division_0_TTbarSingleLep_training_1M.pkl.gz"], prescale = True, reluInputs=True, ptReweight=True, randomize = True):
      #variables to train
      vars = self.getList()
      
      inputData = numpy.empty([0])
      npyInputWgts = numpy.empty([0])
    
      for sample in samplesToRun:
        print sample
        if ".pkl" in sample:
          data = pd.read_pickle(sample)
        elif ".h5" in sample:
          import h5py
          f = h5py.File(sample, "r")
          npData = f["reco_candidates"][:]
          columnHeaders = f["reco_candidates"].attrs["column_headers"]
    
          indices = [npData[:,0].astype(numpy.int), npData[:,1].astype(numpy.int)]
        
          data = pd.DataFrame(npData[:,2:], index=pd.MultiIndex.from_arrays(indices), columns=columnHeaders[2:])
          f.close()
    
        #remove partial tops 
        inputLabels = data.as_matrix(["genConstiuentMatchesVec", "genTopMatchesVec"])
        inputAnswer = (inputLabels[:,0] == 3) & (inputLabels[:,1] == 1)
        inputBackground = (inputLabels[:,0] == 0) & numpy.logical_not(inputLabels[:,1])
        filterArray = ((inputAnswer == 1) | (inputBackground == 1)) & (data["ncand"] > 0)
        data = data[filterArray]
        inputAnswer = inputAnswer[filterArray]
        inputWgts = numpy.copy(data.as_matrix(["sampleWgt"]).astype(numpy.float32))
        
        if ptReweight:
          #calculate pt weights
          #inputWgts = numpy.empty([len(inputAnswer), 1])
          #ptBins = numpy.hstack([[0], numpy.linspace(50, 400, 36), numpy.linspace(450, 700, 6), [800, 10000]])
          ptBins = numpy.hstack([numpy.linspace(0, 2000, 51), [10000]])
          dataPt = data["cand_pt"]
          inputSampleWgts = data["sampleWgt"]
          ptHistSig, _ = numpy.histogram(dataPt[inputAnswer == 1], bins=ptBins, weights=inputSampleWgts[inputAnswer == 1])
          ptHistBg,  _ = numpy.histogram(dataPt[inputAnswer != 1], bins=ptBins, weights=inputSampleWgts[inputAnswer != 1])
          ptHistSig[ptHistSig < 10] = ptHistSig.max()
          ptHistBg[ptHistBg < 10] = ptHistBg.max()
          inputWgts[inputAnswer == 1] *= (1.0/ptHistSig[numpy.digitize(dataPt[inputAnswer == 1], ptBins) - 1]).reshape([-1,1])
          inputWgts[inputAnswer != 1] *= (1.0/ptHistBg [numpy.digitize(dataPt[inputAnswer != 1], ptBins) - 1]).reshape([-1,1])
    
        if len(inputData) == 0:
          inputData = data
          npyInputWgts = inputWgts
        else:
          inputData = pd.concat([inputData, data])
          npyInputWgts = numpy.vstack([npyInputWgts, inputWgts])
    
      #parse pandas dataframe into training data
      npyInputData = inputData.as_matrix(vars).astype(numpy.float32)
      npyInputLabels = inputData.as_matrix(["genConstiuentMatchesVec", "genTopMatchesVec"])
      npyInputAnswer = (npyInputLabels[:,0] == 3) & (npyInputLabels[:,1] == 1)
      npyInputAnswers = numpy.vstack([npyInputAnswer,numpy.logical_not(npyInputAnswer)]).transpose()
      npyInputSampleWgts = inputData.as_matrix(["sampleWgt"]).astype(numpy.float32)
    
      if reluInputs:
        #ensure data is all greater than one
        npyInputData[npyInputData < 0] = 0.0
    
      if prescale:
        #Remove background events so that bg and signal are roughly equally represented
        prescaleRatio = (npyInputAnswer != 1).sum()/(npyInputAnswer == 1).sum()
    
        npyInputData =       self.prescaleBackground(npyInputData, npyInputAnswer, prescaleRatio)
        npyInputAnswers =    self.prescaleBackground(npyInputAnswers, npyInputAnswer, prescaleRatio)
        npyInputWgts =       self.prescaleBackground(npyInputWgts, npyInputAnswer, prescaleRatio)
        npyInputSampleWgts = self.prescaleBackground(npyInputSampleWgts, npyInputAnswer, prescaleRatio)
    
      #equalize bg and signal weights 
      nsig = npyInputWgts[npyInputAnswers[:,0] == 1].sum()
      nbg  = npyInputWgts[npyInputAnswers[:,0] != 1].sum()
      npyInputWgts[npyInputAnswers[:,0] != 1] *= nsig / nbg
    
      if randomize:
        #randomize input data
        perms = numpy.random.permutation(npyInputData.shape[0])
        npyInputData = npyInputData[perms]
        npyInputAnswers = npyInputAnswers[perms]
        npyInputWgts = npyInputWgts[perms]
        npyInputSampleWgts = npyInputSampleWgts[perms]
    
      return npyInputData, npyInputAnswers, npyInputWgts, npyInputSampleWgts


class createModel:
    def weight_variable(self, shape, name):
        """weight_variable generates a weight variable of a given shape."""
        initial = tf.truncated_normal(shape, stddev=0.1, name=name)
        return tf.Variable(initial)
    
    def bias_variable(self, shape, name):
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
    def createMLP(self):
        #constants 
        NLayer = len(self.nnStruct)
    
        if len(self.nnStruct) < 2:
            throw
        
        #Define inputs and training inputs
        self.x_ph = tf.placeholder(tf.float32, [None, self.nnStruct[0]], name="x")
        self.y_ph_ = tf.placeholder(tf.float32, [None, self.nnStruct[NLayer - 1]])
        self.x, self.y_ = self.inputDataQueue.dequeue_many(n=self.nBatch)
        #tf.train.shuffle_batch(inputDataQueue.dequeue_many(n=32), batch_size = 128, capacity = 1024, min_after_dequeue = 512, enqueue_many = True, shapes = [[16], [2]])    
    
        #variables for pre-transforming data
        self.offset = tf.constant(self.offset_initial, name="offest")
        self.scale = tf.constant(self.scale_initial, name="scale")
    
        #variables for weights and activation functions 
        self.w_fc = {}
        self.b_fc = {}
        self.h_fc = {}
    
        self.h_fc_ph = {}
    
        # Fully connected input layer
        self.w_fc[0] = self.weight_variable([self.nnStruct[0], self.nnStruct[1]], name="w_fc0")
        self.b_fc[0] = self.bias_variable([self.nnStruct[1]], name="b_fc0")
        self.h_fc[0] = tf.multiply(self.x-self.offset,self.scale)
    
        self.h_fc_ph[0] = tf.multiply(self.x_ph-self.offset,self.scale)
        
        # create hidden layers 
        for layer in xrange(1, NLayer - 1):
            #use relu for hidden layers as this seems to give best result
            self.h_fc[layer] = tf.nn.relu(tf.add(tf.matmul(self.h_fc[layer - 1], self.w_fc[layer - 1], name="z_fc%i"%(layer)),  self.b_fc[layer - 1], name="a_fc%i"%(layer)), name="h_fc%i"%(layer))
            self.h_fc_ph[layer] = tf.nn.relu(tf.add(tf.matmul(self.h_fc_ph[layer - 1], self.w_fc[layer - 1], name="z_fc%i"%(layer)),  self.b_fc[layer - 1], name="a_fc_ph%i"%(layer)), name="h_fc_ph%i"%(layer))
        
            # Map the features to next layer
            self.w_fc[layer] = self.weight_variable([self.nnStruct[layer], self.nnStruct[layer + 1]], name="w_fc%i"%(layer))
            self.b_fc[layer] = self.bias_variable([self.nnStruct[layer + 1]], name="b_fc%i"%(layer))
        
        #create yt for input to the softmax cross entropy for classification (this should not have softmax applied as the less function will do this)
        self.yt = tf.add(tf.matmul(self.h_fc[NLayer - 2], self.w_fc[NLayer - 2]),  self.b_fc[NLayer - 2], name="yt")
        self.yt_ph = tf.add(tf.matmul(self.h_fc_ph[NLayer - 2], self.w_fc[NLayer - 2]),  self.b_fc[NLayer - 2], name="yt_ph")

        #final answer with softmax applied for the end user
        self.y = tf.nn.softmax(self.yt, name="y")
        self.y_ph = tf.nn.softmax(self.yt_ph, name="y_ph")
    
    def createLoss(self):
        # other placeholders 
        self.reg = tf.placeholder(tf.float32)
        
        self.wgt = tf.placeholder(tf.float32, [None, 1])
        
        tf.add_to_collection('TrainInfo', self.x)
        tf.add_to_collection('TrainInfo', self.y)
        
        #cross_entropy = tf.divide(tf.reduce_sum(tf.multiply(tf.nn.softmax_cross_entropy_with_logits(labels=y_, logits=yt), wgt)), tf.reduce_sum(wgt))
        self.cross_entropy = tf.reduce_mean(tf.nn.softmax_cross_entropy_with_logits(labels=self.y_, logits=self.yt))
        self.cross_entropy_ph = tf.reduce_mean(tf.nn.softmax_cross_entropy_with_logits(labels=self.y_ph_, logits=self.yt_ph))
        self.l2_norm = tf.constant(0.0)
        for w in self.w_fc.values():
          self.l2_norm += tf.nn.l2_loss(w)
        self.loss = self.cross_entropy + self.l2_norm*self.reg
        self.loss_ph = self.cross_entropy_ph + self.l2_norm*self.reg
        #train_step = tf.train.GradientDescentOptimizer(1.0).minimize(cross_entropy)
        self.train_step = tf.train.AdamOptimizer(1e-3).minimize(self.loss, var_list=self.w_fc.values() + self.b_fc.values())


    def createSummaries(self):
        correct_prediction = tf.equal(tf.argmax(self.y_ph, 1), tf.argmax(self.y_ph_, 1))
        self.accuracy = tf.reduce_mean(tf.cast(correct_prediction, tf.float32))

        # Create a summary to monitor cost tensor
        summary_ce = tf.summary.scalar("cross_entropy", self.cross_entropy)
        summary_l2n = tf.summary.scalar("l2_norm", self.l2_norm)
        summary_loss = tf.summary.scalar("loss", self.loss)
        #summary_queueSize = tf.summary.scalar("queue_size", cr.queue_size_op())
        summary_vloss = tf.summary.scalar("valid_loss", self.loss_ph)
        # Create a summary to monitor accuracy tensor
        summary_accuracy = tf.summary.scalar("accuracy", self.accuracy)
        # Merge all summaries into a single op
        #self.merged_train_summary_op = tf.summary.merge([summary_ce, summary_l2n, summary_loss, summary_queueSize])
        self.merged_train_summary_op = tf.summary.merge([summary_ce, summary_l2n, summary_loss])
        self.merged_valid_summary_op = tf.summary.merge([summary_accuracy, summary_vloss])


    def __init__(self, nnStruct, inputDataQueue, nBatch, offset_initial, scale_initial):
        self.nnStruct = nnStruct
        self.inputDataQueue = inputDataQueue
        self.nBatch = nBatch
        self.offset_initial = offset_initial
        self.scale_initial = scale_initial

        self.createMLP()
        self.createLoss()
        self.createSummaries()

        #Create saver object to save variables 
        self.saver = None
        
    def saveCheckpoint(self, sess, outputDirectory = ""):
        #Create saver object to save variables 
        if self.saver == None:
            self.saver = tf.train.Saver()

        #Save training checkpoint (contains a copy of the model and the weights)
        try:
          os.mkdir(outputDirectory + "models")
        except OSError:
          pass
        checkpoint_path = outputDirectory + "models/model.ckpt"
        save_path = self.saver.save(sess, checkpoint_path)

        print("Model checkpoint saved in file: %s" % save_path)
    
    def saveModel(self, sess, outputDirectory = ""):
        from tensorflow.python.framework import graph_io
        from tensorflow.python.tools import freeze_graph

        input_graph_path = outputDirectory + "tfModel.pb"
        graph_io.write_graph(sess.graph, "./", input_graph_path)
    
        #create frozen version of graph for distribution
        input_saver_def_path = ""
        input_binary = False
        checkpoint_path = outputDirectory + "models/model.ckpt"
        output_node_names = "y_ph"
        restore_op_name = "save/restore_all"
        filename_tensor_name = "save/Const:0"
        output_graph_path = outputDirectory + "tfModel_frozen.pb"
        clear_devices = False
    
        freeze_graph.freeze_graph(input_graph_path, input_saver_def_path,
                                  input_binary, checkpoint_path, output_node_names,
                                  restore_op_name, filename_tensor_name,
                                  output_graph_path, clear_devices, "")
    
        print("Frozen model (model and weights) saved in file: %s" % output_graph_path)


class CustomRunner(object):
    """
    This class manages the background threads needed to fill
        a queue full of data.
    """
    def __init__(self, maxEpoch, batchSize, data, answers, queueX):
        self.dataX = tf.placeholder(dtype=tf.float32, shape=[None, data.shape[1]])
        self.dataY = tf.placeholder(dtype=tf.float32, shape=[None, answers.shape[1]])

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
