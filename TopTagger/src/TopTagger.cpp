#include "TopTagger/TopTagger/include/TopTagger.h"

#include "TopTagger/TopTagger/include/TopObject.h"
#include "TopTagger/TopTagger/include/TTModule.h"
#include "TopTagger/TopTagger/include/TopTaggerResults.h"

TopTagger::TopTagger()
{
    topTaggerResults_ = nullptr;
}

TopTagger::~TopTagger()
{
    if(topTaggerResults_) delete topTaggerResults_;
}

void TopTagger::registerModule(std::unique_ptr<TTModule>& module)
{
    topTaggerModules_.push_back(std::move(module));
}

void TopTagger::runTagger(const std::vector<Constituent> * constituents)
{
    if(topTaggerResults_) delete topTaggerResults_;
    topTaggerResults_ = new TopTaggerResults(constituents);

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
