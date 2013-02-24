#ifndef STRUTILS_H
#define STRUTILS_H

#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>

class StrUtils
{
public:
    StrUtils();
    static std::string simplify(const std::string& input);
    static std::string trim(std::string& input);
    static std::string ltrim(std::string& input);
    static std::string rtrim(std::string& input);
    static std::string join(const std::vector< std::string >* values, const std::string& token);
    static std::string join(const std::vector< std::string >* values, const char token);

    static void split(std::vector<std::string>* result, const std::string& text, const std::string& token);
    static void split(std::vector<std::string>* result, const std::string& text, const char token);

    static std::pair<std::string, std::string> simpleSplit(const std::string& text, const char token);
    static std::pair<std::string, std::string> simpleSplit(const std::string& text, const std::string& token);

    static bool startWith(const std::string& text, const std::string& prefix);
};

#endif // STRUTILS_H
