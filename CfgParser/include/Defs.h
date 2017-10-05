#include <string>
#include <vector>
#include <memory>
#include "TopTagger/CfgParser/include/Scanner.h"
#include "TopTagger/CfgParser/include/Language.hh"
#include "TopTagger/CfgParser/include/Condition.hh"
#include "TopTagger/CfgParser/include/CfgDocument.hh"
//#include "log4cplus/logger.h"
//#include "log4cplus/loggingmacros.h"

class CfgBuilder 
{
public:
    //log4cplus::Logger m_logger;
public:
    //CfgBuilder() : m_logger(log4cplus::Logger::getInstance("Hcal.CfgScriptParser")) { theDoc=new cfg::CfgDocument(m_logger); }
CfgBuilder() : theDoc(new cfg::CfgDocument()) {  }

    //void setup(log4cplus::Logger l) { 
    //  m_logger=l; 
    //  delete theDoc;
    //  theDoc=new cfg::CfgDocument(m_logger); 
    //}

    void setup() 
    { 
        theDoc.reset(new cfg::CfgDocument()); 
    }

    std::unique_ptr<cfg::CfgDocument> takeDoc() 
    {
        return std::move(theDoc);
    }

    cfg::SimpleTerm* simpleTerm(const std::string& ident, const std::string& op) 
    {
        cfg::Literal l(literalStack.back()); literalStack.pop_back();
        return new cfg::SimpleTerm(ident,op,l);
    }

    cfg::SimpleTerm* listTerm(const std::string& ident, const std::string& op) 
    {
        cfg::SimpleTerm* rv= new cfg::SimpleTerm(ident,op,literalStack);
        literalStack.clear();
        return rv;
    }

    void assign() 
    {
        if (literalStack.empty()) {
            //LOG4CPLUS_ERROR(m_logger,"Invalid assignment with no value for "+theItemName);
            throw "Invalid assignment with no value for "+theItemName;
            return;
        }
        cfg::Literal l(literalStack.back()); literalStack.pop_back();
        std::string item(theItemName);
        if (theItemIndex>=0) {
            char buf[20];
            snprintf(buf,20,"[%d]",theItemIndex);
            item+=buf;
        }
        theDoc->assignParameter(theNamespace,item,activeChain,l);
    }

    void iLiteral(wchar_t* data) 
    {
        literalStack.push_back(cfg::Literal::create(toString(data)));
        //    std::cout << literalStack.back() << std::endl;
    }

    void fLiteral(wchar_t* data) 
    {
        literalStack.push_back(cfg::Literal::create(toString(data)));
        //    std::cout << literalStack.back() << std::endl;
    }

    void bLiteral(bool b) 
    {
        literalStack.push_back(cfg::Literal(b));
        //    std::cout << literalStack.back() << std::endl;
    }

    void sLiteral(const wchar_t* data) 
    {
        literalStack.push_back(cfg::Literal::create(toString(data)));
        //    std::cout << literalStack.back() << std::endl;
    }

    cfg::Condition* newCondition() 
    {
        cfg::Condition* c=theDoc->addCondition();
        activeChain.push(c);
        return c;
    }

    void popCondition() 
    {
        activeChain.pop();
    }

    void setNamespace(const std::string& s) { theNamespace=s; }
  
    std::vector<cfg::Literal> literalStack;
    cfg::ConditionChain activeChain;
    cfg::Term* lastTerm;
    std::string theNamespace, theItemName;
    int theItemIndex;
    std::unique_ptr<cfg::CfgDocument> theDoc;

    static std::string toString(const wchar_t* d) 
    {
        using namespace hcalcfg;
        char* c=coco_string_create_char(d);
        std::string s(c);
        coco_string_delete(c);
        return s;
    }
    
    static int toInt(const wchar_t* d) 
    {
        using namespace hcalcfg;
        char* c=coco_string_create_char(d);
        int i=strtol(c,0,0);
        coco_string_delete(c);
        return i;
    }
};
