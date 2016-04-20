#include "TopTagger/TopTagger/include/TopTagger.h"

#include "TopTagger/TopTagger/include/TopObject.h"
#include "TopTagger/TopTagger/include/TTModule.h"
#include "TopTagger/TopTagger/include/TopTaggerResults.h"

#include "TopTagger/CfgParser/include/CfgDocument.hh"
#include "TopTagger/CfgParser/include/Record.hh"
#include "TopTagger/CfgParser/include/Context.hh"

#include <iostream>
#include <cstdio>

TopTagger::TopTagger()
{
    topTaggerResults_ = nullptr;
}

TopTagger::TopTagger(const std::string& cfgFileName)
{
    topTaggerResults_ = nullptr;

    setCfgFile(cfgFileName);
}

TopTagger::~TopTagger()
{
    if(topTaggerResults_) delete topTaggerResults_;
}

void TopTagger::setCfgFile(const std::string& cfgFileName)
{
    //Read cfg file text

    //Check that filename exists
    if(cfgFileName.size() < 1)
    {
        throw "TopTagger::setCfgFile(...) : No configuration file name given";
    }

    //buffer to hold file contents 
    std::string cfgText;

    //Read text from file
    FILE *f = fopen(cfgFileName.c_str(), "r");
    if(f)
    {
        char buff[1024];
        for(; !feof(f) && fgets(buff, 1023, f);)
        {
            cfgText += buff;
        }
        
        fclose(f);
    }
    else
    {
        throw "TopTagger::setCfgFile(...) : Invalid configuration file name \"" + cfgFileName + "\"";
    }

    //pass raw text to cfg parser, to return parsed document
    cfgDoc_ =  cfg::CfgDocument::parseDocument(cfgText);

    //Get TopTagger parameters
    getParameters();
}

void TopTagger::setCfgFileDirect(const std::string& cfgText)
{
    //pass raw text to cfg parser, to return parsed document
    cfgDoc_ =  cfg::CfgDocument::parseDocument(cfgText);

    //Get TopTagger parameters
    getParameters();
}

void TopTagger::getParameters()
{
    //Construct TopTagger context
    cfg::Context cxt("TopTagger");

    //Get list of modules to use
    int iModule = 0;
    bool keepLooping;
    do
    {
        keepLooping = false;

        //Get module name
        std::string moduleName = cfgDoc_->get("module", iModule, cxt, "");

        //if it is a non empty string look for module
        if(moduleName.size() > 0)
        {
            //Check in module map for this module
            if(TTMFactory::moduleExists(moduleName))
            {
                //if a module is found, try again until one isn't
                keepLooping = true;

                //Create module and add to module to vector
                topTaggerModules_.emplace_back(TTMFactory::createModule(moduleName));
            }
            else
            {
                throw "TopTagger::getParameters() : No module named \"" + moduleName + "\" exists"; 
            }
        }
        ++iModule;
    }
    while(keepLooping);
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
