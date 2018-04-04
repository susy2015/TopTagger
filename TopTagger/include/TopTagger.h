#ifndef TOPTAGGER_H
#define TOPTAGGER_H

#include <vector>
#include <memory>
#include <string>

class Constituent;
class TTModule;
class TopTaggerResults;
class TTException;

namespace cfg
{
    class CfgDocument;
    class Record;
}

/**
 *This class holds the primary structure of the tagger.  It is responsible for parsing the configuration file for the tagger, instantiating and running the requested modules, and storing the common results objects for the modules and user.
 *
 *The TopTagger module is the primary (and only mandatory) section in every top tagger configuration.  This section defines all the other top tagger modules which will be run and in which order.  This module has 2 variables (both arrays) Which are used to define the module run order and if necessary the module context name.
 *@param module[] (string) This variable is an array and is used to define which other modules will be run and in which order.  This can be any module listed here in this section.
 *@param context[] (string) This variable must be specified for any module being run more than once to specify what context name to read its configuration from.
*/
class TopTagger
{
private:
    ///class to hold all toptagger information
    ///this is passed to modules as non-const and to the outside world as a const ref
    TopTaggerResults *topTaggerResults_;

    ///List of modules to be run, all are based upon the TTModule base class
    std::vector<std::unique_ptr<TTModule>> topTaggerModules_;

    ///config parser 
    std::unique_ptr<cfg::CfgDocument> cfgDoc_;
    //cfg::Record *cfgRecord_;

    ///tagger configuration parameters
    int verbosity_;
    bool reThrow_;

    void getParameters();
    void handelException(const TTException& e) const;

public:
    ///Default constructor to create an empty TopTagger object
    TopTagger();
    ///Constructor to initialize TopTagger object from the provided configuration file 
    TopTagger(const std::string&);

    ~TopTagger();

    /**
     *Set the verbosity level of the top tagger \n
     * 0 - print nothing \n
     * 1 - print exception error messages 
     */       
    void setVerbosity(const int verbosity) { verbosity_ = verbosity; }
    /**
     *If set to true the TopTagger internal error handeling will rethrow TTExceptions,
     *otherwise the exceptions will be handled internally. 
     */       
    void setRethrow(const bool reThrow) { reThrow_ = reThrow; }

    /**
     *Set the configuration file to use to configure the TopTagger object.
     *This function expects the path to an external configuration file.
     */
    void setCfgFile(const std::string&);
    /**
     *Set the configuration file directly from a string.
     *This function expects the configuration in the format of a raw string.
     */
    void setCfgFileDirect(const std::string&);

    /**
     *Runs the top tagger modules specified in the configuration file.  Run once per event.
     */
    void runTagger(const std::vector<Constituent>&);

    //Getters

    /**
     *Gets the top tagger results object from the external user.  
     *Results are returned as a const reference to a TopTaggerResults object.
     */
    const TopTaggerResults& getResults() const;

};

#endif
