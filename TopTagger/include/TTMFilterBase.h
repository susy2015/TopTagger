#ifndef TTMFILTERBASE_H
#define TTMFILTERBASE_H

#include <set>
#include <vector>

class Constituent;

/**
 *This class provides several common functions which can be used by TTModule objects (it is not one itself) to assist in overlap resolution when top candidates sharing the same constituents are selected.
 */
class TTMFilterBase
{
protected:

    /**
     *Checks if the constituents are used
     *
     *@param constituents List of constituents to check
     *@param usedConsts Set of all constituents already used in final reconstructed tops 
     *@param dRMatch The dR requirement used to select whether an AK4 jet matches a AK8 subjet
     */
    bool constituentsAreUsed(const std::vector<const Constituent *>&, const std::set<const Constituent*>&, const double, const double) const ;
    /**
     *Marks constituents as being used in a final reconstructed top 
     *
     *@param constituents List of constituents to mark as used
     *@param allConstituents List of all constituents
     *@param usedConstituents Set of all constituents already used in final reconstructed tops 
     *@param dRMatch The dR requirement used to select whether an AK4 jet matches a AK8 subjet
     */
    void markConstituentsUsed(const std::vector<const Constituent *>&, const std::vector<Constituent>&, std::set<const Constituent*>&, const double, const double) const ;
};


#endif
