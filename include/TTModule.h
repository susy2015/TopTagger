#ifndef TTMODULE_H
#define TTMODULE_H

class TopTaggerResults;

class TTModule
{
private:
    
public:
    virtual void run(TopTaggerResults&) = 0;
};

#endif
