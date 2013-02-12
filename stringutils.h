#ifndef STRINGUTILS_H
#define STRINGUTILS_H

#include <string>
#include <vector>
#include <algorithm>

using namespace std;

class StringUtils
{
public:
    StringUtils();
    static vector<string> split(const string& s, const string& token);
    static bool endsWith(const string& text, const string& suffix);
};

#endif // STRINGUTILS_H
