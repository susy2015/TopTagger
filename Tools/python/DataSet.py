
class DataSet:
    def __init__(self, fileGlob, xsec, Nevts, kFactor, sig, domain, prescale, rescale, nEnqueueThreads):
        self.fileGlob = fileGlob
        self.xsec = xsec
        self.Nevts = Nevts
        self.kFactor = kFactor
        self.sig = sig
        self.domain = domain
        self.prescale = prescale
        self.rescale = rescale
        self.nEnqueueThreads = nEnqueueThreads
