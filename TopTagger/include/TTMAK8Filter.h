#ifndef TTMAK8FILTER_H
#define TTMAK8FILTER_H

#include "TopTagger/TopTagger/include/TTModule.h"

class TopTaggerResults;
class Constituent;

class TTMAK8Filter : public TTModule
{
private:

public:
    void getParameters(const cfg::CfgDocument*);
    void run(TopTaggerResults&);
};
REGISTER_TTMODULE(TTMAK8Filter);

#endif
