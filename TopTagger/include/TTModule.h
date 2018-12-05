#ifndef TTMODULE_H
#define TTMODULE_H

/*! \file */ 

#include "TopTagger/TopTagger/include/TTMFactory.h"

#include <functional>

class TopTaggerResults;

namespace cfg
{
    class CfgDocument;
}

/**
 *This is a pure virtual base class which defines the interface for a top tagger module.  This implements 2 pure virtual functions which must be overridden by all modules.  The first, "getParameters," is used to load any configuration parameters the module may need from the config file.  The second, "run," is used to run the particular algorithm implemented in the module.  None of these functions should even be called directly, but instead are called automatically by the TopTagger class.  It is also important that each module also include the line "REGESTER_TTMODULE(ModuleClassName)" after the class declaration.  This macro calls special code so that the TTMFactory class can dynamically implement the module by name.
 */

class TTModule
{
private:

protected:
    ///Stores the location where the tagger will resolve relative file paths 
    std::string workingDirectory_;
    
public:    

    /**
     *Virtual base distructor in case other daughter classes need distructors 
     */
    virtual ~TTModule() = default;

    /**
     *This function is called by TopTagger to configure each module based upon the information present in the configuration file. The inputs for this function are provided by TopTagger and are the configuration document object and the local context string for this module.  The local context string defines where to look in the configuration file for the local parameters.
     */
    virtual void getParameters(const cfg::CfgDocument*, const std::string&) = 0;
    /**
     *run is called automatically by TopTagger once per event to run th emodule.  The module interfaces with other modules through the TopTaggerResults object which is passed as a non-const reference from TopTagger.
     */
    virtual void run(TopTaggerResults&) = 0;

    /**
     *This function sets the base directory from which all files accessed through relative paths should be referenced 
     */
    void setWorkingDirectory(const std::string& workingDir)
    {
        workingDirectory_ = workingDir;
    }
};

//Macro to suppress the incorrect "unused variable" warnings for module factory instantiation 
#ifdef __GNUC__
#define TT_VARIABLE_IS_NOT_USED __attribute__ ((unused))
#else
#define TT_VARIABLE_IS_NOT_USED
#endif

/*! \def REGISTER_TTMODULE
 *Magic macro for registering classes with the factory object.  
 *Every module definition must contain the line "REGISTER_TTMODULE(ModuleClassName);".
 */
#define REGISTER_TTMODULE( _module ) \
    static bool TT_VARIABLE_IS_NOT_USED _module ## _module_created = TTMFactory::registerModule( #_module, []()->TTModule*{ return new _module(); } )

#endif
