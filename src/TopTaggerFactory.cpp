#include "TopTaggerFactory.h"

#include "TTModule.h"
#include "TTMLazyClusterAlgo.h"
#include "TTMHEPRequirements.h"
#include "TTMOverlapResolution.h"
#include "TopTagger.h"

TopTaggerFactory::~TopTaggerFactory()
{
}

std::unique_ptr<TopTagger> TopTaggerFactory::makeTopTagger(std::string tagger)
{
    //Eventually will have a if-else tree of doom to select tagger varients
    //For now we just have the one terrible top tagger 
    
    //Create empty top tagger
    std::unique_ptr<TopTagger> tt(new TopTagger());

    //Create necessary modules 
    std::unique_ptr<TTModule> ttmLCA(new TTMLazyClusterAlgo());
    std::unique_ptr<TTModule> ttmHEPReq(new TTMHEPRequirements());
    std::unique_ptr<TTModule> ttmOR(new TTMOverlapResolution());

    tt->registerModule(ttmLCA);
    tt->registerModule(ttmHEPReq);
    tt->registerModule(ttmOR);

    return tt;
}
