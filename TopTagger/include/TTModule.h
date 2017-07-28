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

//Macro to suppress the incorrect "unused variable" warnings for module factory instantiation 
#ifdef __GNUC__
#define TT_VARIABLE_IS_NOT_USED __attribute__ ((unused))
#else
#define TT_VARIABLE_IS_NOT_USED
#endif

//magic macro for registering classes with the factory object
#define REGISTER_TTMODULE( _module ) \
    static bool TT_VARIABLE_IS_NOT_USED _module ## _module_created = TTMFactory::registerModule( #_module, []()->TTModule*{ return new _module(); } )

#endif
