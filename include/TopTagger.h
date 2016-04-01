#ifndef TOPTAGGER_H
#define TOPTAGGER_H

#include <vector>
#include <memory>

#include "Constituent.h"

class TTModule;
class TopTaggerResults;

class TopTagger
{
private:
    //class to hold all toptagger information
    //this is passed to modules as non-const and to the outside world as a const ref
    TopTaggerResults *topTaggerResults_;

    //List of modules to be run, all are based upon the TTModule base class
    std::vector<TTModule *> topTaggerModules_;

public:
    TopTagger();

    //Adds new module to the end of the module vector 
    void registerModule(TTModule *);

    //Runs the top tagger modules specified.  Runs once per event
    void runTagger(const std::vector<const Constituent>*);

    //Getters
    const TopTaggerResults& getResults();

};

#endif
