#include "TopTagger/CfgParser/include/Context.hh"
//#include "hcal/exception/CfgLanguageException.hh"
#include <ctype.h>

namespace cfg {

    Context::Context(const std::string& namesp) : m_namespace(capitalize(namesp)) {
    }

    Context::Context(const Context& cxt) : m_namespace(cxt.m_namespace), m_features(cxt.m_features) { }
    Context& Context::operator=(const Context& cxt) {
        m_namespace=cxt.m_namespace;
        m_features=cxt.m_features;
        return *this;
    }
    
    void Context::set(const std::string& name, const std::string& value) {
        m_features.insert(std::pair<std::string,Literal>(capitalize(name),Literal(value)));
    }

    void Context::set(const std::string& name, const char* value) {
        set(name,std::string(value));
    }

    void Context::set(const std::string& name, int value) {
        m_features.insert(std::pair<std::string,Literal>(capitalize(name),Literal(value)));
    }

    void Context::set(const std::string& name, bool value) {
        m_features.insert(std::pair<std::string,Literal>(capitalize(name),Literal(value)));
    }
    
    void Context::unset(const std::string& name) {
        m_features.erase(capitalize(name));
    }
    
    bool Context::exists(const std::string& n) const {
        std::string name(capitalize(n));
        if (name=="NAMESPACE") return true;
        else return (m_features.find(name)!=m_features.end());
    }
    
    bool Context::operator==(const Context& c) const {
        if (m_namespace!=c.m_namespace) return false;
        if (m_features.size()!=c.m_features.size()) return false;
        std::map<std::string,Literal>::const_iterator i,j;
        for (i=m_features.begin(); i!=m_features.end(); i++) {
            j=c.m_features.find(i->first);
            if (j==c.m_features.end()) return false;
            if (!(j->second==i->second)) return false;
        }
        return true;
    }

    const Literal& Context::get(const std::string& n) const {
        std::string name(capitalize(n));
        std::map<std::string,Literal>::const_iterator i=m_features.find(name);
        if (i==m_features.end()) {
            //XCEPT_RAISE(hcal::exception::CfgMissingFeatureException,std::string("Feature not found: ")+name);
            throw "CfgMissingFeatureException: Feature not found:" + name;
        }
        return i->second;
    }    
}
