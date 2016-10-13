CMSSW_BASE = /home/pastika/topTagger/

OPENCV_VERSION=opencv-3.1.0

IDIR       = .
ODIR       = obj
SDIR       = .
#RSDIR      = $(CMSSW_BASE)/src/SusyAnaTools/Tools
#
#TTIDIR     = $(CMSSW_BASE)/src/TopTagger/TopTagger/include
#TPIDIR     = $(CMSSW_BASE)/src/TopTagger/CfgParser/include

RSDIR      = $(CMSSW_BASE)/SusyAnaTools/Tools

TTIDIR     = $(CMSSW_BASE)/TopTagger/TopTagger/include
TPIDIR     = $(CMSSW_BASE)/TopTagger/CfgParser/include

CXX        = g++

#CXXFLAGS  += -I. -I$(CMSSW_BASE)/src -std=c++0x -I$(LHAPDF_DATA_PATH)/../../include
#CXXFLAGS  += -I. -I$(CMSSW_BASE)/ -I/home/pastika/Downloads/Shark-3.1.0/include -I/home/pastika/Downloads/$(OPENCV_VERSION)/include/opencv/ -I/home/pastika/Downloads/$(OPENCV_VERSION)/modules/video/include -I/home/pastika/Downloads/$(OPENCV_VERSION)/modules/objdetect/include -I/home/pastika/Downloads/$(OPENCV_VERSION)/modules/ml/include/ -std=c++0x
CXXFLAGS  += -I. -I$(CMSSW_BASE)/ -I/home/pastika/Downloads/Shark-3.1.0/include -I/home/pastika/Downloads/$(OPENCV_VERSION)/include/ -I/home/pastika/Downloads/$(OPENCV_VERSION)/modules/core/include -I/home/pastika/Downloads/$(OPENCV_VERSION)/modules/video/include -I/home/pastika/Downloads/$(OPENCV_VERSION)/modules/objdetect/include -I/home/pastika/Downloads/$(OPENCV_VERSION)/modules/ml/include/ -std=c++0x
## Optimization flag
CXXFLAGS += -g #-O3
## Enable the maximun warning
#CXXFLAGS += -Wall -Wextra -Weffc++ -g

## Include ROOT
CXXFLAGS  += $(shell root-config --cflags)

CXXDEPFLAGS = -MMD -MP

LD         = g++
LDFLAGS    =

LIBS       = $(shell root-config --glibs)
#TOPLIB     = -L$(CMSSW_BASE)/lib/${SCRAM_ARCH}/ -lTopTaggerTopTagger -lTopTaggerCfgParser
TOPLIB     = -L../TopTagger/test -lTopTagger

#OBJS       = $(patsubst %, $(ODIR)/%, $(OBJ))

#PROGRAMS = makeTrainingTuples sharkTest
PROGRAMS = openCVTest

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
	$(LD) $^ $(LIBS) $(TOPLIB) -o $@

#custom hack for compiling shark, will make bvbetter as time allows
sharkTest: $(ODIR)/NTupleReader.o $(ODIR)/SATException.o sharkTest.o 
	$(LD) $^ $(LIBS) $(TOPLIB) -I /home/pastika/Downloads/Shark-3.1.0/include/ -L /home/pastika/Downloads/Shark-3.1.0/lib/ -L /usr/lib/x86_64-linux-gnu/ -lshark -lboost_serialization -lgomp -o $@

openCVTest: $(ODIR)/NTupleReader.o $(ODIR)/SATException.o openCVTest.o 
	$(LD) $^ -L /home/pastika/Downloads/$(OPENCV_VERSION)/lib/ -lopencv_ml -lopencv_core $(LIBS) $(TOPLIB)  -o $@

clean:
	rm -f $(ODIR)/*.o $(ODIR)/*.so $(ODIR)/*.d $(PROGRAMS) core 

-include $(ODIR)/*.d
