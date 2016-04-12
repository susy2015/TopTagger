#ifndef Context_hh_included
#define Context_hh_included 1

#include "TopTagger/TopTagger/include/hcal/cfg/Language.hh"
#include <map>

namespace hcal {
  namespace cfg {

    class Context {
    public:
      Context(const std::string& namesp);
      Context(const Context& cxt);
      Context& operator=(const Context& cxt);

      /// get the namespace
      std::string ns() const { return m_namespace; }

      /// set a feature of the namespace (string)
      void set(const std::string& name, const std::string& value);
      /// set a feature of the namespace (string)
      void set(const std::string& name, const char* value);
      /// set a feature of the namespace (integer)
      void set(const std::string& name, int value);
      /// set a feature of the namespace (boolean)
      void set(const std::string& name, bool value);
      /// unset a feature of the namespace
      void unset(const std::string& name);      

      /// does this feature exist?
      bool exists(const std::string& name) const;
      /// access the requested feature
      const Literal& get(const std::string& name) const;
      /// equality test
      bool operator==(const Context& c) const;

      typedef std::map<std::string,Literal>::const_iterator const_feature_iterator;

      /// iterate over features
      const_feature_iterator feature_begin() const { return m_features.begin(); }
      /// iterate over features
      const_feature_iterator feature_end() const { return m_features.end(); }

    private:
      std::string m_namespace;
      std::map<std::string,Literal> m_features;      
    };

  }
}


#endif // Context_hh_included
