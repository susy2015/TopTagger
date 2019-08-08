#include "TopTagger/CfgParser/include/TTException.h"

#include <iostream>

TTException::TTException(const int line, const std::string function, const std::string file, const std::string message) : line_(line), funcion_(function), file_(file), message_(message)
{
}

std::string TTException::getPrintMessage() const
{
    return file_ + ":" + std::to_string(line_) + ", in function \"" + funcion_ + "\" -- " + message_;
}

void TTException::print() const
{
    std::cout << getPrintMessage() << std::endl;
}

std::ostream& operator<<(std::ostream& out, const TTException& e)
{
    return out << e.getPrintMessage();
}
