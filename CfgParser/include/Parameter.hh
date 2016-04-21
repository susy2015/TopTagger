#ifndef Parameter_hh_included
#define Parameter_hh_included 1

#include "TopTagger/CfgParser/include/Language.hh"
#include "TopTagger/CfgParser/include/Condition.hh"
#include <set>

namespace cfg
{

    class Parameter
    {
    public:
        Parameter(const std::string& ns, const std::string& name);
        void addAssignment(const ConditionChain& c, const Literal& val);
        Literal valueInContext(const Context& cxt,  bool exceptionMissing=false) const;
        const std::string& name() const { return m_name; }
        const std::string& ns() const { return m_namespace; }
        std::set<std::string> conditionalItems() const;
    private:
        struct Assignment
        {
            Assignment(const ConditionChain& c, const Literal& l) : condition(c), value(l) { }
            ConditionChain condition;
            Literal value;
        };
        std::string m_namespace;
        std::string m_name;
        std::vector<Assignment> m_assignments;
    };

}

#endif // Parameter_hh_included
