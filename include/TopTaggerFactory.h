#ifndef TOPTAGGERFACTORY_H
#define TOPTAGGERFACTORY_H

#include <vector>
#include <memory>

class TTModule;
class TopTagger;

class TopTaggerFactory
{
private:

public:
    TopTaggerFactory() {}

    ~TopTaggerFactory();

    std::unique_ptr<TopTagger> makeTopTagger(std::string = "default");
};

#endif
