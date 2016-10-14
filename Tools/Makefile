IDIR       = .
ODIR       = obj
SDIR       = .
RSDIR      = $(CMSSW_BASE)/src/SusyAnaTools/Tools

TTIDIR     = $(CMSSW_BASE)/src/TopTagger/TopTagger/include
TPIDIR     = $(CMSSW_BASE)/src/TopTagger/CfgParser/include

OPENCV_DIRECTORY=$(CMSSW_BASE)/src/opencv

CXX        = g++

CXXFLAGS  += -I. -I$(CMSSW_BASE)/src -I$(OPENCV_DIRECTORY)/include/ -I$(OPENCV_DIRECTORY)/modules/core/include -I$(OPENCV_DIRECTORY)/modules/video/include -I$(OPENCV_DIRECTORY)/modules/objdetect/include -I$(OPENCV_DIRECTORY)/modules/ml/include/ -I$(OPENCV_DIRECTORY)/modules/photo/include/ -I$(OPENCV_DIRECTORY)/modules/imgproc/include/ -std=c++0x

## Optimization flag
CXXFLAGS += -g #-O3
## Enable the maximun warning
#CXXFLAGS += -Wall -Wextra -Weffc++ -g

## Include ROOT
CXXFLAGS  += $(shell root-config --cflags)

CXXDEPFLAGS = -MMD -MP

LD         = g++
LDFLAGS    =

ROOTLIBS   = $(shell root-config --glibs)
MT2LIB     = -L$(CMSSW_BASE)/lib/${SCRAM_ARCH}/ -lrecipeAUXOxbridgeMT2
TOPLIB     = -L$(CMSSW_BASE)/lib/${SCRAM_ARCH}/ -lTopTaggerTopTagger -lTopTaggerCfgParser
OPENVCLIBS = -L$(OPENCV_DIRECTORY)/lib/ -lopencv_ml -lopencv_core

#OBJS       = $(patsubst %, $(ODIR)/%, $(OBJ))

PROGRAMS = makeTrainingTuples openCVTest # sharkTest

all: mkobj $(PROGRAMS)

mkobj:
	@mkdir -p obj

$(ODIR)/%.o : $(SDIR)/%.C
	$(CXX) $(CXXFLAGS) $(CXXDEPFLAGS) -I$(SDIR) -I$(TTIDIR) -I$(TPIDIR) -o $@ -c $<

$(ODIR)/%.o : $(SDIR)/%.cc
	$(CXX) $(CXXFLAGS) $(CXXDEPFLAGS) -I$(SDIR) -I$(TTIDIR) -I$(TPIDIR) -o $@ -c $<

$(ODIR)/%.o : $(SDIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(CXXDEPFLAGS) -I$(SDIR) -I$(TTIDIR) -I$(TPIDIR) -o $@ -c $<

$(ODIR)/%.o : $(RSDIR)/%.C
	$(CXX) $(CXXFLAGS) $(CXXDEPFLAGS) -I$(RSDIR) -I$(TTIDIR) -I$(TPIDIR) -o $@ -c $<

$(ODIR)/%.o : $(RSDIR)/%.cc
	$(CXX) $(CXXFLAGS) $(CXXDEPFLAGS) -I$(RSDIR) -I$(TTIDIR) -I$(TPIDIR) -o $@ -c $<

$(ODIR)/%.o : $(RSDIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(CXXDEPFLAGS) -I$(RSDIR) -I$(TTIDIR) -I$(TPIDIR) -o $@ -c $<

makeTrainingTuples: $(ODIR)/NTupleReader.o $(ODIR)/SATException.o $(ODIR)/TaggerUtility.o $(ODIR)/PlotUtility.o $(ODIR)/samples.o $(ODIR)/makeTrainingTuples.o $(ODIR)/MiniTupleMaker.o 
	$(LD) $^ $(MT2LIB) $(ROOTLIBS) $(TOPLIB) -o $@

#custom hack for compiling shark, will make bvbetter as time allows
sharkTest: $(ODIR)/NTupleReader.o $(ODIR)/SATException.o sharkTest.o 
	$(LD) $^ $(ROOTLIBS) $(TOPLIB) -I /home/pastika/Downloads/Shark-3.1.0/include/ -L /home/pastika/Downloads/Shark-3.1.0/lib/ -L /usr/lib/x86_64-linux-gnu/ -lshark -lboost_serialization -lgomp -o $@

openCVTest: $(ODIR)/NTupleReader.o $(ODIR)/SATException.o openCVTest.o 
	$(LD) $^ $(OPENVCLIBS) $(ROOTLIBS) $(TOPLIB)  -o $@

clean:
	rm -f $(ODIR)/*.o $(ODIR)/*.so $(ODIR)/*.d $(PROGRAMS) core 

-include $(ODIR)/*.d
