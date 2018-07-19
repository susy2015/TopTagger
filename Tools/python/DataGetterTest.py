from glob import glob
from DataGetter import DataGetter
from taggerOptions import StandardVariables

globalVars, stuff = StandardVariables("TeamAlpha")
print globalVars
dg = DataGetter(globalVars, False, True)

dataFiles = glob("/cms/data/cmadrid/EventShapeTrainingData/trainingTuple_1660_division_2_TT_test_0.h5")
print tuple(dataFiles)
trainData = dg.importData(samplesToRun = tuple(dataFiles), prescale=True, ptReweight=1)
print trainData
