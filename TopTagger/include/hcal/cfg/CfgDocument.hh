#ifndef CfgDocument_hh_included
#define CfgDocument_hh_included 1

#include <string>
#include <map>
#include <vector>
#include "TopTagger/TopTagger/include/hcal/cfg/Parameter.hh"
//#include "log4cplus/logger.h"

namespace hcal {
  namespace cfg {

    class Record;

    class CfgDocument {
    public:
      CfgDocument();
      ~CfgDocument();

      static std::auto_ptr<CfgDocument> parseDocument(const std::string& text);

      void useRecord(Record*);

      Condition* addCondition();
      void assignParameter(const std::string& ns, const std::string& name, const ConditionChain& cc, const Literal& l);
      int get(const std::string& name, const Context& cxt, int defl);
      bool get(const std::string& name, const Context& cxt, bool defl);
      std::string get(const std::string& name, const Context& cxt, const char* defl);
      std::string get(const std::string& name, const Context& cxt, const std::string& defl);
      int get(const std::string& name, int index, const Context& cxt, int defl);
      bool get(const std::string& name, int index, const Context& cxt, bool defl);
      std::string get(const std::string& name, int index, const Context& cxt, const char* defl);
      std::string get(const std::string& name, int index, const Context& cxt, const std::string& defl);

      // access internal values (as needed)
      typedef std::map<std::string, Parameter*>::const_iterator param_itr;
      param_itr param_begin() const { return m_parameters.begin(); }
      param_itr param_end() const { return m_parameters.end(); }

      void postValueUsed(const std::string& name, const Context& cxt, int value);
      void postValueUsed(const std::string& name, const Context& cxt, const char* value);
      void postValueUsed(const std::string& name, const Context& cxt, const std::string& value);
      void postValueUsed(const std::string& name, const Context& cxt, bool value);
    private:      
      Literal get(const std::string& name, const Context& cxt, const Literal& defl);
      std::string makeKey(const std::string& ns, const std::string& name) const;
      std::map<std::string, Parameter*> m_parameters;
      std::vector<Condition*> m_conditions; // owner of all conditions
      Record* m_recordPtr; // not to be deleted!
        //log4cplus::Logger m_logger;
    };

  }
}

#endif // CfgDocument_hh_included
