#include "TopTagger/CfgParser/interface/Parameter.hh"
//#include "hcal/exception/CfgLanguageException.hh"
#include "TopTagger/CfgParser/interface/TTException.h"
//#include <iostream>
#include <string>

namespace cfg
{

    Parameter::Parameter(const std::string& ns, const std::string& name) :
        m_namespace(ns), m_name(name)
    {
    }

    void Parameter::addAssignment(const ConditionChain& c, const Literal& val)
    {
        m_assignments.push_back(Assignment(c,val));
        //      std::cout << m_namespace << "::" << m_name << ": if " << c << " --> " << val << std::endl;
    }

    Literal Parameter::valueInContext(const Context& cxt, bool exceptMissing) const 
    {
        // come up from the bottom, checking conditions
        for (std::vector<Assignment>::const_reverse_iterator i=m_assignments.rbegin(); i!=m_assignments.rend(); i++)
        {
            try
            {
                if (i->condition.test(cxt)) return i->value;
            }
            //catch (hcal::exception::CfgMissingFeatureException& e)
            catch (const TTException e)
            {
                if (exceptMissing) throw e;
            }
        }
        return Literal(); // NULL!
    }

    std::set<std::string> Parameter::conditionalItems() const
    {
        std::set<std::string> retval;
        for (std::vector<Assignment>::const_iterator i=m_assignments.begin(); i!=m_assignments.end(); i++)
        {
            std::set<std::string> subitems=i->condition.queriedItems();
            retval.insert(subitems.begin(),subitems.end());
        }
        return retval;
    }

}
