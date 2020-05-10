#ifndef CfgDocument_hh_included
#define CfgDocument_hh_included 1

#include <string>
#include <map>
#include <vector>
#include <memory>
#include "TopTagger/CfgParser/interface/Parameter.hh"
//#include "log4cplus/logger.h"

namespace cfg {

    class Record;

    class CfgDocument {
    public:
        CfgDocument();
        ~CfgDocument();

        static std::unique_ptr<CfgDocument> parseDocument(const std::string& text);

        void useRecord(Record*);

        Condition* addCondition();
        void assignParameter(const std::string& ns, const std::string& name, const ConditionChain& cc, const Literal& l);
        int         get(const std::string& name, const Context& cxt, int defl) const;
        double      get(const std::string& name, const Context& cxt, double defl) const;
        bool        get(const std::string& name, const Context& cxt, bool defl) const;
        std::string get(const std::string& name, const Context& cxt, const char* defl) const;
        std::string get(const std::string& name, const Context& cxt, const std::string& defl) const;
        int         get(const std::string& name, int index, const Context& cxt, int defl) const;
        double      get(const std::string& name, int index, const Context& cxt, double defl) const;
        bool        get(const std::string& name, int index, const Context& cxt, bool defl) const;
        std::string get(const std::string& name, int index, const Context& cxt, const char* defl) const;
        std::string get(const std::string& name, int index, const Context& cxt, const std::string& defl) const;

        // access internal values (as needed)
        typedef std::map<std::string, std::unique_ptr<Parameter>>::const_iterator param_itr;
        param_itr param_begin() const { return m_parameters.begin(); }
        param_itr param_end() const { return m_parameters.end(); }

        void postValueUsed(const std::string& name, const Context& cxt, int value);
        void postValueUsed(const std::string& name, const Context& cxt, const char* value);
        void postValueUsed(const std::string& name, const Context& cxt, const std::string& value);
        void postValueUsed(const std::string& name, const Context& cxt, bool value);
    private:      
        Literal get(const std::string& name, const Context& cxt, const Literal& defl) const;
        std::string makeKey(const std::string& ns, const std::string& name) const;
        std::map<std::string, std::unique_ptr<Parameter>> m_parameters;
        std::vector<Condition*> m_conditions; // owner of all conditions
        Record* m_recordPtr; // not to be deleted!
        //log4cplus::Logger m_logger;
    };

}

#endif // CfgDocument_hh_included
