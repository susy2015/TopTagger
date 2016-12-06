#include "TopTagger/TopTagger/include/TTMBasicClusterAlgo.h"

#include "TopTagger/TopTagger/include/TopTaggerResults.h"
#include "TopTagger/CfgParser/include/Context.hh"
#include "TopTagger/CfgParser/include/CfgDocument.hh"

void TTMBasicClusterAlgo::getParameters(const cfg::CfgDocument* cfgDoc, const std::string& localContextName)
{
    //Construct contexts
    cfg::Context commonCxt("Common");
    cfg::Context localCxt(localContextName);
    
    //monojet parameters
    minAK8TopMass_    = cfgDoc->get("minAK8TopMass",    localCxt, -999.9);
    maxAK8TopMass_    = cfgDoc->get("maxAK8TopMass",    localCxt, -999.9);
    maxTopTau32_      = cfgDoc->get("maxTopTau32",      localCxt, -999.9);
    minAK8TopPt_      = cfgDoc->get("minAK8TopPt",      localCxt, -999.9);
    doMonojet_        = cfgDoc->get("doMonojet",        localCxt,  false);

    //dijet parameters
    minAK8WMass_      = cfgDoc->get("minAK8WMass",      localCxt, -999.9);
    maxAK8WMass_      = cfgDoc->get("maxAK8WMass",      localCxt, -999.9);
    maxWTau21_        = cfgDoc->get("maxWTau21",        localCxt, -999.9);
    minAK8WPt_        = cfgDoc->get("minAK8WPt",        localCxt, -999.9);
    minAK4WPt_        = cfgDoc->get("minAK4WPt",        localCxt, -999.9);
    doDijet_          = cfgDoc->get("doDijet",          localCxt,  false);
    dRMaxDiJet_       = cfgDoc->get("dRMaxDijet",       localCxt, -999.9);

    //trijet parameters
    minTopCandMass_   = cfgDoc->get("minTopCandMass",   localCxt, -999.9);
    maxTopCandMass_   = cfgDoc->get("maxTopCandMass",   localCxt, -999.9);
    doTrijet_         = cfgDoc->get("doTrijet",         localCxt,  false);
    dRMaxTrijet_      = cfgDoc->get("dRMaxTrijet",      localCxt, -999.9);
    minAK4ResolvedPt_ = cfgDoc->get("minAK4ResolvedPt", localCxt, -999.9);
}

void TTMBasicClusterAlgo::run(TopTaggerResults& ttResults)
{
    const std::vector< Constituent>& constituents = ttResults.getConstituents();
    std::vector<TopObject>& topCandidates = ttResults.getTopCandidates();

    for(unsigned int i = 0; i < constituents.size(); ++i)
    {
        //singlet tops
        if(doMonojet_)
        {
            //Only use AK8 tops here 
            if(passAK8TopReqs(constituents[i]))
            {
                TopObject topCand({&constituents[i]});

                topCandidates.push_back(topCand);
            }
        }

        //singlet w-bosons + jet
        if(doDijet_)
        {
            if(passAK8WReqs(constituents[i]))
            {
                //dijet combinations
                for(unsigned int j = 0; j < constituents.size(); ++j)
                {
                    //Ensure we never use the same jet twice
                    //Only pair the AK8 W with an AK4 jet
                    if(i == j || passAK4WReqs(constituents[j])) continue;

                    TopObject topCand({&constituents[i], &constituents[j]});

                    //mass window on the top candidate mass
                    double m123 = topCand.p().M();
                    bool passMassWindow = (minTopCandMass_ < m123) && (m123 < maxTopCandMass_);

                    if(topCand.getDRmax() < dRMaxDiJet_ && passMassWindow)
                    {
                        topCandidates.push_back(topCand);
                    }
                }
            }
        }

        //Trijet combinations 
        if(doTrijet_)
        {
            if(passAK4ResolvedReqs(constituents[i]))
            {
                for(unsigned int j = 0; j < i; ++j)
                {
                    if(passAK4ResolvedReqs(constituents[j]))
                    {                
                        for(unsigned int k = 0; k < j; ++k)
                        {
                            if(passAK4ResolvedReqs(constituents[k]))
                            {
                                TopObject topCand({&constituents[k], &constituents[j], &constituents[i]});

                                //mass window on the top candidate mass
                                double m123 = topCand.p().M();
                                bool passMassWindow = (minTopCandMass_ < m123) && (m123 < maxTopCandMass_);

                                if(topCand.getDRmax() < dRMaxTrijet_ && passMassWindow)
                                {
                                    topCandidates.push_back(topCand);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

bool TTMBasicClusterAlgo::passAK8WReqs(const Constituent& constituent) const
{
    //check that it is an AK8 jet
    if(constituent.getType() != AK8JET) return false;

    //check that tau1 and 2 are valid
    if(constituent.getTau1() <= 0 || constituent.getTau2() <= 0) return false;

    double tau21 = constituent.getTau2()/constituent.getTau1();

    return constituent.p().Pt() > minAK8WPt_ &&
           constituent.getSoftDropMass() > minAK8WMass_  && 
           constituent.getSoftDropMass() < maxAK8WMass_ &&
           tau21 < maxWTau21_;
}

bool TTMBasicClusterAlgo::passAK4WReqs(const Constituent& constituent) const
{
    return constituent.getType() == AK4JET && constituent.p().Pt() > minAK4WPt_;
}

bool TTMBasicClusterAlgo::passAK8TopReqs(const Constituent& constituent) const
{
    //check that it is an AK8 jet
    if(constituent.getType() != AK8JET) return false;

    //check that tau2 and 3 are valid
    if(constituent.getTau2() <= 0 || constituent.getTau3() <= 0) return false;

    double tau32 = constituent.getTau3()/constituent.getTau2();

    return constituent.p().Pt() > minAK8TopPt_ &&
           constituent.getSoftDropMass() > minAK8TopMass_  && 
           constituent.getSoftDropMass() < maxAK8TopMass_ &&
           tau32 < maxTopTau32_;
}

bool TTMBasicClusterAlgo::passAK4ResolvedReqs(const Constituent& constituent) const
{
    return constituent.getType() == AK4JET && constituent.p().Pt() > minAK4ResolvedPt_;
}
