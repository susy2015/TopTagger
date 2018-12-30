#include "TopTagger/TopTagger/interface/TopTagger.h"

#include "TopTagger/TopTagger/interface/TopObject.h"
#include "TopTagger/TopTagger/interface/TTModule.h"
#include "TopTagger/TopTagger/interface/TopTaggerResults.h"

#include "TopTagger/CfgParser/include/TTException.h"
#include "TopTagger/CfgParser/include/CfgDocument.hh"
#include "TopTagger/CfgParser/include/Record.hh"
#include "TopTagger/CfgParser/include/Context.hh"

TopTagger::TopTagger() : topTaggerResults_(nullptr), verbosity_(1), reThrow_(true), workingDirectory_()
{
}

TopTagger::TopTagger(const std::string& cfgFileName, const std::string& workingDir) : TopTagger()
{
    workingDirectory_ = workingDir;
    setCfgFile(cfgFileName);
}

TopTagger::~TopTagger()
{
    if(topTaggerResults_) delete topTaggerResults_;
}

void TopTagger::setCfgFile(const std::string& cfgFileName)
{
    //try-catch the entire function - exceptions rethrown by default
    try
    {
        //Read cfg file text

        //Check that filename exists
        if(cfgFileName.size() < 1)
        {
            //throw "TopTagger::setCfgFile(...) : No configuration file name given";
            THROW_TTEXCEPTION("No configuration file name given");
        }

        //buffer to hold file contents 
        std::string cfgText;

        //Read text from file
        std::string cfgFileNameAndPath;
        if(workingDirectory_.size() > 0)
        {
            cfgFileNameAndPath = workingDirectory_ + "/" + cfgFileName;
        }
        else
        {
            cfgFileNameAndPath = cfgFileName;
        }


        FILE *f = fopen(cfgFileNameAndPath.c_str(), "r");

        


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
            //throw "TopTagger::setCfgFile(...) : Invalid configuration file name \"" + cfgFileName + "\"";
            THROW_TTEXCEPTION("Invalid configuration file name \"" + cfgFileNameAndPath + "\"");
        }

        //pass raw text to cfg parser, to return parsed document
        cfgDoc_ =  cfg::CfgDocument::parseDocument(cfgText);

        //Get TopTagger parameters
        getParameters();
    }
    catch(const TTException& e)
    {
        handelException(e);
    }
}

void TopTagger::setCfgFileDirect(const std::string& cfgText)
{
    //try-catch the entire function - exceptions rethrown by default
    try
    {
        //pass raw text to cfg parser, to return parsed document
        cfgDoc_ =  cfg::CfgDocument::parseDocument(cfgText);

        //Get TopTagger parameters
        getParameters();
    }
    catch(const TTException& e)
    {
        handelException(e);
    }
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
        std::string contextName = cfgDoc_->get("context", iModule, cxt, "");

        //if it is a non empty string look for module
        if(moduleName.size() > 0)
        {
            //Check in module map for this module
            if(TTMFactory::moduleExists(moduleName))
            {
                //if a module is found, try again until one isn't
                keepLooping = true;

                //if the context name is empty, use the moduleName
                if(contextName.size() == 0) contextName = moduleName;

                //Create module and add to module to vector
                topTaggerModules_.emplace_back(TTMFactory::createModule(moduleName));
                //Set working directory 
                topTaggerModules_.back()->setWorkingDirectory(workingDirectory_);
                //configure the new module from the config document
                topTaggerModules_.back()->getParameters(cfgDoc_.get(), contextName);
            }
            else
            {
                //throw "TopTagger::getParameters() : No module named \"" + moduleName + "\" exists"; 
                THROW_TTEXCEPTION("No module named \"" + moduleName + "\" exists");
            }
        }
        ++iModule;
    }
    while(keepLooping);
}

void TopTagger::runTagger(std::vector<Constituent>&& constituents)
{
    //try-catch the entire function - exceptions rethrown by default
    try
    {
        if(topTaggerResults_) delete topTaggerResults_;
        topTaggerResults_ = new TopTaggerResults(std::forward<std::vector<Constituent>>(constituents));

        for(std::unique_ptr<TTModule>& module : topTaggerModules_)
        {
            module->run(*topTaggerResults_);
        }
    }
    catch(const TTException& e)
    {
        handelException(e);
    }
}

void TopTagger::runTagger(const std::vector<Constituent>& constituents)
{
    //try-catch the entire function - exceptions rethrown by default
    try
    {
        if(topTaggerResults_) delete topTaggerResults_;
        topTaggerResults_ = new TopTaggerResults(constituents);

        for(std::unique_ptr<TTModule>& module : topTaggerModules_)
        {
            module->run(*topTaggerResults_);
        }
    }
    catch(const TTException& e)
    {
        handelException(e);
    }
}

const TopTaggerResults& TopTagger::getResults() const
{
    //try-catch the entire function - exceptions rethrown by default
    try
    {
        if(topTaggerResults_) return *topTaggerResults_;
        else
        {
            //throw "const TopTaggerResults& TopTagger::getResults() : Invalid TopTaggerResults ptr";
            THROW_TTEXCEPTION("Invalid TopTaggerResults ptr");
        }
    }
    catch(const TTException& e)
    {
        handelException(e);
    }

    return *static_cast<TopTaggerResults*>(nullptr);
}

void TopTagger::handelException(const TTException& e) const
{
    if(verbosity_ >= 1) e.print();
    if(reThrow_) throw e;
}
