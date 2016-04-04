#include "TopTagger.h"

#include "TopObject.h"
#include "TTModule.h"
#include "TopTaggerResults.h"

TopTagger::TopTagger()
{
    topTaggerResults_ = nullptr;
}

void TopTagger::registerModule(std::unique_ptr<TTModule>& module)
{
    topTaggerModules_.push_back(std::move(module));
}

void TopTagger::runTagger(const std::vector<const Constituent> * constituents)
{
    if(topTaggerResults_) delete topTaggerResults_;
    topTaggerResults_ = new TopTaggerResults();

    for(std::unique_ptr<TTModule>& module : topTaggerModules_)
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
