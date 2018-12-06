#include "TopTagger/TopTagger/interface/TTMFactory.h"

#include "TopTagger/TopTagger/interface/TTModule.h"

//c++ is dumb and makes me declare static members again
TTMFactory* TTMFactory::instance_;

TTMFactory* TTMFactory::getInstance()
{
    if(!instance_) instance_ = new TTMFactory();

    return instance_;
}

bool TTMFactory::registerModule(const std::string& name, std::function<TTModule*()> creator)
{
    TTMFactory *fact = getInstance();

    //Check that the function is valid
    if(!creator) return false;

    fact->creators_[name] = creator;

    return true;
}

bool TTMFactory::moduleExists(const std::string& name)
{
    TTMFactory *fact = getInstance();

    return fact->creators_.find(name) != fact->creators_.end();
}

TTModule* TTMFactory::createModule(const std::string& name)
{
    TTMFactory *fact = getInstance();

    return fact->creators_[name]();
}

