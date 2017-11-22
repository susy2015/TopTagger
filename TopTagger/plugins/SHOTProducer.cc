// -*- C++ -*-
//
// Package:    TopTagger/TopTagger
// Class:      SHOTProducer
// 
/**\class SHOTProducer SHOTProducer.cc TopTagger/TopTagger/plugins/SHOTProducer.cc

   Description: [one line class summary]

   Implementation:
   [Notes on implementation]
*/
//
// Original Author:  Nathaniel Pastika
//         Created:  Thu, 09 Nov 2017 21:29:56 GMT
//
//


// system include files
#include <memory>
#include <vector>
#include <string>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/StreamID.h"

#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/Common/interface/Handle.h"

#include "DataFormats/PatCandidates/interface/Jet.h"

//#include "FWCore/Framework/interface/EventSetup.h"
//#include "FWCore/Framework/interface/ESHandle.h"

#include "TLorentzVector.h"

#include "TopTagger/TopTagger/include/TopTagger.h"
#include "TopTagger/TopTagger/include/TopTaggerResults.h"
#include "TopTagger/TopTagger/include/TopObject.h"
#include "TopTagger/TopTagger/include/Constituent.h"

class SHOTProducer : public edm::stream::EDProducer<> 
{
public:
    explicit SHOTProducer(const edm::ParameterSet&);
    ~SHOTProducer();

    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
    virtual void beginStream(edm::StreamID) override;
    virtual void produce(edm::Event&, const edm::EventSetup&) override;
    virtual void endStream() override;

    // ----------member data ---------------------------
    edm::InputTag jetSrc_;
    edm::EDGetTokenT<std::vector<pat::Jet> > JetTok_;

    std::string qgTaggerKey_, deepFlavorBJetTags_, deepCSVBJetTags_, bTagKeyString_, taggerCfg_;
    double ak4ptCut_;

    TopTagger tt;
};

SHOTProducer::SHOTProducer(const edm::ParameterSet& iConfig)
{
    //register vector of top objects 
    produces<std::vector<TopObject>>("shotTops");
 
    //now do what ever other initialization is needed
    jetSrc_ = iConfig.getParameter<edm::InputTag>("ak4JetSrc");
    ak4ptCut_ = iConfig.getParameter<double>("ak4ptCut");

    qgTaggerKey_ = iConfig.getParameter<std::string>("qgTaggerKey");
    deepFlavorBJetTags_ = iConfig.getParameter<std::string>("deepFlavorBJetTags");
    deepCSVBJetTags_ = iConfig.getParameter<std::string>("deepCSVBJetTags");
    bTagKeyString_ = iConfig.getParameter<std::string>("bTagKeyString");

    taggerCfg_ = iConfig.getParameter<std::string>("taggerCfg");

    JetTok_ = consumes<std::vector<pat::Jet> >(jetSrc_);

    //configure the top tagger 
    tt.setCfgFileDirect(taggerCfg_);
}


SHOTProducer::~SHOTProducer()
{
 
}


//
// member functions
//

// ------------ method called to produce the data  ------------
void SHOTProducer::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
    using namespace edm;

    edm::Handle<std::vector<pat::Jet> > jets;
    iEvent.getByToken(JetTok_, jets);

    std::vector<Constituent> constituents;

    for(const pat::Jet& jet : *jets)
    {
        if(jet.pt() < ak4ptCut_) continue;

        TLorentzVector perJetLVec;
        perJetLVec.SetPtEtaPhiE( jet.pt(), jet.eta(), jet.phi(), jet.energy() );

        double qgLikelihood = jet.userFloat(qgTaggerKey_+":qgLikelihood");
        double qgPtD = jet.userFloat(qgTaggerKey_+":ptD");
        double qgAxis1 = jet.userFloat(qgTaggerKey_+":axis1");
        double qgAxis2 = jet.userFloat(qgTaggerKey_+":axis2");
        double qgMult = static_cast<double>(jet.userInt(qgTaggerKey_+":mult"));
        double deepFlavorb = jet.bDiscriminator((deepFlavorBJetTags_+":probb").c_str());
        double deepFlavorbb = jet.bDiscriminator((deepFlavorBJetTags_+":probbb").c_str());
        double deepFlavorlepb = jet.bDiscriminator((deepFlavorBJetTags_+":problepb").c_str());
        double deepFlavorc = jet.bDiscriminator((deepFlavorBJetTags_+":probc").c_str());
        double deepFlavoruds = jet.bDiscriminator((deepFlavorBJetTags_+":probuds").c_str());
        double deepFlavorg = jet.bDiscriminator((deepFlavorBJetTags_+":probg").c_str());
        double deepCSVb = jet.bDiscriminator((deepCSVBJetTags_+":probb").c_str());
        double deepCSVc = jet.bDiscriminator((deepCSVBJetTags_+":probc").c_str());
        double deepCSVl = jet.bDiscriminator((deepCSVBJetTags_+":probudsg").c_str());
        double deepCSVbb = jet.bDiscriminator((deepCSVBJetTags_+":probbb").c_str());
        double deepCSVcc = jet.bDiscriminator((deepCSVBJetTags_+":probcc").c_str());
        double btag = jet.bDiscriminator(bTagKeyString_.c_str());
        double charge = jet.jetCharge();
        double chargedHadronEnergyFraction = jet.chargedHadronEnergyFraction();
        double neutralHadronEnergyFraction = jet.neutralHadronEnergyFraction();
        double chargedEmEnergyFraction = jet.chargedEmEnergyFraction();
        double neutralEmEnergyFraction = jet.neutralEmEnergyFraction();
        double muonEnergyFraction = jet.muonEnergyFraction();
        double photonEnergyFraction = jet.photonEnergyFraction();
        double electronEnergyFraction = jet.electronEnergyFraction();
        double recoJetsHFHadronEnergyFraction = jet.HFHadronEnergyFraction();
        double recoJetsHFEMEnergyFraction = jet.HFEMEnergyFraction();
        double chargedHadronMultiplicity = jet.chargedHadronMultiplicity();
        double neutralHadronMultiplicity = jet.neutralHadronMultiplicity();
        double photonMultiplicity = jet.photonMultiplicity();
        double electronMultiplicity = jet.electronMultiplicity();
        double muonMultiplicity = jet.muonMultiplicity();

        constituents.emplace_back(perJetLVec, btag, qgLikelihood);
        constituents.back().setExtraVar("qgMult"                              , qgMult);
        constituents.back().setExtraVar("qgPtD"                               , qgPtD);
        constituents.back().setExtraVar("qgAxis1"                             , qgAxis1);
        constituents.back().setExtraVar("qgAxis2"                             , qgAxis2);
        constituents.back().setExtraVar("recoJetschargedHadronEnergyFraction" , chargedHadronEnergyFraction);
        constituents.back().setExtraVar("recoJetschargedEmEnergyFraction"     , chargedEmEnergyFraction);
        constituents.back().setExtraVar("recoJetsneutralEmEnergyFraction"     , neutralEmEnergyFraction);
        constituents.back().setExtraVar("recoJetsmuonEnergyFraction"          , muonEnergyFraction);
        constituents.back().setExtraVar("recoJetsHFHadronEnergyFraction"      , recoJetsHFHadronEnergyFraction);
        constituents.back().setExtraVar("recoJetsHFEMEnergyFraction"          , recoJetsHFEMEnergyFraction);
        constituents.back().setExtraVar("recoJetsneutralEnergyFraction"       , neutralHadronEnergyFraction);
        constituents.back().setExtraVar("PhotonEnergyFraction"                , photonEnergyFraction);
        constituents.back().setExtraVar("ElectronEnergyFraction"              , electronEnergyFraction);
        constituents.back().setExtraVar("ChargedHadronMultiplicity"           , chargedHadronMultiplicity);
        constituents.back().setExtraVar("NeutralHadronMultiplicity"           , neutralHadronMultiplicity);
        constituents.back().setExtraVar("PhotonMultiplicity"                  , photonMultiplicity);
        constituents.back().setExtraVar("ElectronMultiplicity"                , electronMultiplicity);
        constituents.back().setExtraVar("MuonMultiplicity"                    , muonMultiplicity);
        constituents.back().setExtraVar("DeepFlavorb"                         , deepFlavorb);
        constituents.back().setExtraVar("DeepFlavorbb"                        , deepFlavorbb);
        constituents.back().setExtraVar("DeepFlavorlepb"                      , deepFlavorlepb);
        constituents.back().setExtraVar("DeepFlavorc"                         , deepFlavorc);
        constituents.back().setExtraVar("DeepFlavoruds"                       , deepFlavoruds);
        constituents.back().setExtraVar("DeepFlavorg"                         , deepFlavorg);
        constituents.back().setExtraVar("DeepCSVb"                            , deepCSVb);
        constituents.back().setExtraVar("DeepCSVc"                            , deepCSVc);
        constituents.back().setExtraVar("DeepCSVl"                            , deepCSVl);
        constituents.back().setExtraVar("DeepCSVbb"                           , deepCSVbb);
        constituents.back().setExtraVar("DeepCSVcc"                           , deepCSVcc);
        constituents.back().setExtraVar("recoJetsCharge"                      , charge);
    }

    //run top tagger
    tt.runTagger(constituents);

    //retrieve the top tagger results object
    const TopTaggerResults& ttr = tt.getResults();
    
    //get reconstructed top
    const std::vector<TopObject*>& tops = ttr.getTops();

    std::unique_ptr<std::vector<TLorentzVector>> top4vecs(new std::vector<TLorentzVector>());
    
    for(auto* top : tops)
    {
        top4vecs->emplace_back(top->p());
    }

    iEvent.put(std::move(top4vecs), "shotTops");
 
}

// ------------ method called once each stream before processing any runs, lumis or events  ------------
void SHOTProducer::beginStream(edm::StreamID)
{
}

// ------------ method called once each stream after processing all runs, lumis and events  ------------
void SHOTProducer::endStream() 
{
}
 
// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void SHOTProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    //The following says we do not know what parameters are allowed so do no validation
    // Please change this to state exactly what you do use, even if it is no parameters
    edm::ParameterSetDescription desc;
    desc.setUnknown();
    descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(SHOTProducer);
