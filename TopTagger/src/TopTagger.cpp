#include "TopTagger/TopTagger/include/TopTagger.h"

#include "TopTagger/TopTagger/include/TopObject.h"
#include "TopTagger/TopTagger/include/TTModule.h"
#include "TopTagger/TopTagger/include/TopTaggerResults.h"

#include "TopTagger/TopTagger/include/hcal/cfg/CfgDocument.hh"
#include "TopTagger/TopTagger/include/hcal/cfg/Record.hh"
#include "TopTagger/TopTagger/include/hcal/cfg/Context.hh"

#include <iostream>

TopTagger::TopTagger()
{
    topTaggerResults_ = nullptr;

    //dummy configuration document
    std::string cfgDocText = 
        "scope\n"
        "{\n"
        "    var1 = 45\n"
        "}\n";

    cfgDoc_ = (hcal::cfg::CfgDocument::parseDocument(cfgDocText)).release();
    cfgRecord_ = new hcal::cfg::Record();
    cfgDoc_->useRecord(cfgRecord_);

    hcal::cfg::Context cxt("scope");

    int var1 = cfgDoc_->get("var1", cxt, 123);

    std::cout << "var1: " << var1 << std::endl;
}

TopTagger::~TopTagger()
{
    if(topTaggerResults_) delete topTaggerResults_;
}

void TopTagger::registerModule(std::unique_ptr<TTModule>& module)
{
    topTaggerModules_.push_back(std::move(module));
}

void TopTagger::runTagger(const std::vector<Constituent>& constituents)
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
