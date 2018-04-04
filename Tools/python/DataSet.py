
class DataSet:
    def __init__(self, fileGlob, xsec, Nevts, kFactor, sig, prescale, rescale):
        self.fileGlob = fileGlob
        self.xsec = xsec
        self.Nevts = Nevts
        self.kFactor = kFactor
        self.sig = sig
        self.prescale = prescale
        self.rescale = rescale
