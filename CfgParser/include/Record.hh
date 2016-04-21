#ifndef Record_hh_included
#define Record_hh_included 1

#include "TopTagger/CfgParser/include/Context.hh"
#include <map>
#include <vector>
#include <set>

namespace cfg {

    class Record {
    public:
        typedef std::map<std::string, Literal> item_collection;
        typedef std::map<std::string, Literal>::const_iterator const_item_iterator;
        typedef std::map<std::string, Literal>::iterator item_iterator;

        ~Record();

        void record(const Context& cxt, const std::string& item, const Literal& val, bool isDefault=false);
      
        const_item_iterator item_begin(const Context& cxt) const;
        const_item_iterator item_end(const Context& cxt) const;
        bool wasDefault(const Context& cxt, const std::string& item) const;

        std::vector<Context> contexts() const;
      

    private:
      
        struct ContextRecord {
            ContextRecord(const Context& c) : cxt(c) { }
            Context cxt;
            item_collection items;
            std::set<std::string> itemsDefault;
        };
        std::vector<ContextRecord*> m_records;

        ContextRecord* find(const Context& cxt) const;
        item_collection emptyColl_;
    };

}

#endif // Record_hh_included
