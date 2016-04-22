#include "TopTagger/CfgParser/include/Language.hh"
#include <stdlib.h>
//#include "hcal/exception/CfgLanguageException.hh"
#include "TopTagger/CfgParser/include/TTException.h"

namespace cfg {

    std::string capitalize(const std::string& s) {
        std::string ss;
        ss.reserve(s.size());
        for (std::string::size_type i=0; i<s.length(); i++)
            ss.push_back(toupper(s[i]));
        return ss;
    }

    Literal::Literal() : m_flavor(fl_Null) {
    }

    Literal::Literal(const std::string& val) : m_flavor(fl_String){
        m_value.s=val;
    }
    Literal::Literal(const char* val) : m_flavor(fl_String){
        m_value.s=val;
    }
    Literal::Literal(const bool& val) : m_flavor(fl_Boolean){
        m_value.b=val;
    }
    Literal::Literal(const int& val) : m_flavor(fl_Integer){
        m_value.i=val;
    }
    Literal::Literal(const double& val) : m_flavor(fl_Float){
        m_value.f=val;
    }
    Literal Literal::create(const std::string& text) 
    {
        if (text.empty())  return Literal(""); // empty is a string
        if (text=="false") return Literal(false);
        if (text=="true")  return Literal(true);
        if (text.length()>1 && text[0]=='"' && text[text.length()-1]=='"') 
        {
            return Literal(text.substr(1,text.length()-2));
        }
        if(text.find(".") != std::string::npos) return Literal(std::stod(text.c_str()));
        return Literal(std::stoi(text.c_str(),0,0));
    }
    int Literal::intValue() const { return m_value.i; }
    double Literal::floatValue() const {return m_value.f; }
    std::string Literal::strValue() const { return m_value.s; }
    bool Literal::boolValue() const { return m_value.b; }
    
    bool Literal::operator==(const Literal& l) const {
        if (l.m_flavor!=m_flavor) {
            //XCEPT_RAISE(hcal::exception::CfgLanguageException,"Flavor mismatch");
            //throw "CfgLanguageException: Flavor mismatch";
            THROW_TTEXCEPTION("CfgLanguageException: Flavor mismatch");
        }
        switch (m_flavor) {
        case (fl_Null) : return true; // all NULL are the same
        case (fl_String) : return m_value.s==l.m_value.s;
        case (fl_Integer) : return m_value.i==l.m_value.i;
        case (fl_Boolean) : return m_value.b==l.m_value.b;
        default : return false;
            //XCEPT_RAISE(hcal::exception::CfgLanguageException,"Unknown flavor");
            //throw "CfgLanguageException: Unknown flavor";
            THROW_TTEXCEPTION("CfgLanguageException: Unknown flavor");
        }
    }

    bool Literal::operator!=(const Literal& l) const {
        if (l.m_flavor!=m_flavor) {
            //XCEPT_RAISE(hcal::exception::CfgLanguageException,"Flavor mismatch");
            //throw "CfgLanguageException: Flavor mismatch";
            THROW_TTEXCEPTION("CfgLanguageException: Flavor mismatch");
        }
        switch (m_flavor) {
        case (fl_Null) : return false; // all NULL are the same
        case (fl_String) : return m_value.s!=l.m_value.s;
        case (fl_Integer) : return m_value.i!=l.m_value.i;
        case (fl_Boolean) : return m_value.b!=l.m_value.b;
        default : return false;
            //XCEPT_RAISE(hcal::exception::CfgLanguageException,"Unknown flavor");
            //throw "CfgLanguageException: Unknown flavor";
            THROW_TTEXCEPTION("CfgLanguageException: Unknown flavor");
        }
    }

    bool Literal::operator>=(const Literal& l) const {
        if (l.m_flavor!=m_flavor) {
            //XCEPT_RAISE(hcal::exception::CfgLanguageException,"Flavor mismatch");
            //throw "CfgLanguageException: Flavor mismatch";
            THROW_TTEXCEPTION("CfgLanguageException: Flavor mismatch");
        }
        switch (m_flavor) {
        case (fl_String) : return m_value.s>=l.m_value.s;
        case (fl_Integer) : return m_value.i>=l.m_value.i;
        case (fl_Boolean) : return m_value.b>=l.m_value.b;
        default : return false;
            //XCEPT_RAISE(hcal::exception::CfgLanguageException,"Unknown flavor");
            //throw "CfgLanguageException: Unknown flavor";
            THROW_TTEXCEPTION("CfgLanguageException: Unknown flavor");
        }
    }

    bool Literal::operator<=(const Literal& l) const {
        if (l.m_flavor!=m_flavor) {
            //XCEPT_RAISE(hcal::exception::CfgLanguageException,"Flavor mismatch");
            //throw "CfgLanguageException: Flavor mismatch";
            THROW_TTEXCEPTION("CfgLanguageException: Flavor mismatch");
        }
        switch (m_flavor) {
        case (fl_String) : return m_value.s<=l.m_value.s;
        case (fl_Integer) : return m_value.i<=l.m_value.i;
        case (fl_Boolean) : return m_value.b<=l.m_value.b;
        default : return false;
            //XCEPT_RAISE(hcal::exception::CfgLanguageException,"Unknown flavor");
            //throw "CfgLanguageException: Unknown flavor";
            THROW_TTEXCEPTION("CfgLanguageException: Unknown flavor");
        }
    }

    bool Literal::operator>(const Literal& l) const {
        if (l.m_flavor!=m_flavor) {
            //XCEPT_RAISE(hcal::exception::CfgLanguageException,"Flavor mismatch");
            //throw "CfgLanguageException: Flavor mismatch";
            THROW_TTEXCEPTION("CfgLanguageException: Flavor mismatch");
        }
        switch (m_flavor) {
        case (fl_String) : return m_value.s>l.m_value.s;
        case (fl_Integer) : return m_value.i>l.m_value.i;
        case (fl_Boolean) : return m_value.b>l.m_value.b;
        default : return false;
            //XCEPT_RAISE(hcal::exception::CfgLanguageException,"Unknown flavor");
            //throw "CfgLanguageException: Unknown flavor";
            THROW_TTEXCEPTION("CfgLanguageException: Unknown flavor");
        }
    }
    bool Literal::operator<(const Literal& l) const {
        if (l.m_flavor!=m_flavor) {
            //XCEPT_RAISE(hcal::exception::CfgLanguageException,"Flavor mismatch");
            //throw "CfgLanguageException: Flavor mismatch";
            THROW_TTEXCEPTION("CfgLanguageException: Flavor mismatch");
        }
        switch (m_flavor) {
        case (fl_String) : return m_value.s<l.m_value.s;
        case (fl_Integer) : return m_value.i<l.m_value.i;
        case (fl_Boolean) : return m_value.b<l.m_value.b;
        default : return false;
            //XCEPT_RAISE(hcal::exception::CfgLanguageException,"Unknown flavor");
            //throw "CfgLanguageException: Unknown flavor";
            THROW_TTEXCEPTION("CfgLanguageException: Unknown flavor");
        }
    }

    std::ostream& Literal::put(std::ostream& s) const {
        switch (m_flavor) {
        case(fl_String) : s << '"' << m_value.s << '"'; break;
        case(fl_Integer) : s << m_value.i; break;
        case(fl_Boolean) : s << ((m_value.b)?("true"):("false")); break;
        default: break;
        }
        return s;
    }
}


std::ostream& operator<<(std::ostream& s, const cfg::Literal& l) {
    return l.put(s);
}
