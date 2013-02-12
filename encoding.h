#ifndef ENCODING_H
#define ENCODING_H

#include <stdlib.h>
#include <cstring>
#include <locale>
#include <iostream>
#include <string>
#include <sstream>
#include <string>

class Encoding
{
public:
    Encoding();
    static std::wstring char2Wide(const char* chars);
    static std::string wide2std(const std::wstring &str);
    static std::wstring std2wide(const std::string &str);
};

#endif // ENCODING_H
