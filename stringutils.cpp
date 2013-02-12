#include "stringutils.h"

vector<string> MVCS::StringUtils::split(const string &s, const string &token)
{
    // return this
    vector<string> result;

    // at pos begins the first char of the token...
    unsigned long pos = s.find(token, 0);

    // return original string if the token was not found
    if ( pos == std::string::npos ) {
        result.push_back(s);
        return result;
    }

    std::string tmp;
    while(pos != std::string::npos) {

        // first iteration
        if(tmp.length() == 0)
            tmp = s;

        result.push_back(tmp.substr(0, pos));

        tmp = tmp.substr(pos + token.length()); // fast forward to skip the token itself
        pos = tmp.find(token, 0);
    }

    return result;
}

bool MVCS::StringUtils::endsWith(const string &text, string suffix) {
    if( text.length() == 0 || suffix.length() == 0 )
        return false;

    string tmp = text.substr(text.length() - suffix.length());

    // transform both to lowercase for testing, and thus, ignores case
    std::transform(tmp.begin(), tmp.end(), tmp.begin(), ::tolower);
    std::transform(suffix.begin(), suffix.end(), suffix.begin(), ::tolower);

    if(tmp == suffix) return true;
    return false;
}

void MVCS::StringUtils::replace(string &text, const string &from, const string &to) {

}
