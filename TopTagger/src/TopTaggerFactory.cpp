#include "TopTagger/TopTagger/include/TopTaggerFactory.h"

#include "TopTagger/TopTagger/include/TTModule.h"
#include "TopTagger/TopTagger/include/TTMLazyClusterAlgo.h"
#include "TopTagger/TopTagger/include/TTMHEPRequirements.h"
#include "TopTagger/TopTagger/include/TTMOverlapResolution.h"
#include "TopTagger/TopTagger/include/TopTagger.h"

TopTaggerFactory::~TopTaggerFactory()
{
}

TopTagger* TopTaggerFactory::makeTopTagger(std::string tagger)
{
    //Eventually will have a if-else tree of doom to select tagger varients
    //For now we just have the one terrible top tagger 
    
    //Create empty top tagger

    TopTagger *tt = new TopTagger();

    //Create necessary modules 
    std::unique_ptr<TTModule> ttmLCA(new TTMLazyClusterAlgo());
    std::unique_ptr<TTModule> ttmHEPReq(new TTMHEPRequirements());
    std::unique_ptr<TTModule> ttmOR(new TTMOverlapResolution());

    tt->registerModule(ttmLCA);
    tt->registerModule(ttmHEPReq);
    tt->registerModule(ttmOR);

    return tt;
}
