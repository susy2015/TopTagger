#include "TopTagger/CfgParser/include/Record.hh"
#include "TopTagger/CfgParser/include/CfgDocument.hh"
//#include "hcal/exception/CfgLanguageException.hh"
#include "TopTagger/CfgParser/include/Scanner.h"
#include "TopTagger/CfgParser/include/Parser.h"

namespace cfg {

    //CfgDocument::CfgDocument(log4cplus::Logger l) : m_logger(l) {
    CfgDocument::CfgDocument(){
        m_recordPtr=0;
    }

    std::unique_ptr<CfgDocument> CfgDocument::parseDocument(const std::string& text) 
    {
        if (text.size()<5) 
        {
            return std::unique_ptr<CfgDocument>();
        } 
        else 
        {
            hcalcfg::Scanner sc(text);
            hcalcfg::Parser p(&sc);
            p.m_builder.setup();
            p.Parse();
            return std::unique_ptr<CfgDocument>(p.m_builder.takeDoc());
        }
    }

    CfgDocument::~CfgDocument() {
        for (std::map<std::string, Parameter*>::iterator i=m_parameters.begin(); i!=m_parameters.end(); i++)
            delete i->second;
        for (std::vector<Condition*>::iterator i=m_conditions.begin(); i!=m_conditions.end(); i++)
            delete *i;
    }
    
    Condition* CfgDocument::addCondition() {
        Condition* c=new Condition();
        m_conditions.push_back(c);
        return c;
    }

    void CfgDocument::useRecord(Record* r) { m_recordPtr=r; }


    void CfgDocument::assignParameter(const std::string& ns, const std::string& name, const ConditionChain& cc, const Literal& l) {
        std::string key=makeKey(ns,name);
        std::map<std::string, Parameter*>::iterator i=m_parameters.find(key);
        if (i==m_parameters.end()) {
            m_parameters.insert(std::pair<std::string,Parameter*>(key,new Parameter(ns,name)));
            i=m_parameters.find(key);
        }
        i->second->addAssignment(cc,l);
    }

    Literal CfgDocument::get(const std::string& name, const Context& cxt, const Literal& defl)
    {
        std::string key=makeKey(cxt.ns(),name);
        std::map<std::string, Parameter*>::const_iterator i=m_parameters.find(key);
        if (i==m_parameters.end()) 
        {
            if (m_recordPtr!=0) m_recordPtr->record(cxt, name, defl, true);
            return defl;
        } 
        else
        {
            Literal l=i->second->valueInContext(cxt,true);
            if (l.flavor()==Literal::fl_Null) 
            {
                l=defl;
                if (m_recordPtr!=0) m_recordPtr->record(cxt, name, defl, true);
            }
            else if (m_recordPtr!=0) m_recordPtr->record(cxt, name, l, false);
            return l;
        }
    }

    int CfgDocument::get(const std::string& name, const Context& cxt, int defl)
    {
        Literal l=get(name,cxt,Literal(defl));
        if (l.flavor()!=Literal::fl_Integer) 
        {
            //XCEPT_RAISE(hcal::exception::CfgLanguageException,"Type mismatch (expected int)");
            throw "CfgLanguageException: Type mismatch (expected int)";
        }
        return l.intValue();
    }
    
    double CfgDocument::get(const std::string& name, const Context& cxt, double defl) 
    {
        Literal l=get(name,cxt,Literal(defl));
        
        if(l.flavor() == Literal::fl_Float) 
        {
            return l.floatValue();
        }
        else if(l.flavor() == Literal::fl_Integer)
        {
            return static_cast<double>(l.intValue());
        }
        else
        {
            //XCEPT_RAISE(hcal::exception::CfgLanguageException,"Type mismatch (expected int)");
            throw "CfgLanguageException: Type mismatch (expected float)";
        }
        
    }

    bool CfgDocument::get(const std::string& name, const Context& cxt, bool defl) {
        Literal l=get(name,cxt,Literal(defl));
        if (l.flavor()!=Literal::fl_Boolean) {
            //XCEPT_RAISE(hcal::exception::CfgLanguageException,"Type mismatch (expected bool)");
            throw "CfgLanguageException: Type mismatch (expected bool)";
        }
        return l.boolValue();
    }
    std::string CfgDocument::get(const std::string& name, const Context& cxt, const std::string& defl) {
        Literal l=get(name,cxt,Literal(defl));
        if (l.flavor()!=Literal::fl_String) {
            //XCEPT_RAISE(hcal::exception::CfgLanguageException,"Type mismatch (expected string)");
            throw "CfgLanguageException: Type mismatch (expected string)";
        }
        return l.strValue();
    }
    std::string CfgDocument::get(const std::string& name, const Context& cxt, const char* defl) {
        Literal l=get(name,cxt,Literal(defl));
        if (l.flavor()!=Literal::fl_String) {
            //XCEPT_RAISE(hcal::exception::CfgLanguageException,"Type mismatch (expected string)");
            throw "CfgLanguageException: Type mismatch (expected string)";
        }
        return l.strValue();
    }

    int CfgDocument::get(const std::string& name, int index, const Context& cxt, int defl) {
        std::string fullName(name);
        char text[20];
        snprintf(text,20,"[%d]",index);
        fullName+=text;
        return get(fullName,cxt,defl);
    }

    double CfgDocument::get(const std::string& name, int index, const Context& cxt, double defl) {
        std::string fullName(name);
        char text[20];
        snprintf(text,20,"[%d]",index);
        fullName+=text;
        return get(fullName,cxt,defl);
    }

    std::string CfgDocument::get(const std::string& name, int index, const Context& cxt, const std::string& defl) {
        std::string fullName(name);
        char text[20];
        snprintf(text,20,"[%d]",index);
        fullName+=text;
        return get(fullName,cxt,defl);
    }

    std::string CfgDocument::get(const std::string& name, int index, const Context& cxt, const char* defl) {
        std::string fullName(name);
        char text[20];
        snprintf(text,20,"[%d]",index);
        fullName+=text;
        return get(fullName,cxt,defl);
    }

    bool CfgDocument::get(const std::string& name, int index, const Context& cxt, bool defl) {
        std::string fullName(name);
        char text[20];
        snprintf(text,20,"[%d]",index);
        fullName+=text;
        return get(fullName,cxt,defl);
    }


    void CfgDocument::postValueUsed(const std::string& name, const Context& cxt, int value) {
        Literal l(value);
        if (m_recordPtr!=0) m_recordPtr->record(cxt, name, l, true);
    }
    void CfgDocument::postValueUsed(const std::string& name, const Context& cxt, const char* value) {
        Literal l(value);
        if (m_recordPtr!=0) m_recordPtr->record(cxt, name, l, true);
    }
    void CfgDocument::postValueUsed(const std::string& name, const Context& cxt, const std::string& value) {
        Literal l(value);
        if (m_recordPtr!=0) m_recordPtr->record(cxt, name, l, true);
    }
    void CfgDocument::postValueUsed(const std::string& name, const Context& cxt, bool value) {
        Literal l(value);
        if (m_recordPtr!=0) m_recordPtr->record(cxt, name, l, true);
    }
    
    std::string CfgDocument::makeKey(const std::string& ns, const std::string& name) const {
        return capitalize(ns)+"::"+capitalize(name);
    }

}
