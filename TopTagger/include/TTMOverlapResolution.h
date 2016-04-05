#ifndef TTMOVERLAPRESOLUTION_H
#define TTMOVERLAPRESOLUTION_H

#include "TopTagger/TopTagger/include/TTModule.h"

class TopTaggerResults;

class TTMOverlapResolution : public TTModule
{
private:

public:
    void run(TopTaggerResults&);
};

#endif
