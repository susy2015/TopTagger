#ifndef TTMODULE_H
#define TTMODULE_H

#include "TopTagger/TopTagger/include/TTMFactory.h"

#include <functional>

class TopTaggerResults;
class CfgDocument;

//Base Object for Top Taggger Modules
class TTModule
{
private:
    CfgDocument* cfgDoc_;
    
public:    

    void setCfgDoc(CfgDocument* cfgDoc) { cfgDoc_ = cfgDoc; }

    //virtual void getParameters() = 0;
    virtual void run(TopTaggerResults&) = 0;
};

//magic macro for registering classes with the factory object
#define REGESTER_TTMODULE( _module ) \
    static bool _module ## _module_created = TTMFactory::registerModule( #_module, []()->TTModule*{ return new _module(); } )

#endif
