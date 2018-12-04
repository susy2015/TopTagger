#include "DataFormats/Common/interface/Wrapper.h"
#include "DataFormats/Common/interface/Association.h"

#include "DataFormats/Provenance/interface/ProductID.h"
#include "DataFormats/Common/interface/ValueMap.h"

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/PatCandidates/interface/UserData.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"

#include "DataFormats/Common/interface/RefToBaseVector.h"

#include "TLorentzVector.h"

#include "TopTagger/TopTagger/include/TopObjLite.h"

namespace { 
  struct dictionary {

     typedef unsigned long key_type;
  
     std::vector<TLorentzVector> vt;
     edm::Wrapper<std::vector<TLorentzVector> > wvt;

     std::vector<std::vector<TLorentzVector> > vtt;
     edm::Wrapper<std::vector<std::vector<TLorentzVector> > > wvtt;

     TopObjLite tol;
     edm::Wrapper<TopObjLite> wtol;

     std::vector<TopObjLite> vtol;
     edm::Wrapper<std::vector<TopObjLite>> wvtol;
  };
}


