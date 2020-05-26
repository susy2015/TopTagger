#include "TopTagger/CfgParser/interface/Record.hh"

namespace cfg {

    Record::~Record() {
        for (std::vector<ContextRecord*>::iterator i=m_records.begin(); i!=m_records.end(); i++)
            delete *i;
    }

    Record::ContextRecord* Record::find(const Context& cxt) const {
        for (std::vector<ContextRecord*>::const_iterator i=m_records.begin(); i!=m_records.end(); i++) {
            if ((*i)->cxt==cxt) return *i;
        }
        return 0;
    }

    void Record::record(const Context& cxt, const std::string& item, const Literal& val, bool isDefault) {
        ContextRecord* rec=find(cxt);
        if (rec==0) {
            rec=new ContextRecord(cxt);
            m_records.push_back(rec);
        }
      
        if (rec->items.find(item)==rec->items.end()) {
            rec->items.insert(std::pair<std::string,Literal>(item,val));
        } else {
            rec->items[item]=val;
        }
        if (isDefault) {
            rec->itemsDefault.insert(item);
        } else if (rec->itemsDefault.find(item)!=rec->itemsDefault.end()) {
            rec->itemsDefault.erase(item); 
        }
    }


    Record::const_item_iterator Record::item_begin(const Context& cxt) const {
        ContextRecord* rec=find(cxt);
        if (rec!=0) return rec->items.begin();
        else return emptyColl_.end();
    }
    Record::const_item_iterator Record::item_end(const Context& cxt) const {
        ContextRecord* rec=find(cxt);
        if (rec!=0) return rec->items.end();
        else return emptyColl_.end();
    }
    bool Record::wasDefault(const Context& cxt, const std::string& item) const {
        ContextRecord* rec=find(cxt);
        if (rec==0) return false;
        return rec->itemsDefault.find(item)!=rec->itemsDefault.end();
    }
    
    std::vector<Context> Record::contexts() const {
        std::vector<Context> retval;
        for (std::vector<ContextRecord*>::const_iterator i=m_records.begin(); i!=m_records.end(); i++) {
            retval.push_back((*i)->cxt);
        }
        return retval;
    }

}
