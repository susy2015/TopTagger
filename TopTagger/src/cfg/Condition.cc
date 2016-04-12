#include "TopTagger/TopTagger/include/hcal/cfg/Condition.hh"
//#include "hcal/exception/CfgLanguageException.hh"
#include <sstream>

std::ostream& operator<<(std::ostream& o, const hcal::cfg::ConditionChain& c) {
  return c.put(o);
}
std::ostream& operator<<(std::ostream& o, const hcal::cfg::Condition& c) {
  return c.put(o);
}

namespace hcal {
  namespace cfg {


    Condition::Condition() : m_term(new TermAnd) {     
    }
    void Condition::set(TermAnd* ta) {
      m_term=std::auto_ptr<TermAnd>(ta);  
    }

    std::ostream& Condition::put(std::ostream& o) const {
      return m_term->put(o);
    }

    bool Condition::test(const Context& cxt) const {
      return m_term->matches(cxt);
    }

    void Condition::andTerm(Term* t) {
      m_term->And(t);
    }

    std::set<std::string> ConditionChain::queriedItems() const {
      std::set<std::string> items;
      for (std::vector<const Condition*>::const_iterator i=m_conds.begin(); i!=m_conds.end(); i++) {
	std::set<std::string> subitems=(*i)->queriedItems();
	items.insert(subitems.begin(),subitems.end());
      }
      return items;
    }

    void ConditionChain::push(const Condition* c) {
      m_conds.push_back(c);
    }
    void ConditionChain::pop() {
      m_conds.pop_back();
    }
    bool ConditionChain::test(const Context& cxt) const {
      bool ok=true;
      for (std::vector<const Condition*>::const_iterator i=m_conds.begin(); ok && i!=m_conds.end(); i++) {
	ok=ok && (*i)->test(cxt);
      }
      return ok;
    }

    std::ostream& ConditionChain::put(std::ostream& o) const {
      for (std::vector<const Condition*>::const_iterator i=m_conds.begin(); i!=m_conds.end(); i++) {
	if (i!=m_conds.begin()) o << " + ";
	(*i)->put(o);
      }
      return o;
    }

    TermAnd::TermAnd() {
    }

    TermAnd::~TermAnd() {
      for (std::vector<Term*>::iterator i=m_terms.begin(); i!=m_terms.end(); i++)
	delete *i;
    }

    std::set<std::string> TermAnd::queriedItems() const {
      std::set<std::string> items;
      for (std::vector<Term*>::const_iterator i=m_terms.begin(); i!=m_terms.end(); i++) {
	std::set<std::string> subitems=(*i)->queriedItems();
	items.insert(subitems.begin(),subitems.end());
      }
      return items;
    }

    bool TermAnd::matches(const Context& cxt) const {
      bool ok=true;
      for (std::vector<Term*>::const_iterator i=m_terms.begin(); ok && i!=m_terms.end(); i++)
	ok=ok && (*i)->matches(cxt);
      return ok;
    }

    void TermAnd::And(Term* t) {
      m_terms.push_back(t);
    }
    std::ostream& TermAnd::put(std::ostream& o) const {
      for (std::vector<Term*>::const_iterator i=m_terms.begin(); i!=m_terms.end(); i++) {
	if (i!=m_terms.begin()) o << " && ";
	(*i)->put(o);
      }
      return o;
    }
    static const std::string opEQ("==");
    static const std::string opNE("!=");
    static const std::string opGE(">=");
    static const std::string opLE("<=");
    static const std::string opGT(">");
    static const std::string opLT("<");
    static const std::string opIN("IN");

    SimpleTerm::SimpleTerm(const std::string& itemName, const std::string& op, const Literal& value) :
      m_item(itemName), m_op(op) {
      m_values.push_back(value);
      if (m_op!=opEQ && m_op!=opNE &&
	  m_op!=opGE && m_op!=opLE &&
	  m_op!=opGT && m_op!=opLT)
      {
	  //XCEPT_RAISE(hcal::exception::CfgLanguageException,std::string("Unknown operator: ")+m_op);
      }
    }

    SimpleTerm::SimpleTerm(const std::string& itemName, const std::string& op, const std::vector<Literal>& values) :
      m_item(itemName), m_op(capitalize(op)), m_values(values) {
      if (m_op!=opIN) 
	{
            //XCEPT_RAISE(hcal::exception::CfgLanguageException,std::string("Unknown operator: ")+m_op);
	}
    }
    std::set<std::string> SimpleTerm::queriedItems() const {
      std::set<std::string> retval;
      retval.insert(m_item);
      return retval;
    }
    bool SimpleTerm::matches(const Context& cxt) const {
      const Literal& val=cxt.get(m_item);
      if (val.flavor()!=m_values.front().flavor()) {
	std::ostringstream ss;
	ss << val << " (" << m_item << ") not same flavor as " << m_values.front();
	//XCEPT_RAISE(hcal::exception::CfgLanguageException,ss.str());
      }

      if (m_op==opEQ) return val==m_values.front();
      if (m_op==opNE) return val!=m_values.front();
      if (m_op==opLE) return val<=m_values.front();
      if (m_op==opGE) return val>=m_values.front();
      if (m_op==opLT) return val<m_values.front();
      if (m_op==opGT) return val>m_values.front();
      if (m_op==opIN) {
	for (std::vector<Literal>::const_iterator i=m_values.begin(); i!=m_values.end(); i++) {
	  if (val==*i) return true;
	}
      }
      return false; // unreachable
    }
    std::ostream& SimpleTerm::put(std::ostream& o) const {
      if (m_op==opIN) {
	o << m_item << " IN (";
	for (std::vector<Literal>::const_iterator i=m_values.begin(); i!=m_values.end(); i++) {
	  if (i!=m_values.begin()) o << ", ";
	  o << *i;
	}
	o << ")";
      } else
	o << m_item << m_op << m_values.front();
      return o;
    }
  }
}
