#ifndef Language_hh_included
#define Language_hh_included 1

#include <string>
#include <ostream>

namespace cfg {

    std::string capitalize(const std::string& str);

    class Literal { 
    public:
        enum Flavor { fl_Null, fl_String, fl_Boolean, fl_Integer, fl_Float };
        Literal();
        Literal(const std::string& val);
        Literal(const char* val);
        Literal(const bool& val);
        Literal(const int& val);
        Literal(const double& val);
      
        int intValue() const;
        double floatValue() const;
        bool boolValue() const;
        std::string strValue() const;

        bool operator==(const Literal& l) const;
        bool operator!=(const Literal& l) const;
        bool operator>=(const Literal& l) const;
        bool operator>(const Literal& l) const;
        bool operator<=(const Literal& l) const;
        bool operator<(const Literal& l) const;
	  
        std::ostream& put(std::ostream& s) const;

        static Literal create(const std::string& text);
        Flavor flavor() const { return m_flavor; }
    private:
        Flavor m_flavor;
        struct {
            int i;
            double f;
            std::string s;
            bool b;
        } m_value;
    };

}

std::ostream& operator<<(std::ostream& s, const cfg::Literal& l);

#endif // Language_hh_included
