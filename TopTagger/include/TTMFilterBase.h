#ifndef TTMFILTERBASE_H
#define TTMFILTERBASE_H

#include <set>
#include <vector>

class Constituent;

class TTMFilterBase
{
protected:

    bool constituentsAreUsed(const std::vector<const Constituent *>&, const std::set<const Constituent*>&, const double) const ;
    void markConstituentsUsed(const std::vector<const Constituent *>&, const std::vector<Constituent>&, std::set<const Constituent*>&, const double) const ;
};


#endif
