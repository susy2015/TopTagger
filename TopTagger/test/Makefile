ODIR       = obj

TTIDIR     = $(CMSSW_BASE)/src/TopTagger/TopTagger/include
TPIDIR     = $(CMSSW_BASE)/src/TopTagger/CfgParser/include

TTSDIR     = $(CMSSW_BASE)/src/TopTagger/TopTagger/src
TPSDIR     = $(CMSSW_BASE)/src/TopTagger/CfgParser/src

CXX        = g++

CXXFLAGS  += -I$(CMSSW_BASE)/src -std=c++0x
## Optimization/debug flag
CXXFLAGS += -g #-O3
## Enable for maximum warning
#CXXFLAGS += -Wall -Wextra -Weffc++

## Include ROOT
CXXFLAGS  += $(shell root-config --cflags)

CXXDEPFLAGS = -MMD -MP

LD         = g++
LDFLAGS    =

LIBS       = $(shell root-config --glibs)

PROGRAMS = 

LIBRARIES = TopTagger

all: mkobj $(LIBRARIES) $(PROGRAMS)


mkobj:
	@mkdir -p obj


$(ODIR)/%.o : $(TTSDIR)/%.cc
	$(CXX) $(CXXFLAGS) $(CXXDEPFLAGS) -fPIC -o $@ -c $<

$(ODIR)/%.o : $(TTSDIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(CXXDEPFLAGS) -fPIC -o $@ -c $<

$(ODIR)/%.o : $(TPSDIR)/%.cc
	$(CXX) $(CXXFLAGS) $(CXXDEPFLAGS) -fPIC -o $@ -c $<

$(ODIR)/%.o : $(TPSDIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(CXXDEPFLAGS) -fPIC -o $@ -c $<

#harvestHists: $(ODIR)/harvestHists.o
#	$(LD) $^ $(LIBS) -o $@

TopTagger : $(addprefix $(ODIR)/, $(notdir $(patsubst %.cc, %.o, $(patsubst %.cpp, %.o, $(wildcard $(TTSDIR)/*.cpp $(TTSDIR)/*.cc $(TPSDIR)/*.cpp $(TPSDIR)/*.cc)))))
	ar rcs lib$@.a $^
	gcc -shared -o lib$@.so $^

clean:
	rm -f $(ODIR)/*.o $(ODIR)/*.so $(ODIR)/*.d $(PROGRAMS) $(LIBRARIES) core 

-include $(ODIR)/*.d
