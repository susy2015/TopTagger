#ifndef Condition_hh_included
#define Condition_hh_included 1

#include <vector>
#include <memory>
#include <set>
#include "TopTagger/CfgParser/interface/Language.hh"
#include "TopTagger/CfgParser/interface/Context.hh"

namespace cfg {
 

    class Term {
    public:
        virtual ~Term() { }
        virtual bool matches(const Context& cxt) const = 0;
        virtual std::ostream& put(std::ostream& o) const = 0;
        virtual std::set<std::string> queriedItems() const = 0;
    };

    class TermAnd : public Term {
    public:
        TermAnd();
        virtual ~TermAnd();
        virtual bool matches(const Context& cxt) const;
        void And(Term* t);
        virtual std::set<std::string> queriedItems() const;
        virtual std::ostream& put(std::ostream& o) const;
    private:
        std::vector<Term*> m_terms;
    };
   
    class SimpleTerm : public Term {
    public:
        SimpleTerm(const std::string& itemName, const std::string& op, const Literal& value);
        /// "IN"
        SimpleTerm(const std::string& itemName, const std::string& op, const std::vector<Literal>& value); 
        virtual ~SimpleTerm() {}
        virtual bool matches(const Context& cxt) const;
        virtual std::set<std::string> queriedItems() const;
        virtual std::ostream& put(std::ostream& o) const;
    private:
        std::string m_item;
        std::string m_op;
        std::vector<Literal> m_values;
    };

    class Condition 
    {
    public: 
        Condition();
        void set(TermAnd* ta);
        bool test(const Context& cxt) const;
        void andTerm(Term* t);
        std::ostream& put(std::ostream& o) const;
        std::set<std::string> queriedItems() const { return m_term->queriedItems(); }
    private:
        std::unique_ptr<TermAnd> m_term;
    };

    class ConditionChain {
    public:
        ConditionChain() { }
        void push(const Condition* c);
        void pop();
        bool test(const Context& cxt) const;
        std::ostream& put(std::ostream& o) const;
        std::set<std::string> queriedItems() const;
    private:
        std::vector<const Condition*> m_conds;
    };

}

std::ostream& operator<<(std::ostream& o, const cfg::ConditionChain& c);
std::ostream& operator<<(std::ostream& o, const cfg::Condition& c);

#endif // Condition_hh_included
