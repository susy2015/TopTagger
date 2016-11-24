#ifndef TTMODULE_H
#define TTMODULE_H

#include "TopTagger/TopTagger/include/TTMFactory.h"

#include <functional>

class TopTaggerResults;

namespace cfg
{
    class CfgDocument;
}

//Base Object for Top Taggger Modules
class TTModule
{
private:

protected:
    
public:    

    virtual void getParameters(const cfg::CfgDocument*, const std::string&) = 0;
    virtual void run(TopTaggerResults&) = 0;
};

//magic macro for registering classes with the factory object
#define REGISTER_TTMODULE( _module ) \
    static bool _module ## _module_created = TTMFactory::registerModule( #_module, []()->TTModule*{ return new _module(); } )

#endif
