#ifndef TTMFACTORY_H
#define TTMFACTORY_H

#include <functional>
#include <string>
#include <map>

class TTModule;

//This class is only ever intended to be used through static calls!

class TTMFactory
{
public:

    // Get the static instance of the factory (or make it if it does not exist)
    static TTMFactory* getInstance();

    // register a new module in the map, this is called automatically by the magic macro
    static bool registerModule(const std::string&, std::function<TTModule*()>);

    // checks if a module with the requested name exists in the map
    static bool moduleExists(const std::string&);

    // Called to create an instance of a module by name
    static TTModule* createModule(const std::string&);

private:

    //internal pointer to the object
    static TTMFactory* instance_;

    //map to store module name and corrosponding factory function 
    std::map<std::string, std::function<TTModule*()>> creators_;

};

#endif
