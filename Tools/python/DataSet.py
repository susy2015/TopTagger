
class DataSet:
    def __init__(self, fileGlob, xsec, Nevts, kFactor, sig, domain, prescale, rescale, nEnqueueThreads, weightHist=None, include=True):
        self.fileGlob = fileGlob
        self.xsec = xsec
        self.Nevts = Nevts
        self.kFactor = kFactor
        self.sig = sig
        self.domain = domain
        self.prescale = prescale
        self.rescale = rescale
        self.nEnqueueThreads = nEnqueueThreads
        self.weightHist = weightHist
        self.include = include
