#ifndef STRINGUTILS_H
#define STRINGUTILS_H

#include <string>
#include <vector>
#include <algorithm>

using namespace std;

// M utt VC ard S earch ;)
namespace MVCS {

    class StringUtils
    {
    public:
        StringUtils();
        static vector<string> split(const string& s, const string& token);
        static bool endsWith(const string& text, const string& suffix);
        void replace(std::string& text, const std::string& from, const std::string& to);
    };

}

#endif // STRINGUTILS_H
