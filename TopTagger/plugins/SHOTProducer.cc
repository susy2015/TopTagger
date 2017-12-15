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

#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
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

    // helper function to calculate subjet qg input vars 
    void compute(const reco::Jet * jet, bool isReco, double& totalMult_, double& ptD_, double& axis1_, double& axis2_);

    // ----------member data ---------------------------
    edm::InputTag jetSrc_, AK8JetSrc_;
    edm::EDGetTokenT<std::vector<pat::Jet> > JetTok_, AK8JetTok_;

    std::string qgTaggerKey_, deepFlavorBJetTags_, deepCSVBJetTags_, bTagKeyString_, taggerCfgFile_, NjettinessAK8Puppi_label_, ak8PFJetsPuppi_label_;
    double ak4ptCut_, ak8ptCut_;

    TopTagger tt;
};

void SHOTProducer::compute(const reco::Jet * jet, bool isReco, double& totalMult_, double& ptD_, double& axis1_, double& axis2_)
{

    totalMult_ = 0;
    ptD_       = 0;
    axis1_     = 0;
    axis2_     = 0;
    
    if(jet->numberOfDaughters() == 0) return;

    float sum_weight    = 0.0;
    float sum_dEta      = 0.0;
    float sum_dPhi      = 0.0;
    float sum_dEta2     = 0.0;
    float sum_dPhi2     = 0.0;
    float sum_dEta_dPhi = 0.0;
    float sum_pt        = 0.0;
    bool useQC          = false; // useQualityCuts; hard-coded for now to mimic what jetMet does in 731

    // loop over the jet constituents
    // (packed candidate situation)
    for(auto part : jet->getJetConstituentsQuick()) {
        if(part->charge()){ // charged particles
            if(isReco) {
                auto p = dynamic_cast<const pat::PackedCandidate*>(part);
                if(!p){
                    try { throw; }
                    catch(...) {
                        std::cout << "ERROR: QGTagging variables cannot be computed for these jets!" << std::endl
                                  << "       See QuauarGluonTaggingVaiables::compute()"              << std::endl;
                    } // catch(...)
                } // !p
                if(!( p->fromPV() > 1 && p->trackHighPurity() )) continue;
                if(useQC) {
                    // currently hard-coded to false above
                    // this isn't stored for packedCandidates, so will need fix if useQC is changed to true
                    if( p->dzError()==0 || p->dxyError()==0 ) continue;
                    if( (p->dz()*p->dz() )  / (p->dzError()*p->dzError() ) > 25. ) continue;
                    if( (p->dxy()*p->dxy()) / (p->dxyError()*p->dxyError()) < 25. ) ++totalMult_; // this cut only applies to multiplicity
                } else ++totalMult_;
            } else ++totalMult_;
        } else { // neutral particles
            if(part->pt() < 1.0) continue;
            ++totalMult_;
        } // charged, neutral particles

        float dEta   = part->eta() - jet->eta();
        float dPhi   = reco::deltaPhi(part->phi(), jet->phi());
        float partPt = part->pt();
        float weight = partPt*partPt;

        sum_weight    += weight;
        sum_pt        += partPt;
        sum_dEta      += dEta      * weight;
        sum_dPhi      += dPhi      * weight;
        sum_dEta2     += dEta*dEta * weight;
        sum_dEta_dPhi += dEta*dPhi * weight;
        sum_dPhi2     += dPhi*dPhi * weight;
    } // jet->getJetConstituentsQuick()

    // calculate axis2 and ptD
    float a = 0.0;
    float b = 0.0;
    float c = 0.0;
    float ave_dEta  = 0.0;
    float ave_dPhi  = 0.0;
    float ave_dEta2 = 0.0;
    float ave_dPhi2 = 0.0;

    if(sum_weight > 0){
        ptD_ = sqrt(sum_weight)/sum_pt;
        ave_dEta  = sum_dEta  / sum_weight;
        ave_dPhi  = sum_dPhi  / sum_weight;
        ave_dEta2 = sum_dEta2 / sum_weight;
        ave_dPhi2 = sum_dPhi2 / sum_weight;
        a = ave_dEta2 - ave_dEta*ave_dEta;
        b = ave_dPhi2 - ave_dPhi*ave_dPhi;
        c = -(sum_dEta_dPhi/sum_weight - ave_dEta*ave_dPhi);
    } else ptD_ = 0;

    float delta = sqrt(fabs( (a-b)*(a-b) + 4*c*c ));
    if(a+b-delta > 0) axis2_ = sqrt(0.5*(a+b-delta));
    else              axis2_ = 0.0;
    if(a+b+delta > 0) axis1_ = sqrt(0.5*(a+b+delta));
    else              axis1_ = 0.0;
}



SHOTProducer::SHOTProducer(const edm::ParameterSet& iConfig)
{
    //register vector of top objects 
    produces<std::vector<TLorentzVector>>("shotTopsP4");
    produces<std::vector<int>>("shotTopsType");
 
    //now do what ever other initialization is needed
    jetSrc_ = iConfig.getParameter<edm::InputTag>("ak4JetSrc");
    ak4ptCut_ = iConfig.getParameter<double>("ak4ptCut");

    AK8JetSrc_ = iConfig.getParameter<edm::InputTag>("ak8JetSrc");
    ak8ptCut_ = iConfig.getParameter<double>("ak8ptCut");

    qgTaggerKey_ = iConfig.getParameter<std::string>("qgTaggerKey");
    deepFlavorBJetTags_ = iConfig.getParameter<std::string>("deepFlavorBJetTags");
    deepCSVBJetTags_ = iConfig.getParameter<std::string>("deepCSVBJetTags");
    bTagKeyString_ = iConfig.getParameter<std::string>("bTagKeyString");

    NjettinessAK8Puppi_label_ = iConfig.getParameter<std::string>("NjettinessAK8Puppi_label");
    ak8PFJetsPuppi_label_ = iConfig.getParameter<std::string>("ak8PFJetsPuppi_label");

    taggerCfgFile_ = iConfig.getParameter<std::string>("taggerCfgFile");

    JetTok_ = consumes<std::vector<pat::Jet> >(jetSrc_);
    AK8JetTok_ = consumes<std::vector<pat::Jet> >(AK8JetSrc_);

    //configure the top tagger 
    tt.setCfgFile(taggerCfgFile_);
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

    edm::Handle<std::vector<pat::Jet> > ak8Jets;
    iEvent.getByToken(AK8JetTok_, ak8Jets);

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

    //Add ak8 jets to the constituent vector
    for(const pat::Jet& jet : *ak8Jets)
    {
        if(jet.pt() < ak8ptCut_) continue;

        TLorentzVector perJetLVec;
        perJetLVec.SetPtEtaPhiE( jet.pt(), jet.eta(), jet.phi(), jet.energy() );

        double puppi_tau1_uf         = jet.userFloat(NjettinessAK8Puppi_label_+":tau1");
        double puppi_tau2_uf         = jet.userFloat(NjettinessAK8Puppi_label_+":tau2");
        double puppi_tau3_uf         = jet.userFloat(NjettinessAK8Puppi_label_+":tau3");
        double puppisoftDropMass_uf  = jet.userFloat(ak8PFJetsPuppi_label_+"SoftDropMass");

        std::vector<Constituent> subjetVec;
        auto const & subjets = jet.subjets("SoftDrop");
        for( auto const & it : subjets)
        {
            TLorentzVector perSubJetLVec;
            perSubJetLVec.SetPtEtaPhiE( jet.pt(), jet.eta(), jet.phi(), jet.energy() );

            // btag info
            double subjetBDiscriminator = it->bDiscriminator(bTagKeyString_.c_str());

            //compute the qg input variables for the subjet
            double totalMult = 0;
            double ptD       = 0;
            double axis1     = 0;
            double axis2     = 0;
            compute(dynamic_cast<const pat::Jet *>(&(*it)), true, totalMult, ptD, axis1, axis2);

            subjetVec.emplace_back(perSubJetLVec, AK8SUBJET);
            subjetVec.back().setBTag(subjetBDiscriminator);
            subjetVec.back().setExtraVar("qgMult"      , totalMult);
            subjetVec.back().setExtraVar("qgPtD"       , ptD);
            subjetVec.back().setExtraVar("qgAxis1"     , axis1);
            subjetVec.back().setExtraVar("qgAxis2"     , axis2);
        }

        constituents.emplace_back(perJetLVec, puppi_tau1_uf, puppi_tau2_uf, puppi_tau3_uf, puppisoftDropMass_uf, subjetVec, 1.0);
    }

    //run top tagger
    tt.runTagger(constituents);

    //retrieve the top tagger results object
    const TopTaggerResults& ttr = tt.getResults();
    
    //get reconstructed top
    const std::vector<TopObject*>& tops = ttr.getTops();

    std::unique_ptr<std::vector<TLorentzVector>> top4vecs(new std::vector<TLorentzVector>());
    std::unique_ptr<std::vector<int>> toptype(new std::vector<int>());
    
    for(auto* top : tops)
    {
        top4vecs->emplace_back(top->p());
        toptype->emplace_back(top->getNConstituents());
    }

    iEvent.put(std::move(top4vecs), "shotTopsP4");
    iEvent.put(std::move(toptype), "shotTopsType");
 
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
