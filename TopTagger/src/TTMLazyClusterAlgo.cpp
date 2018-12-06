#include "TopTagger/TopTagger/interface/TTMLazyClusterAlgo.h"

#include "TopTagger/TopTagger/interface/TopTaggerResults.h"
#include "TopTagger/CfgParser/include/Context.hh"
#include "TopTagger/CfgParser/include/CfgDocument.hh"

void TTMLazyClusterAlgo::getParameters(const cfg::CfgDocument* cfgDoc, const std::string& localContextName)
{
    //Construct contexts
    cfg::Context commonCxt("Common");
    cfg::Context localCxt(localContextName);
    
    dRMax_          = cfgDoc->get("dRMax",           localCxt, -999.9);

    lowWMassCut_    = cfgDoc->get("lowWJetMassCut",  localCxt, -999.9);
    highWMassCut_   = cfgDoc->get("highWJetMassCut", localCxt, -999.9);
    lowtMassCut_    = cfgDoc->get("lowtJetMassCut",  localCxt, -999.9);
    hightMassCut_   = cfgDoc->get("hightJetMassCut", localCxt, -999.9);
    minTopCandMass_ = cfgDoc->get("minTopCandMass",  localCxt, -999.9);
    maxTopCandMass_ = cfgDoc->get("maxTopCandMass",  localCxt, -999.9);
    minJetPt_       = cfgDoc->get("minJetPt",        localCxt, -999.9);

    doMonojet_      = cfgDoc->get("doMonojet",      localCxt,  false);
    doDijet_        = cfgDoc->get("doDijet",        localCxt,  false);
    doTrijet_       = cfgDoc->get("doTrijet",       localCxt,  false);
}

void TTMLazyClusterAlgo::run(TopTaggerResults& ttResults)
{
    const std::vector< Constituent>& constituents = ttResults.getConstituents();
    std::vector<TopObject>& topCandidates = ttResults.getTopCandidates();

    for(unsigned int i = 0; i < constituents.size(); ++i)
    {
        if(constituents[i].p().Pt() < minJetPt_ || constituents[i].getType() != AK4JET) continue;

        //singlet tops
        if(doMonojet_)
        {
            if(constituents[i].p().M() >= lowtMassCut_ && constituents[i].p().M() <= hightMassCut_)
            {
                TopObject topCand({&constituents[i]});

                topCandidates.push_back(topCand);
            }
        }

        //singlet w-bosons
        if(doDijet_)
        {
            if(constituents[i].p().M() >= lowWMassCut_ && constituents[i].p().M() <= highWMassCut_)
            {
                //dijet combinations
                for(unsigned int j = 0; j < constituents.size(); ++j)
                {
                    if(i == j) continue;
                    if(constituents[j].p().Pt() < minJetPt_ || constituents[j].getType() != AK4JET) continue;

                    TopObject topCand({&constituents[i], &constituents[j]});
                
                    if(topCand.getDRmax() < dRMax_)
                    {
                        topCandidates.push_back(topCand);
                    }
                }
            }
        }

        //Trijet combinations 
        if(doTrijet_)
        {
            for(unsigned int j = 0; j < i; ++j)
            {
                if(constituents[j].p().Pt() < minJetPt_ || constituents[j].getType() != AK4JET) continue;

                for(unsigned int k = 0; k < j; ++k)
                {
                    if(constituents[k].p().Pt() < minJetPt_ || constituents[k].getType() != AK4JET) continue;

                    TopObject topCand({&constituents[k], &constituents[j], &constituents[i]});

                    //mass window on the top candidate mass
                    double m123 = topCand.p().M();
                    bool passMassWindow = (minTopCandMass_ < m123) && (m123 < maxTopCandMass_);

                    if(topCand.getDRmax() < dRMax_ && passMassWindow)
                    {
                        topCandidates.push_back(topCand);
                    }
                }
            }
        }
    }
}
