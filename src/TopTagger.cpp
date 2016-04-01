#include "TopTagger.h"

#include "TopObject.h"
#include "TTModule.h"
#include "TopTaggerResults.h"

TopTagger::TopTagger()
{
    topTaggerResults_ = nullptr;
}

void TopTagger::registerModule(TTModule * module)
{
    topTaggerModules_.emplace_back(module);
}

void TopTagger::runTagger(const std::vector<const Constituent> * constituents)
{
    topTaggerResults_ = new TopTaggerResults();

    for(TTModule * const module : topTaggerModules_)
    {
        module->run(*topTaggerResults_);
    }
}

const TopTaggerResults& TopTagger::getResults()
{
    if(topTaggerResults_) return *topTaggerResults_;
    else
    {
        throw "const TopTaggerResults& TopTagger::getResults() : Invalid TopTaggerResults ptr";
    }
}
