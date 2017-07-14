import os
import numpy
import pandas as pd
import math
import tensorflow as tf
import threading
import Queue

class DataGetter:

    #The constructor simply takes in a list and saves it to self.list
    def __init__(self, variables):
        self.list = variables

    #This method accepts a string and will return a DataGetter object with the variable list defined in this method.
    @classmethod
    def StandardVariables(cls, variables):

        if variables == "TeamAlpha":
            vNames = ["cand_m", "j12_m", "j13_m", "j23_m", "j1_p", "j2_p", "j3_p", "dTheta12", "dTheta23", "dTheta13", "j1_CSV", "j2_CSV", "j3_CSV", "j1_QGL", "j2_QGL", "j3_QGL"]
        elif variables == "Mixed":
            vNames = ["cand_m", "j12_m_lab", "j13_m_lab", "j23_m_lab", "j1_p", "j2_p", "j3_p", "dTheta12", "dTheta23", "dTheta13", "j1_CSV_lab", "j2_CSV_lab", "j3_CSV_lab", "j1_QGL_lab", "j2_QGL_lab", "j3_QGL_lab", "dRPtTop", "dRPtW", "sd_n2"]
        elif variables == "TeamA":
            vNames = ["j1_m_lab", "j1_CSV_lab", "j2_CSV_lab", "j2_QGL_lab", "j3_CSV_lab", "j3_QGL_lab", "cand_m", "dRPtTop", "j23_m_lab", "dRPtW", "j12_m_lab", "j13_m_lab", "sd_n2"]
        elif variables == "TeamAlphaMoreQGL":
            vNames = ["cand_m", "j12_m", "j13_m", "j23_m", "j1_p", "j2_p", "j3_p", "dTheta12", "dTheta23", "dTheta13", "j1_CSV", "j2_CSV", "j3_CSV", "j1_qgAxis1_lab", "j1_qgMult_lab", "j1_qgPtD_lab", "j2_qgAxis1_lab", "j2_qgMult_lab", "j2_qgPtD_lab", "j3_qgAxis1_lab", "j3_qgMult_lab", "j3_qgPtD_lab"]
        elif variables == "TeamAMoreQGL":
            vNames = ["j1_m_lab", "j1_CSV_lab", "j2_CSV_lab", "j3_CSV_lab", "cand_m", "dRPtTop", "j23_m_lab", "dRPtW", "j12_m_lab", "j13_m_lab", "sd_n2", "j2_qgAxis1_lab", "j2_qgMult_lab", "j2_qgPtD_lab", "j3_qgAxis1_lab", "j3_qgMult_lab", "j3_qgPtD_lab"]
        elif variables == "MixedMoreQGLCandPt":
            vNames = ["cand_m", "cand_pt", "j12_m", "j13_m", "j23_m", "j1_p", "j2_p", "j3_p", "dTheta12", "dTheta23", "dTheta13", "j1_CSV_lab", "j2_CSV_lab", "j3_CSV_lab", "dRPtTop", "dRPtW", "sd_n2", "j2_qgAxis1_lab", "j2_qgMult_lab", "j2_qgPtD_lab", "j3_qgAxis1_lab", "j3_qgMult_lab", "j3_qgPtD_lab"]
        elif variables == "TeamAlphaMoreQGLCandPt":
            vNames = ["cand_m", "cand_pt", "j12_m", "j13_m", "j23_m", "j1_p", "j2_p", "j3_p", "dTheta12", "dTheta23", "dTheta13", "j1_CSV", "j2_CSV", "j3_CSV", "j1_qgAxis1_lab", "j1_qgMult_lab", "j1_qgPtD_lab", "j2_qgAxis1_lab", "j2_qgMult_lab", "j2_qgPtD_lab", "j3_qgAxis1_lab", "j3_qgMult_lab", "j3_qgPtD_lab"]
        elif variables == "TeamAMoreQGLCandPt":
            vNames = ["cand_pt", "j1_m_lab", "j1_CSV_lab", "j2_CSV_lab", "j3_CSV_lab", "cand_m", "dRPtTop", "j23_m_lab", "dRPtW", "j12_m_lab", "j13_m_lab", "sd_n2", "j2_qgAxis1_lab", "j2_qgMult_lab", "j2_qgPtD_lab", "j3_qgAxis1_lab", "j3_qgMult_lab", "j3_qgPtD_lab"]
        elif variables == "TeamAlpha1DConv":
            #vNames = ["cand_m", "cand_pt", "j12_m", "j13_m", "j23_m", "dTheta12", "dTheta23", "dTheta13", "j1_p", "j2_p", "j3_p", "j1_CSV", "j2_CSV", "j3_CSV", "j1_qgAxis1_lab", "j2_qgAxis1_lab", "j3_qgAxis1_lab", "j1_qgMult_lab", "j2_qgMult_lab", "j3_qgMult_lab", "j1_qgPtD_lab", "j2_qgPtD_lab", "j3_qgPtD_lab"]
            vNames = ["cand_m", "cand_pt", "j12_m", "j13_m", "j23_m", "dTheta12", "dTheta23", "dTheta13", "j1_p", "j1_CSV", "j1_qgAxis1_lab", "j1_qgMult_lab", "j1_qgPtD_lab", "j1_chargedHadEFrac","j1_chargedEmEFrac", "j1_neutralEmEFrac", "j1_muonEFrac", "j1_photonEFrac", "j1_elecEFrac", "j1_chargedHadMult", "j1_neutralHadMult", "j1_photonMult", "j1_elecMult", "j1_muonMult", "j1_jetCharge", "j2_p", "j2_CSV", "j2_qgAxis1_lab", "j2_qgMult_lab", "j2_qgPtD_lab", "j2_chargedHadEFrac","j2_chargedEmEFrac", "j2_neutralEmEFrac", "j2_muonEFrac", "j2_photonEFrac", "j2_elecEFrac", "j2_chargedHadMult", "j2_neutralHadMult", "j2_photonMult", "j2_elecMult", "j2_muonMult", "j2_jetCharge", "j3_p", "j3_CSV", "j3_qgAxis1_lab", "j3_qgMult_lab", "j3_qgPtD_lab", "j3_chargedHadEFrac","j3_chargedEmEFrac", "j3_neutralEmEFrac", "j3_muonEFrac", "j3_photonEFrac", "j3_elecEFrac", "j3_chargedHadMult", "j3_neutralHadMult", "j3_photonMult", "j3_elecMult", "j3_muonMult", "j3_jetCharge"];

        return cls(vNames)

    #Simply accept a list and pass it to the constructor
    @classmethod
    def DefinedVariables(cls, variables):
        return cls(variables)

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
        #print sample
        #if ".pkl" in sample:
        #  data = pd.read_pickle(sample)
        #elif ".h5" in sample:
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
    
      #normalize training weights
      npyInputWgts /= npyInputWgts.mean()
      
      if randomize:
        #randomize input data
        perms = numpy.random.permutation(npyInputData.shape[0])
        npyInputData = npyInputData[perms]
        npyInputAnswers = npyInputAnswers[perms]
        npyInputWgts = npyInputWgts[perms]
        npyInputSampleWgts = npyInputSampleWgts[perms]
    
      return {"data":npyInputData, "labels":npyInputAnswers, "weights":npyInputWgts, "":npyInputSampleWgts}


class CreateModel:
    def weight_variable(self, shape, name):
        """weight_variable generates a weight variable of a given shape."""
        initial = tf.truncated_normal(shape, stddev=0.1, name=name)
        return tf.Variable(initial)
    
    def bias_variable(self, shape, name):
        """bias_variable generates a bias variable of a given shape."""
        initial = tf.truncated_normal(shape, stddev=0.1, name=name)#tf.constant(0.1, shape=shape, name=name)
        return tf.Variable(initial)
    
    def createRecurentLayers(self, inputs, nodes=[], share=None):
        #define variable scope
        with tf.variable_scope("rnn") as scope:
            #condition input shape 
            slicedInputs = []
            for iW in xrange(inputs.shape[1]):
                slicedInputs.append(tf.reshape(tf.slice(inputs, [0,iW,0], [-1, 1, -1]), [-1, int(inputs.shape[2])]))
            #create the rnn cell 
            cell = tf.nn.rnn_cell.BasicLSTMCell(nodes[0], reuse=share)
            #create the rnn layer 
            output, _ = tf.nn.static_rnn(cell, slicedInputs, dtype=tf.float32, scope=scope)
            #reshape output to match the cnn output
            output = tf.stack(output, axis=1) 
            return output

    def createConvLayers(self, inputs, convWeights, postfix=""):
        with tf.variable_scope("cnn") as scope:
            #list to hold conv layers 
            convLayers = [inputs]

            #create the convolutional layers
            for iLayer in xrange(len(convWeights)):
                convLayers.append(tf.nn.relu(tf.nn.conv1d(value = convLayers[iLayer], filters=convWeights[iLayer], stride=1, data_format='NHWC', padding='SAME', name="conv1d"+postfix)))
    
            return convLayers[-1]

    def createCNNRNNLayers(self, NDENSEONLYVAR, NCONSTITUENTS, nChannel, inputVars, convWeights=[], rnnNodes=[], postfix=""):
        #prep inputs by splitting apart dense only variables from convolutino variables and reshape convolution variables
        dInputs = tf.slice(inputVars, [0,0], [-1, NDENSEONLYVAR])
        cProtoInputs = tf.slice(inputVars, [0,NDENSEONLYVAR], [-1, -1])
        cInputs = tf.reshape(cProtoInputs, [-1, NCONSTITUENTS, nChannel])
        
        output = cInputs

        if len(convWeights) > 0:
            output = self.createConvLayers(output, convWeights, postfix)

        if len(rnnNodes) > 0:
            output = self.createRecurentLayers(output, [16], len(postfix)>0)

        #Reshape convolutional output and recombine with the variables which bypass the convolution stage 
        convLayerShape = output.shape[1] * output.shape[2]
        flatConvLayer = tf.reshape(output, shape=[-1, int(convLayerShape)])
        denseInputLayer = tf.concat([dInputs, flatConvLayer], axis=1, name="dil"+postfix)

        return denseInputLayer

    def createDenseNetwork(self, denseInputLayer, nnStruct, w_fc = {}, b_fc = {}, prefix=""):
        with tf.variable_scope("dense") as scope:
            #constants 
            NLayer = len(self.nnStruct)
            
            #variables
            h_fc = {}
            
            # Fully connected input layer
            if not 0 in w_fc:
                w_fc[0] = self.weight_variable([int(denseInputLayer.shape[1]), nnStruct[1]], name="w_fc0")
            if not 0 in b_fc:
                b_fc[0] = self.bias_variable([nnStruct[1]], name="b_fc0")
            
            h_fc[0] = denseInputLayer
            
            # create hidden layers 
            for layer in xrange(1, NLayer - 1):
                #use relu for hidden layers as this seems to give best result
                h_fc[layer] = tf.nn.relu(tf.add(tf.matmul(h_fc[layer - 1], w_fc[layer - 1], name="z_fc%i%s"%(layer,prefix)),  b_fc[layer - 1], name="a_fc%i%s"%(layer,prefix)), name="h_fc%i%s"%(layer,prefix))
            
                # Map the features to next layer
                if not layer in w_fc:
                    w_fc[layer] = self.weight_variable([nnStruct[layer], nnStruct[layer + 1]], name="w_fc%i"%(layer))
                if not layer in b_fc:
                    b_fc[layer] = self.bias_variable([nnStruct[layer + 1]], name="b_fc%i"%(layer))
            
            #create yt for input to the softmax cross entropy for classification (this should not have softmax applied as the loss function will do this)
            yt = tf.add(tf.matmul(h_fc[NLayer - 2], w_fc[NLayer - 2]),  b_fc[NLayer - 2], name="yt"+prefix)
            
            return yt


    ### createMLP
    # This fucntion is designed to create a MLP for classification purposes (using softmax_cross_entropy_with_logits)
    # inputs 
    #  nnStruct - a list containing the number of nodes in each layer, including the input and output layers 
    #  offset_initial - a list of offsets which will be applied to the initial input features, they are stored in the tf model
    #  scale_initial - a list of scales which will be applied to each input feature after the offsets are subtracted, they are stored in the tf model
    def createMLP(self, useConvolution=False):
        #constants 
        NLayer = len(self.nnStruct)
    
        if len(self.nnStruct) < 2:
            throw
        
        #Define inputs and training inputs
        self.x_ph = tf.placeholder(tf.float32, [None, self.nnStruct[0]], name="x")
        self.y_ph_ = tf.placeholder(tf.float32, [None, self.nnStruct[NLayer - 1]], name="y_ph_")
        self.wgt_ph = tf.placeholder(tf.float32, [None, 1], name="wgt_ph")
        self.x, self.y_, self.wgt = self.inputDataQueue.dequeue_many(n=self.nBatch)

        #variables for pre-transforming data
        self.offset = tf.constant(self.offset_initial, name="offest")
        self.scale = tf.constant(self.scale_initial, name="scale")

        #input variables after rescaling 
        transformedX = tf.multiply(self.x-self.offset,self.scale)
        transformedX_ph = tf.multiply(self.x_ph-self.offset,self.scale)

        if useConvolution:
            #Implement 1D convolution layer here
            NDENSEONLYVAR = 8
            NCONSTITUENTS = 3
            FILTERWIDTH = 1

            nChannel = int((transformedX.shape[1] - NDENSEONLYVAR)/NCONSTITUENTS)

            #weights for convolution filters - shared between all parallel graphs
            self.convWeights = [tf.Variable(tf.random_normal([FILTERWIDTH, nChannel, 16]), name="conv1_weights"),
                                tf.Variable(tf.random_normal([FILTERWIDTH,       16,  8]), name="conv1_weights")]

            #Create colvolution layers 
            denseInputLayer = self.createCNNRNNLayers(NDENSEONLYVAR, NCONSTITUENTS, nChannel, transformedX, convWeights=self.convWeights, rnnNodes=[16], postfix="")
            #denseInputLayer = self.createCNNRNNLayers(NDENSEONLYVAR, NCONSTITUENTS, nChannel, transformedX, convWeights=[], rnnNodes=[16], postfix="")
            denseInputLayer_ph = self.createCNNRNNLayers(NDENSEONLYVAR, NCONSTITUENTS, nChannel, transformedX_ph, convWeights=self.convWeights, rnnNodes=[16], postfix="_ph")
            #denseInputLayer_ph = self.createCNNRNNLayers(NDENSEONLYVAR, NCONSTITUENTS, nChannel, transformedX_ph, convWeights=[], rnnNodes=[16], postfix="_ph")

        else:
            #If convolution is not used, just pass in the transformed input variables 
            denseInputLayer = transformedX
            denseInputLayer_ph = transformedX_ph


        #variables for weights and activation functions 
        self.w_fc = {}
        self.b_fc = {}

        #create dense network
        self.yt = self.createDenseNetwork(denseInputLayer, self.nnStruct, self.w_fc, self.b_fc)
        self.yt_ph = self.createDenseNetwork(denseInputLayer_ph, self.nnStruct, self.w_fc, self.b_fc, "_ph")
    
        #final answer with softmax applied for the end user
        self.y = tf.nn.softmax(self.yt, name="y")
        self.y_ph = tf.nn.softmax(self.yt_ph, name="y_ph")
    
    def createLoss(self):
        # other placeholders 
        self.reg = tf.placeholder(tf.float32)
        
        tf.add_to_collection('TrainInfo', self.x)
        tf.add_to_collection('TrainInfo', self.y)
        
        self.cross_entropy    = tf.losses.compute_weighted_loss(losses=tf.nn.softmax_cross_entropy_with_logits(labels=self.y_,    logits=self.yt),    weights=tf.reshape(self.wgt, [-1]),    reduction=tf.losses.Reduction.MEAN)
        self.cross_entropy_ph = tf.losses.compute_weighted_loss(losses=tf.nn.softmax_cross_entropy_with_logits(labels=self.y_ph_, logits=self.yt_ph), weights=tf.reshape(self.wgt_ph, [-1]), reduction=tf.losses.Reduction.MEAN)
        self.l2_norm = tf.constant(0.0)
        for w in self.w_fc.values():
          self.l2_norm += tf.nn.l2_loss(w)
        self.loss = self.cross_entropy + self.l2_norm*self.reg
        self.loss_ph = self.cross_entropy_ph + self.l2_norm*self.reg
        #train_step = tf.train.GradientDescentOptimizer(1.0).minimize(cross_entropy)
        self.train_step = tf.train.AdamOptimizer(1e-3).minimize(self.loss)#, var_list=self.w_fc.values() + self.b_fc.values())


    def createSummaries(self):
        correct_prediction = tf.equal(tf.argmax(self.y_ph, 1), tf.argmax(self.y_ph_, 1))
        self.accuracy = tf.reduce_mean(tf.cast(correct_prediction, tf.float32))

        # Create a summary to monitor cost tensor
        summary_ce = tf.summary.scalar("cross_entropy", self.cross_entropy)
        summary_l2n = tf.summary.scalar("l2_norm", self.l2_norm)
        summary_loss = tf.summary.scalar("loss", self.loss)
        summary_queueSize = tf.summary.scalar("queue_size", self.inputDataQueue.size())
        summary_vloss = tf.summary.scalar("valid_loss", self.loss_ph)
        # Create a summary to monitor accuracy tensor
        summary_accuracy = tf.summary.scalar("accuracy", self.accuracy)
        # create image of colvolutional filters 
        valid_summaries = [summary_accuracy, summary_vloss]
        try:
            for i in xrange(len(convWeights)):
                shapes = self.convWeights[i].shape
                valid_summaries.append(tf.summary.image("conv_wgt_%i"%i, tf.reshape(self.convWeights[0], [int(shapes[0]), int(shapes[1]), int(shapes[2]), 1])))
        except NameError:
            pass
        # Merge all summaries into a single op
        self.merged_train_summary_op = tf.summary.merge([summary_ce, summary_l2n, summary_loss, summary_queueSize])
        self.merged_valid_summary_op = tf.summary.merge(valid_summaries)


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


class FileNameQueue:
    #This class is designed to store and randomize a filelist for use by several CustomRunner objects
    def __init__(self, files, nEpoch, nFeatures, nLabels, nWeigts, nEnqueuers = 4, batchSize = 128):
        self.files = numpy.array(files)
        self.nEpoch = nEpoch

        #filename queue
        self.fileQueue = Queue.Queue(self.files.shape[0])

        #Define input data queue
        #inputDataQueue = tf.FIFOQueue(capacity=512*32, shapes=[[16], [2], [1]], dtypes=[tf.float32, tf.float32, tf.float32])
        self.inputDataQueue = tf.RandomShuffleQueue(capacity=16284, min_after_dequeue=16284 - batchSize*(nEnqueuers + 2), shapes=[[nFeatures], [nLabels], [nWeigts]], dtypes=[tf.float32, tf.float32, tf.float32])

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
                self.fileQueue.put(fileName)

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


class CustomRunner(object):
    """
    This class manages the background threads needed to fill
        a queue full of data.
    """
    def __init__(self, batchSize, variables, fileQueue, ptReweight=True):

        self.fileQueue = fileQueue
        # The actual queue of data. 
        self.queueX = self.fileQueue.inputDataQueue

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
      dg = DataGetter(self.variables)
            
      #loop until there are no more files to get from the queue
      for fileName in fIter:
        #read next file
        data = dg.importData([fileName], ptReweight=self.ptReweight)
        
        batch_idx = 0
        while batch_idx + self.batch_size <= data["data"].shape[0]:
            yield data["data"][batch_idx:batch_idx+self.batch_size], data["labels"][batch_idx:batch_idx+self.batch_size], data["weights"][batch_idx:batch_idx+self.batch_size]
            batch_idx += self.batch_size

      return


    def thread_main(self, sess):
      """
      Function run on alternate thread. Basically, keep adding data to the queue.
      """
      for dataX, dataY, dataW in self.data_iterator():
        sess.run([self.enqueue_opX], feed_dict={self.dataX:dataX, self.dataY:dataY, self.dataW:dataW})

    def start_threads(self, sess, n_threads=1):
      """ Start background threads to feed queue """
      threads = []
      for n in range(n_threads):
        t = threading.Thread(target=self.thread_main, args=(sess,))
        #p.daemon = True # thread will close when parent quits
        t.start()
        threads.append(t)

      #pass threads to FileQueue for management
      self.fileQueue.addCustomRunnerThreads(threads)
      return threads

