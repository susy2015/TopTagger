#ifndef TOPTAGGER_H
#define TOPTAGGER_H

#include <vector>
#include <memory>

class Constituent;
class TTModule;
class TopTaggerResults;

namespace cfg
{
    class CfgDocument;
    class Record;
}

class TopTagger
{
private:
    //class to hold all toptagger information
    //this is passed to modules as non-const and to the outside world as a const ref
    TopTaggerResults *topTaggerResults_;

    //List of modules to be run, all are based upon the TTModule base class
    std::vector<std::unique_ptr<TTModule>> topTaggerModules_;

    //config parser 
    std::unique_ptr<cfg::CfgDocument> cfgDoc_;
    //cfg::Record *cfgRecord_;

    void getParameters();

public:
    TopTagger();
    TopTagger(const std::string&);

    ~TopTagger();

    void setCfgFile(const std::string&);
    void setCfgFileDirect(const std::string&);

    //Adds new module to the end of the module vector 
    void registerModule(std::unique_ptr<TTModule>&);

    //Runs the top tagger modules specified.  Runs once per event
    void runTagger(const std::vector<Constituent>&);

    //Getters
    const TopTaggerResults& getResults();

};

#endif
