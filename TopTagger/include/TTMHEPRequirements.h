#ifndef TTMHEPREQUIREMENTS_H
#define TTMHEPREQUIREMENTS_H

#include "TopTagger/TopTagger/include/TTModule.h"

class TopTaggerResults;

class TTMHEPRequirements : public TTModule
{
private:

public:
    void run(TopTaggerResults&);
};

#endif
