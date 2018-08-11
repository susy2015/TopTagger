import os
import tensorflow as tf

class CreateModel:
    def weight_variable(self, shape, name):
        """weight_variable generates a weight variable of a given shape."""
        initial = tf.truncated_normal(shape, stddev=2.0/shape[0], name=name)
        return tf.Variable(initial)
    
    def bias_variable(self, shape, name):
        """bias_variable generates a bias variable of a given shape."""
        initial = tf.constant(0.00, shape=shape, name=name)
        return tf.Variable(initial)
    
    def createRecurentLayers(self, inputs, nodes=0, layers=0, keep_prob=1.0, share=None):
        #define variable scope
        with tf.variable_scope("rnn") as scope:
            #condition input shape 
            slicedInputs = []
#            for iW in reversed(range(inputs.shape[1])):
            for iW in range(inputs.shape[1]):
                slicedInputs.append(tf.reshape(tf.slice(inputs, [0,iW,0], [-1, 1, -1]), [-1, int(inputs.shape[2])]))
            #create the rnn cell
            cell = tf.nn.rnn_cell.MultiRNNCell([tf.nn.rnn_cell.BasicLSTMCell(nodes, reuse=share, state_is_tuple=True) for _ in xrange(layers)], state_is_tuple=True)
            #add dropout
            cell = tf.nn.rnn_cell.DropoutWrapper(cell, input_keep_prob=1.0, output_keep_prob=keep_prob)
            #create the rnn layer 
            output, _ = tf.nn.static_rnn(cell, slicedInputs, dtype=tf.float32, scope=scope)
            #reshape output to match the cnn output
            output = tf.stack(output, axis=1) 
            return output

    def createConvLayers(self, inputs, convWeights, convBiases, keep_prob=1.0, postfix=""):
        with tf.variable_scope("cnn") as scope:
            #list to hold conv layers 
            convLayers = [inputs]

            #create the convolutional layers
            for iLayer in xrange(len(convWeights)):
                convLayers.append(tf.nn.dropout(tf.nn.relu(tf.nn.conv1d(value = convLayers[iLayer], filters=convWeights[iLayer], stride=1, data_format='NHWC', padding='SAME', name="conv1d"+postfix) + convBiases[iLayer]), keep_prob))
    
            return convLayers[-1]

    def createCNNRNNLayers(self, NDENSEONLYVAR, NCONSTITUENTS, nChannel, inputVars, convWeights=[], convBiases=[], rnnNodes=0, rnnLayers=0, keep_prob=1.0, postfix=""):
        #prep inputs by splitting apart dense only variables from convolution variables and reshape convolution variables
        dInputs = tf.slice(inputVars, [0,0], [-1, NDENSEONLYVAR])
        cProtoInputs = tf.slice(inputVars, [0,NDENSEONLYVAR], [-1, -1])
        cInputs = tf.reshape(cProtoInputs, [-1, NCONSTITUENTS, nChannel])
        
        output = cInputs

        if len(convWeights) > 0:
            output = self.createConvLayers(output, convWeights, convBiases, keep_prob, postfix)

        if rnnNodes > 0 and rnnLayers > 0:
            output = self.createRecurentLayers(output, rnnNodes, rnnLayers, keep_prob, len(postfix)>0)

        #Reshape convolutional output and recombine with the variables which bypass the convolution stage 
        convLayerShape = output.shape[1] * output.shape[2]
        flatConvLayer = tf.reshape(output, shape=[-1, int(convLayerShape)])
        denseInputLayer = tf.concat([dInputs, flatConvLayer], axis=1, name="dil"+postfix)

        return denseInputLayer

    def gradientReversal(self, input, parameter):
        gradientOp = -parameter*tf.identity(input, name="Identity")
        return gradientOp + tf.stop_gradient(tf.identity(input) - gradientOp)

    def createDenseNetwork(self, denseInputLayer, nnStruct, w_fc = {}, b_fc = {}, keep_prob=1.0, gradientReversalWeight=1.0, training = False, prefix="", nDomainNode=2):
        with tf.variable_scope("dense") as scope:
            #constants 
            NLayer = len(nnStruct)
            share = len(prefix) > 0
            
            #variables
            h_fc = {}

            # Fully connected input layer
            if not 0 in w_fc:
                w_fc[0] = self.weight_variable([int(denseInputLayer.shape[1]), nnStruct[1]], name="w_fc0")
            if not 0 in b_fc:
                b_fc[0] = self.bias_variable([nnStruct[1]], name="b_fc0")
            
            #h_fc[0] = tf.nn.dropout(denseInputLayer, keep_prob)
            h_fc[0] = tf.layers.batch_normalization(denseInputLayer, training=training, reuse=share, trainable=not share, name="layer0_bn")
            
            # create hidden layers 
            for layer in xrange(1, NLayer - 1):
                #use relu for hidden layers as this seems to give best result
                addResult = tf.add(tf.matmul(h_fc[layer - 1], w_fc[layer - 1], name="z_fc%i%s"%(layer,prefix)),  b_fc[layer - 1], name="a_fc%i%s"%(layer,prefix))
                #add batch normalization 
                batchNormalizedLayer = tf.layers.batch_normalization(addResult, training=training, reuse=share, trainable=not share, name="layer%i_bn"%layer)
                if self.options.netOp.denseActivationFunc == "relu":
                    layerOutput = tf.nn.relu(batchNormalizedLayer, name="h_fc%i%s"%(layer,prefix))
                elif self.options.netOp.denseActivationFunc == "sigmoid":
                    layerOutput = tf.nn.sigmoid(batchNormalizedLayer, name="h_fc%i%s"%(layer,prefix))
                elif self.options.netOp.denseActivationFunc == "tanh":
                    layerOutput = tf.nn.tanh(batchNormalizedLayer, name="h_fc%i%s"%(layer,prefix))
                elif self.options.netOp.denseActivationFunc == "none":
                    layerOutput = batchNormalizedLayer
                #add dropout 
                h_fc[layer] = tf.nn.dropout(layerOutput, keep_prob)
                
                # Map the features to next layer
                if not layer in w_fc:
                    w_fc[layer] = self.weight_variable([nnStruct[layer], nnStruct[layer + 1]], name="w_fc%i"%(layer))
                if not layer in b_fc:
                    b_fc[layer] = self.bias_variable([nnStruct[layer + 1]], name="b_fc%i"%(layer))
                    
            #create yt for input to the softmax cross entropy for classification (this should not have softmax applied as the loss function will do this)
            # for primary class classification
            yt = tf.add(tf.matmul(h_fc[NLayer - 2], w_fc[NLayer - 2]),  b_fc[NLayer - 2], name="yt"+prefix)

            #Adding the hidden layers to domain classification
            layer = NLayer - 1
            HArch = [20]
            GRHArch = [nnStruct[layer - 1]] + HArch 
            
            #apply gradient reversal 
            h_fc[layer - 1] = self.gradientReversal(h_fc[layer - 1], gradientReversalWeight)
            
            for i in xrange(1, len(GRHArch)):
                if not layer in w_fc:
                    w_fc[layer] = self.weight_variable([GRHArch[i - 1], GRHArch[i]], name="w_fc%i"%(layer))
                if not layer in b_fc:
                    b_fc[layer] = self.bias_variable([GRHArch[i]], name="b_fc%i"%(layer))
                addResult = tf.add(tf.matmul(h_fc[layer - 1], w_fc[layer], name="z_fc%i%s"%(layer,prefix)),  b_fc[layer], name="a_fc%i%s"%(layer,prefix))
                batchNormalizedLayer = tf.layers.batch_normalization(addResult, training=training, reuse=share, trainable=not share, name="layer%i_bn"%layer)
                layerOutput = tf.nn.relu(batchNormalizedLayer, name="h_fc%i%s"%(layer, prefix))
                h_fc[layer] = tf.nn.dropout(layerOutput, keep_prob)
                layer += 1
            
            #create pt for domain classification            
            if not layer in w_fc:
                w_fc[layer] = self.weight_variable([GRHArch[-1], int(nDomainNode)], name="w_fc%i"%(layer))
            if not layer in b_fc:
                b_fc[layer] = self.bias_variable([int(nDomainNode)], name="b_fc%i"%(layer))
                        
            pt = tf.add(tf.matmul(h_fc[layer - 1], w_fc[layer]),  b_fc[layer], name="pt"+prefix)

            return yt, pt

    ### createMLP
    # This fucntion is designed to create a MLP for classification purposes (using softmax_cross_entropy_with_logits)
    # inputs 
    #  nnStruct - a list containing the number of nodes in each layer, including the input and output layers 
    #  offset_initial - a list of offsets which will be applied to the initial input features, they are stored in the tf model
    #  scale_initial - a list of scales which will be applied to each input feature after the offsets are subtracted, they are stored in the tf model
    def createMLP(self):
        #constants 
        NLayer = len(self.nnStruct)
    
        if len(self.nnStruct) < 2:
            throw
        
        self.keep_prob = tf.placeholder_with_default(1.0, [], name="keep_prob")
        self.training = tf.placeholder_with_default(False, [], name="training")
        self.gradientReversalWeight = tf.placeholder_with_default(1.0, [], name="gradientReversalWeight")

        #define a StagingArea here
        stagingAreaShape = [[self.nBatch, int(a.dims[0])] for a in self.inputDataQueue.shapes]
        self.stagingArea = tf.contrib.staging.StagingArea(self.inputDataQueue.dtypes, shapes=stagingAreaShape)
        self.stagingOp = self.stagingArea.put(self.inputDataQueue.dequeue_many(n=self.nBatch))
        self.x, self.y_, self.p_, self.wgt = self.stagingArea.get()

        #Define inputs and training inputs
        self.x_ph = tf.placeholder(tf.float32, [None, self.nnStruct[0]], name="x")
        self.y_ph_ = tf.placeholder(tf.float32, [None, self.nnStruct[NLayer - 1]], name="y_ph_")
        self.p_ph_ = tf.placeholder(tf.float32, [None, self.p_.shape[1]], name="p_ph_")
        self.wgt_ph = tf.placeholder(tf.float32, [None, 1], name="wgt_ph")

        #self.x, self.y_, self.wgt = self.inputDataQueue.dequeue_many(n=self.nBatch)

        #variables for pre-transforming data
        self.offset = tf.constant(self.offset_initial, name="offset")
        self.scale = tf.constant(self.scale_initial, name="scale")

        #input variables after rescaling 
        transformedX = tf.multiply(self.x-self.offset,self.scale)
        transformedX_ph = tf.multiply(self.x_ph-self.offset,self.scale)

        if len(self.convLayers) or self.rnnNodes > 0:
            #Implement 1D convolution layer here
            #NDENSEONLYVAR = 8
            #NCONSTITUENTS = 3
            #FILTERWIDTH = 1

            NDENSEONLYVAR = self.options.netOp.convNDenseOnlyVar
            NCONSTITUENTS = self.options.netOp.convNConstituents
            FILTERWIDTH   = self.options.netOp.convFilterWidth
            nChannel      = self.options.netOp.convNChannels

            #weights for convolution filters - shared between all parallel graphs
            self.convWeights = []
            self.convBiases = []
            cnnStruct = [nChannel] + self.convLayers
            for i in xrange(len(cnnStruct) - 1):
                self.convWeights.append(tf.Variable(tf.random_normal([FILTERWIDTH, cnnStruct[i], cnnStruct[i + 1]]), name="conv1_weights"))
                self.convBiases.append(self.bias_variable([cnnStruct[i + 1]], name="conv1_biasses"))


            #Create colvolution layers 
            denseInputLayer    = self.createCNNRNNLayers(NDENSEONLYVAR, NCONSTITUENTS, nChannel, transformedX,    convWeights=self.convWeights, convBiases=self.convBiases, rnnNodes=self.rnnNodes, rnnLayers=self.rnnLayers, keep_prob=self.keep_prob, postfix="")
            denseInputLayer_ph = self.createCNNRNNLayers(NDENSEONLYVAR, NCONSTITUENTS, nChannel, transformedX_ph, convWeights=self.convWeights, convBiases=self.convBiases, rnnNodes=self.rnnNodes, rnnLayers=self.rnnLayers, keep_prob=self.keep_prob, postfix="_ph")

        else:
            #If convolution is not used, just pass in the transformed input variables 
            denseInputLayer = transformedX
            denseInputLayer_ph = transformedX_ph


        #variables for weights and activation functions 
        self.w_fc = {}
        self.b_fc = {}

        #create dense network
        self.yt,    self.pt    = self.createDenseNetwork(denseInputLayer,    self.nnStruct, self.w_fc, self.b_fc, keep_prob=self.keep_prob, gradientReversalWeight=self.gradientReversalWeight, training=self.training, nDomainNode=self.p_.shape[1])
        self.yt_ph, self.pt_ph = self.createDenseNetwork(denseInputLayer_ph, self.nnStruct, self.w_fc, self.b_fc, keep_prob=self.keep_prob, gradientReversalWeight=self.gradientReversalWeight, training=self.training, prefix="_ph", nDomainNode=self.p_.shape[1])
    
        #final answer with softmax applied for the end user
        self.y = tf.nn.softmax(self.yt, name="y")
        self.y_ph = tf.nn.softmax(self.yt_ph, name="y_ph")
        self.p = tf.nn.softmax(self.pt, name="p")
    
    def createLoss(self):
        # other placeholders 
        self.reg = tf.placeholder(tf.float32)
        
        tf.add_to_collection('TrainInfo', self.x)
        tf.add_to_collection('TrainInfo', self.y)
        
        #self.cross_entropy    = tf.losses.compute_weighted_loss(losses=tf.nn.softmax_cross_entropy_with_logits(labels=self.y_,    logits=self.yt),    weights=tf.reshape(self.wgt, [-1]),    reduction=tf.losses.Reduction.MEAN)
        #self.cross_entropy_ph = tf.losses.compute_weighted_loss(losses=tf.nn.softmax_cross_entropy_with_logits(labels=self.y_ph_, logits=self.yt_ph), weights=tf.reshape(self.wgt_ph, [-1]), reduction=tf.losses.Reduction.MEAN)
        #self.cross_entropy    = tf.losses.softmax_cross_entropy(onehot_labels=self.y_,    logits=self.yt,    weights=tf.reshape(self.wgt, [-1]))
        #self.cross_entropy_ph = tf.losses.softmax_cross_entropy(onehot_labels=self.y_ph_, logits=self.yt_ph, weights=tf.reshape(self.wgt_ph, [-1]))
        self.cross_entropy    = tf.losses.softmax_cross_entropy(onehot_labels=self.y_,    logits=self.yt)
        self.cross_entropy_ph = tf.losses.softmax_cross_entropy(onehot_labels=self.y_ph_, logits=self.yt_ph)

        self.cross_entropy_d    = tf.losses.softmax_cross_entropy(onehot_labels=self.p_,    logits=self.pt,    weights=tf.reduce_sum(self.p_, axis=1))
        self.cross_entropy_d_ph = tf.losses.softmax_cross_entropy(onehot_labels=self.p_ph_, logits=self.pt_ph, weights=tf.reduce_sum(self.p_ph_, axis=1))

        self.l2_norm = tf.constant(0.0)
        for w in self.w_fc.values():
          self.l2_norm += tf.nn.l2_loss(w)
        self.loss = self.cross_entropy_d + self.cross_entropy + self.l2_norm*self.reg
        self.loss_ph = self.cross_entropy_d_ph + self.cross_entropy_ph + self.l2_norm*self.reg

        #these operations are necessary to run batch normalization 
        extra_update_ops = tf.get_collection(tf.GraphKeys.UPDATE_OPS)
        # forgive the hack which removes the operations associated with the placeholder copy of the network
        #if another copy of the network were added /2 would need to be /3
        self.batch_norm_ops = extra_update_ops[:len(extra_update_ops)/2]

        #attach the batch norm update ops to the training step 
        with tf.control_dependencies(self.batch_norm_ops):
            # Ensures that we execute the update_ops before performing the train_step
            self.train_step = tf.train.AdamOptimizer(1.0e-5).minimize(self.loss)#, var_list=self.w_fc.values() + self.b_fc.values())



    def createSummaries(self):
        correct_prediction = tf.equal(tf.argmax(self.y_ph, 1), tf.argmax(self.y_ph_, 1))
        self.accuracy = tf.reduce_mean(tf.cast(correct_prediction, tf.float32))
        correct_prediction_train = tf.equal(tf.argmax(self.y, 1), tf.argmax(self.y_, 1))
        self.accuracy_train = tf.reduce_mean(tf.cast(correct_prediction_train, tf.float32))
        correct_prediction_domain = tf.equal(tf.argmax(self.p, 1), tf.argmax(self.p_, 1))
        self.accuracy_domain = tf.reduce_mean(tf.cast(correct_prediction_domain, tf.float32))

        summary_grw = tf.summary.scalar("gradient reversal weight",  self.gradientReversalWeight)
        summary_signalFrac = tf.summary.scalar("signalBatchFrac",  tf.reduce_mean(self.y_, axis=0)[0])
        summary_domainFrac = tf.summary.scalar("domainBatchFrac",  tf.reduce_mean(self.p_, axis=0)[0])

        # Create a summary to monitor cost tensor
        summary_ce = tf.summary.scalar("cross_entropy", self.cross_entropy)
        summary_ce_d = tf.summary.scalar("cross_entropy_domain", self.cross_entropy_d)
        summary_l2n = tf.summary.scalar("l2_norm", self.l2_norm)
        summary_loss = tf.summary.scalar("loss", self.loss)
        summary_queueSize = tf.summary.scalar("queue_size", self.inputDataQueue.size())
        summary_vce = tf.summary.scalar("valid_cross_entropy", self.cross_entropy_ph)
        summary_vloss = tf.summary.scalar("valid_loss", self.loss_ph)
        # Create a summary to monitor accuracy tensor
        summary_accuracy = tf.summary.scalar("accuracy", self.accuracy_train)
        summary_daccuracy = tf.summary.scalar("domain accuracy", self.accuracy_domain)
        summary_vaccuracy = tf.summary.scalar("valid accuracy", self.accuracy)

        #special validations
        summary_vce_QCDMC = tf.summary.scalar("valid_cross_entropy_sig2", self.cross_entropy_ph)
        summary_vaccuracy_QCDMC = tf.summary.scalar("valid accuracy_sig2", self.accuracy)

        summary_vce_QCDData = tf.summary.scalar("valid_cross_entropy_sig3", self.cross_entropy_ph)
        summary_vaccuracy_QCDData = tf.summary.scalar("valid accuracy_sig3", self.accuracy)

        # create image of colvolutional filters 
        valid_summaries = [summary_vaccuracy, summary_vloss, summary_vce]
        valid_summaries_QCDMC = [summary_vce_QCDMC, summary_vaccuracy_QCDMC]
        valid_summaries_QCDData = [summary_vce_QCDData, summary_vaccuracy_QCDData]
        try:
            for i in xrange(len(self.convWeights)):
                shapes = self.convWeights[i].shape
                valid_summaries.append(tf.summary.image("conv_wgt_%i"%i, tf.reshape(self.convWeights[i], [int(shapes[0]), int(shapes[1]), int(shapes[2]), 1])))
        except NameError:
            pass
        except AttributeError:
            pass
        # Merge all summaries into a single op
        self.merged_train_summary_op = tf.summary.merge([summary_ce, summary_ce_d, summary_l2n, summary_loss, summary_queueSize, summary_accuracy, summary_daccuracy, summary_grw, summary_signalFrac, summary_domainFrac])
        self.merged_valid_summary_op = tf.summary.merge(valid_summaries)
        self.merged_valid_QCDMC_summary_op = tf.summary.merge(valid_summaries_QCDMC)
        self.merged_valid_QCDData_summary_op = tf.summary.merge(valid_summaries_QCDData)


    def __init__(self, options, nnStruct, convLayers, rnnNodes, rnnLayers, inputDataQueue, nBatch, offset_initial, scale_initial):
        self.options = options        

        self.nnStruct = nnStruct
        self.convLayers = convLayers
        self.rnnNodes = rnnNodes
        self.rnnLayers = rnnLayers
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


