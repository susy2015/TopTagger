#ifndef TTMLAZYCLUSTERALGO_H
#define TTMLAZYCLUSTERALGO_H

#include "TopTagger/TopTagger/include/TTModule.h"

class TopTaggerResults;

class TTMLazyClusterAlgo : public TTModule
{
private:

public:
    void run(TopTaggerResults&);
};

#endif
