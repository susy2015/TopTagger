#include "TopTagger/TopTagger/interface/TTMOverlapResolution.h"

#include "TopTagger/TopTagger/interface/TopTaggerResults.h"
#include "TopTagger/CfgParser/interface/Context.hh"
#include "TopTagger/CfgParser/interface/TTException.h"
#include "TopTagger/CfgParser/interface/CfgDocument.hh"

#include <set>
#include <algorithm>
#include <vector>
#include <cmath>

void TTMOverlapResolution::getParameters(const cfg::CfgDocument* cfgDoc, const std::string& localContextName)
{
    //Construct contexts
    cfg::Context commonCxt("Common");
    cfg::Context localCxt(localContextName);

    mt_           = cfgDoc->get("mt",           commonCxt, -999.9);
    maxTopEta_    = cfgDoc->get("maxTopEta",    commonCxt, -999.9);
    dRMatch_      = cfgDoc->get("dRMatch",      commonCxt, -999.9);
    dRMatchAK8_   = cfgDoc->get("dRMatchAK8",   commonCxt, 0.8);

    cvsThreshold_  = cfgDoc->get("cvsThreshold",  localCxt,  -999.9);
    type_          = static_cast<TopObject::Type>(cfgDoc->get("NConstituents", localCxt,  TopObject::ANY));
    sortMethod_    = cfgDoc->get("sortMethod",    localCxt,  "EMPTY");
    markUsed_      = cfgDoc->get("markUsed",      localCxt,  true);

    //select the approperiate sorting function 
    doSort_ = true;  //sort true unless option "none" is selected
    if(sortMethod_.compare("topMass") == 0)
    {
        sortFunc_ = [this](const TopObject* t1, const TopObject* t2)
        {
            double m1 = -999.9, m2 = -999.9;
            const auto& constVec1 = t1->getConstituents();
            switch(t1->getNConstituents())
            {
            case 3:
                m1 = t1->p().M();
                break;
            case 2:
                if(constVec1[0]->getType() == Constituent::AK8JET)
                {
                    TLorentzVector psudoVec;
                    psudoVec.SetPtEtaPhiM(constVec1[0]->p().Pt(), constVec1[0]->p().Eta(), constVec1[0]->p().Phi(), constVec1[0]->getSoftDropMass() * constVec1[0]->getWMassCorr());
                    m1 = (psudoVec + constVec1[1]->p()).M();
                }
                else
                {
                    TLorentzVector psudoVec;
                    psudoVec.SetPtEtaPhiM(constVec1[1]->p().Pt(), constVec1[1]->p().Eta(), constVec1[1]->p().Phi(), constVec1[1]->getSoftDropMass() * constVec1[1]->getWMassCorr());
                    m1 = (psudoVec + constVec1[0]->p()).M();
                }
                break;
            case 1:
                m1 = constVec1[0]->getSoftDropMass();
                break;
            }

            const auto& constVec2 = t2->getConstituents();
            switch(t2->getNConstituents())
            {
            case 3:
                m2 = t2->p().M();
                break;
            case 2:
                if(constVec2[0]->getType() == Constituent::AK8JET)
                {
                    TLorentzVector psudoVec;
                    psudoVec.SetPtEtaPhiM(constVec2[0]->p().Pt(), constVec2[0]->p().Eta(), constVec2[0]->p().Phi(), constVec2[0]->getSoftDropMass() * constVec2[0]->getWMassCorr());
                    m2 = (psudoVec + constVec2[1]->p()).M();
                }
                else
                {
                    TLorentzVector psudoVec;
                    psudoVec.SetPtEtaPhiM(constVec2[1]->p().Pt(), constVec2[1]->p().Eta(), constVec2[1]->p().Phi(), constVec2[1]->getSoftDropMass() * constVec2[1]->getWMassCorr());
                    m2 = (psudoVec + constVec2[0]->p()).M();
                }
                break;
            case 1:
                m2 = constVec2[0]->getSoftDropMass();
                break;
            }
            return fabs(m1 - this->mt_) < fabs(m2 - this->mt_); 
        };
    }
    else if(sortMethod_.compare("topPt") == 0)
    {
        sortFunc_ = [this](const TopObject* t1, const TopObject* t2){ return t1->p().Pt() > t2->p().Pt(); };
    }
    else if(sortMethod_.compare("mvaDisc") == 0)
    {
        sortFunc_ = [this](const TopObject* t1, const TopObject* t2){ return t1->getDiscriminator() > t2->getDiscriminator(); };
    }
    else if(sortMethod_.compare("mvaDiscWithb") == 0)
    {
        sortFunc_ = [this](const TopObject* t1, const TopObject* t2)
        {
            int nb1 = t1->getNBConstituents(cvsThreshold_);
            int nb2 = t2->getNBConstituents(cvsThreshold_);
            if     (nb1 <= 1 && nb2 > 1)  return true;
            else if(nb1 > 1  && nb2 <= 1) return false;
            else if(nb1 <= 1 && nb2 <= 1) ; //sort by discriminator
            else if(nb1 < nb2)            return true;
            else if(nb1 > nb2)            return false;

            if(t1->getDiscriminator() > t2->getDiscriminator()) return true; 

            return false;
        };
    }
    else if(sortMethod_.compare("none") == 0)
    {
        //do nothing 
        doSort_ = false;
    }
    else
    {
        THROW_TTEXCEPTION("Invalid sorting option");
    }

}

void TTMOverlapResolution::run(TopTaggerResults& ttResults)
{
    //Get list of constituents used to construct tops
    const std::vector< Constituent>& constituents = ttResults.getConstituents();

    //Get vector of final tops to prune
    std::vector<TopObject*>& tops = ttResults.getTops();

    //This container will keep track of which jets have been included in final tops
    std::set<Constituent const *>& usedJets = ttResults.getUsedConstituents();

    //Sort the top vector for overlap resolution
    if(doSort_) std::sort(tops.begin(), tops.end(), sortFunc_);

    for(auto iTop = tops.begin(); iTop != tops.end();)
    {
        //Check that this top had the expected type
        if((type_ == TopObject::ANY) || ((*iTop)->getType() == type_))
        {
            //Get constituent jets for this top
            const std::vector<Constituent const *>& jets = (*iTop)->getConstituents();

            //Requirement on top eta here
            bool passTopEta = (fabs((*iTop)->p().Eta()) < maxTopEta_);

            //Check if the candidates have been used in another top
            bool overlaps = constituentsAreUsed(jets, usedJets, dRMatch_, dRMatchAK8_);

            //Prune top from final top collection if it fails the following requirements
            if(overlaps || !passTopEta)
            {
                //This is inefficient and dumb
                iTop = tops.erase(iTop);
                continue;
            }

            //If the candidate survived, it must be a good top!!!

            //Add the good tops constituents to the set tracking which constituents have been used
            if(markUsed_) markConstituentsUsed(jets, constituents, usedJets, dRMatch_, dRMatchAK8_);
        }

        ++iTop;
    }
}
