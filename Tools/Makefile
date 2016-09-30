IDIR       = .
ODIR       = obj
SDIR       = .
RSDIR      = $(CMSSW_BASE)/src/SusyAnaTools/Tools

TTIDIR     = $(CMSSW_BASE)/src/TopTagger/TopTagger/include
TPIDIR     = $(CMSSW_BASE)/src/TopTagger/CfgParser/include

CXX        = g++

CXXFLAGS  += -I. -I$(CMSSW_BASE)/src -std=c++0x -I$(LHAPDF_DATA_PATH)/../../include
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
MT2LIB     = -L$(CMSSW_BASE)/lib/${SCRAM_ARCH}/ -lrecipeAUXOxbridgeMT2 -lTopTaggerTopTagger -lTopTaggerCfgParser

#OBJS       = $(patsubst %, $(ODIR)/%, $(OBJ))

PROGRAMS = makeTrainingTuples

all: mkobj $(PROGRAMS)


mkobj:
	@mkdir -p obj

$(ODIR)/%.o : $(SDIR)/%.C
	$(CXX) $(CXXFLAGS) $(CXXDEPFLAGS) -I$(RSDIR) -I$(TTIDIR) -I$(TPIDIR) -o $@ -c $<

$(ODIR)/%.o : $(SDIR)/%.cc
	$(CXX) $(CXXFLAGS) $(CXXDEPFLAGS) -I$(RSDIR) -I$(TTIDIR) -I$(TPIDIR) -o $@ -c $<

$(ODIR)/%.o : $(SDIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(CXXDEPFLAGS) -I$(RSDIR) -I$(TTIDIR) -I$(TPIDIR) -o $@ -c $<

$(ODIR)/%.o : $(RSDIR)/%.C
	$(CXX) $(CXXFLAGS) $(CXXDEPFLAGS) -I$(RSDIR) -I$(TTIDIR) -I$(TPIDIR) -o $@ -c $<

$(ODIR)/%.o : $(RSDIR)/%.cc
	$(CXX) $(CXXFLAGS) $(CXXDEPFLAGS) -I$(RSDIR) -I$(TTIDIR) -I$(TPIDIR) -o $@ -c $<

$(ODIR)/%.o : $(RSDIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(CXXDEPFLAGS) -I$(RSDIR) -I$(TTIDIR) -I$(TPIDIR) -o $@ -c $<

makeTrainingTuples: $(ODIR)/NTupleReader.o $(ODIR)/SATException.o $(ODIR)/TaggerUtility.o $(ODIR)/PlotUtility.o $(ODIR)/samples.o $(ODIR)/makeTrainingTuples.o $(ODIR)/MiniTupleMaker.o 
	$(LD) $^ $(LIBS) $(MT2LIB) -o $@

clean:
	rm -f $(ODIR)/*.o $(ODIR)/*.so $(ODIR)/*.d $(PROGRAMS) core 

-include $(ODIR)/*.d
