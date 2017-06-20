import ROOT
import numpy
import math
import tensorflow as tf

class DataGetter:

    def __init__(self):
        self.list = ["cand_m", "j12_m", "j13_m", "j23_m", "j1_p", "j2_p", "j3_p", "dTheta12", "dTheta23", "dTheta13", "j1_CSV", "j2_CSV", "j3_CSV", "j1_QGL", "j2_QGL", "j3_QGL"]
        #self.list = ["cand_m", "j12_m_lab", "j13_m_lab", "j23_m_lab", "j1_p", "j2_p", "j3_p", "dTheta12", "dTheta23", "dTheta13", "j1_CSV_lab", "j2_CSV_lab", "j3_CSV_lab", "j1_QGL_lab", "j2_QGL_lab", "j3_QGL_lab", "dRPtTop", "dRPtW", "sd_n2"]
        #self.list = ["j1_m_lab", "j1_CSV_lab", "j2_CSV_lab", "j2_QGL_lab", "j3_CSV_lab", "j3_QGL_lab", "cand_m", "dRPtTop", "j23_m_lab", "dRPtW", "j12_m_lab", "j13_m_lab", "sd_n2"]
        #self.list = ["cand_m", "j12_m", "j13_m", "j23_m", "j1_p", "j2_p", "j3_p", "dTheta12", "dTheta23", "dTheta13", "j1_CSV", "j2_CSV", "j3_CSV", "j1_qgAxis1_lab", "j1_qgMult_lab", "j1_qgPtD_lab", "j2_qgAxis1_lab", "j2_qgMult_lab", "j2_qgPtD_lab", "j3_qgAxis1_lab", "j3_qgMult_lab", "j3_qgPtD_lab"]
        #self.list = ["j1_m_lab", "j1_CSV_lab", "j2_CSV_lab", "j3_CSV_lab", "cand_m", "dRPtTop", "j23_m_lab", "dRPtW", "j12_m_lab", "j13_m_lab", "sd_n2", "j2_qgAxis1_lab", "j2_qgMult_lab", "j2_qgPtD_lab", "j3_qgAxis1_lab", "j3_qgMult_lab", "j3_qgPtD_lab"]
        #self.list = ["cand_m", "cand_pt", "j12_m_lab", "j13_m_lab", "j23_m_lab", "j1_p", "j2_p", "j3_p", "dTheta12", "dTheta23", "dTheta13", "j1_CSV_lab", "j2_CSV_lab", "j3_CSV_lab", "dRPtTop", "dRPtW", "sd_n2", "j2_qgAxis1_lab", "j2_qgMult_lab", "j2_qgPtD_lab", "j3_qgAxis1_lab", "j3_qgMult_lab", "j3_qgPtD_lab"]
        #self.list = ["cand_m", "cand_pt", "j12_m", "j13_m", "j23_m", "j1_p", "j2_p", "j3_p", "dTheta12", "dTheta23", "dTheta13", "j1_CSV", "j2_CSV", "j3_CSV", "j1_qgAxis1_lab", "j1_qgMult_lab", "j1_qgPtD_lab", "j2_qgAxis1_lab", "j2_qgMult_lab", "j2_qgPtD_lab", "j3_qgAxis1_lab", "j3_qgMult_lab", "j3_qgPtD_lab"]
        self.list2 = ["event." + v for v in self.list]

        self.theStrCommand = "numpy.array([" + ", ".join(self.list2) + "]).transpose()"

    def getData(self, event):
        array = eval(self.theStrCommand)
        array[array<0]=0
        return array

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
def createMLP(nnStruct, offset_initial, scale_initial):
    #constants 
    NLayer = len(nnStruct)

    if len(nnStruct) < 2:
        throw
    
    #Define input queues
    inputDataQueue = tf.FIFOQueue(capacity=512, shapes=[16], dtypes=tf.float32)
    inputAnsQueue = tf.FIFOQueue(capacity=512, shapes=[2], dtypes=tf.float32)

    #Define inputs and training inputs
    x_ph = tf.placeholder(tf.float32, [None, nnStruct[0]], name="x")
    y_ph_ = tf.placeholder(tf.float32, [None, nnStruct[NLayer - 1]])
    x = inputDataQueue.dequeue_many(n=128)
    y_ = inputAnsQueue.dequeue_many(n=128)

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

    return x, y_, y, yt, w_fc, b_fc, inputDataQueue, inputAnsQueue, x_ph, y_ph_, yt_ph, y_ph
    

def HEPReqs(event, i):
    Mw = 80.385
    Mt = 173.5
    Rmin_ = 0.85 *(Mw/Mt)
    Rmax_ = 1.25 *(Mw/Mt)
    CSV_ = 0.800
    minTopCandMass_ = 100
    maxTopCandMass_ = 250
    dRMax_ = 1.5

    #HEP tagger requirements
    passHEPRequirments = True

    #Get the total candidate mass
    m123 = event.cand_m[i]

    m12  = event.j12_m[i];
    m23  = event.j23_m[i];
    m13  = event.j13_m[i];
    dRMax = event.cand_dRMax[i]
    
    #HEP Pre requirements
    passPreRequirments = True
    passMassWindow = (minTopCandMass_ < m123) and (m123 < maxTopCandMass_)
    passPreRequirments = passMassWindow and dRMax < dRMax_

    #Implement HEP mass ratio requirements here
    criterionA = 0.2 < math.atan(m13/m12) and math.atan(m13/m12) < 1.3 and Rmin_ < m23/m123 and m23/m123 < Rmax_

    criterionB = ((Rmin_**2)*(1+(m13/m12)**2) < (1 - (m23/m123)**2)) and ((1 - (m23/m123)**2) < (Rmax_**2)*(1 + (m13/m12)**2))

    criterionC = ((Rmin_**2)*(1+(m12/m13)**2) < (1 - (m23/m123)**2)) and ((1 - (m23/m123)**2) < (Rmax_**2)*(1 + (m12/m13)**2))

    passHEPRequirments = criterionA or criterionB or criterionC;

    passBreq = (int(event.j1_CSV[i] > CSV_) + int(event.j2_CSV[i] > CSV_) + int(event.j3_CSV[i] > CSV_)) <= 1

   # return passHEPRequirments and passBreq
    return passPreRequirments and passHEPRequirments and passBreq

class simpleTopCand:
    def __init__(self, event, i, discriminator):
        self.j1 = ROOT.TLorentzVector()
        self.j2 = ROOT.TLorentzVector()
        self.j3 = ROOT.TLorentzVector()
        self.j1.SetPtEtaPhiM(event.j1_pt_lab[i], event.j1_eta_lab[i], event.j1_phi_lab[i], event.j1_m[i])
        self.j2.SetPtEtaPhiM(event.j2_pt_lab[i], event.j2_eta_lab[i], event.j2_phi_lab[i], event.j2_m[i])
        self.j3.SetPtEtaPhiM(event.j3_pt_lab[i], event.j3_eta_lab[i], event.j3_phi_lab[i], event.j3_m[i])
        self.discriminator = discriminator

    def __lt__(self, other):
        return self.discriminator < other.discriminator

def jetInList(jet, jlist):
    for j in jlist:
        if(abs(jet.M() - j.M()) < 0.0001):
            return True
    return False

def resolveOverlap(event, discriminators, threshold):
    topCands = [simpleTopCand(event, i, discriminators[i]) for i in xrange(len(event.j1_pt_lab))]
    topCands.sort(reverse=True)

    finalTops = []
    usedJets = []
    for cand in topCands:
        #if not cand.j1 in usedJets and not cand.j2 in usedJets and not cand.j3 in usedJets:
        if not jetInList(cand.j1, usedJets) and not jetInList(cand.j2, usedJets) and not jetInList(cand.j3, usedJets):
            if cand.discriminator > threshold:
                usedJets += [cand.j1, cand.j2, cand.j3]
                finalTops.append(cand)

    return finalTops

class simpleTopCandHEP:
    def __init__(self, event, i, passFail):
        self.j1 = ROOT.TLorentzVector()
        self.j2 = ROOT.TLorentzVector()
        self.j3 = ROOT.TLorentzVector()
        self.j1.SetPtEtaPhiM(event.j1_pt_lab[i], event.j1_eta_lab[i], event.j1_phi_lab[i], event.j1_m[i])
        self.j2.SetPtEtaPhiM(event.j2_pt_lab[i], event.j2_eta_lab[i], event.j2_phi_lab[i], event.j2_m[i])
        self.j3.SetPtEtaPhiM(event.j3_pt_lab[i], event.j3_eta_lab[i], event.j3_phi_lab[i], event.j3_m[i])
        self.cand_m = event.cand_m[i]
        self.passHEP = passFail

    def __lt__(self, other):
        return abs(self.cand_m - 173.4) < abs(other.cand_m - 173.4)

def resolveOverlapHEP(event, passFail):
    topCands = [simpleTopCandHEP(event, i, passFail[i]) for i in xrange(len(event.j1_pt_lab))]
    topCands.sort(reverse=True)

    finalTops = []
    usedJets = []
    for cand in topCands:
        #if not cand.j1 in usedJets and not cand.j2 in usedJets and not cand.j3 in usedJets:
        if not jetInList(cand.j1, usedJets) and not jetInList(cand.j2, usedJets) and not jetInList(cand.j3, usedJets):
            if cand.passHEP:
                usedJets += [cand.j1, cand.j2, cand.j3]
                finalTops.append(cand)

    return finalTops

NEVTS = 1e10
NEVTS_Z = 1e10

