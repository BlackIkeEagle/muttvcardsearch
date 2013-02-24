#include "strutils.h"

StrUtils::StrUtils()
{
}

void StrUtils::split(std::vector<std::string> *result, const std::string &text, char token) {
    // at pos begins the first char of the token...
    unsigned long pos = text.find(token, 0);

    // return original string if the token was not found
    if ( pos == std::string::npos ) {
        result->push_back(text);
        return;
    }

    std::string tmp;
    while(pos != std::string::npos) {

        // first iteration
        if(tmp.length() == 0)
            tmp = text;

        result->push_back(tmp.substr(0, pos));

        tmp = tmp.substr(pos + 1); // fast forward to skip the token itself
        pos = tmp.find(token, 0);

        // only one match found?
        if(pos == std::string::npos) {
            result->push_back(tmp);
        }
    }
}

void StrUtils::split(std::vector<std::string> *result, const std::string &text, const std::string &token) {
    // at pos begins the first char of the token...
    unsigned long pos = text.find(token, 0);

    // return original string if the token was not found
    if ( pos == std::string::npos ) {
        result->push_back(text);
        return;
    }

    std::string tmp;
    while(pos != std::string::npos) {

        // first iteration
        if(tmp.length() == 0)
            tmp = text;

        result->push_back(tmp.substr(0, pos));

        tmp = tmp.substr(pos + token.length()); // fast forward to skip the token itself
        pos = tmp.find(token, 0);

        // only one match found?
        if(pos == std::string::npos) {
            result->push_back(tmp);
        }
    }
}

bool StrUtils::startWith(const std::string &text, const std::string &prefix) {
    if( text.length() == 0 || prefix.length() == 0 )
        return false;

    if( prefix.length() > text.length() )
        return false;

    std::string tmp = text.substr(0, prefix.length());
    if(tmp == prefix) return true;
    return false;
}

// remove all multiple occurence of whitespace in the string with one space. i.e. ' '
std::string StrUtils::simplify(const std::string &input) {
    std::string result;
    int numFound = 0;

    for(std::string::size_type i = 0; i < input.size(); ++i) {
        switch(input[i]) {
        case ' ':
        case '\t':
        case '\r':
            numFound++;
            if(numFound == 1) result.push_back(' ');
            break;
        default:
            result.push_back(input[i]);
            numFound = 0;
        }
    }
    return result;
}

std::string StrUtils::trim(std::string &input) {
    std::string s(StrUtils::ltrim(input));
    return StrUtils::rtrim(s);
}

std::string StrUtils::ltrim(std::string &input) {
    input.erase(input.begin(), std::find_if(input.begin(), input.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
    return input;
}

std::string StrUtils::rtrim(std::string &input) {
    input.erase(std::find_if(input.rbegin(), input.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), input.end());
    return input;
}

std::string StrUtils::join(const std::vector<std::string> *values, const char token) {
    std::stringstream ss;
    for (std::vector<std::string>::const_iterator it = values->begin() ; it != values->end(); ++it) {
        ss << *it << token;
    }

    std::string result(ss.str());
    return result.substr(0, result.size()-1);
}

std::string StrUtils::join(const std::vector<std::string> *values, const std::string &token) {
    std::stringstream ss;
    for (std::vector<std::string>::const_iterator it = values->begin() ; it != values->end(); ++it) {
        ss << *it << token;
    }

    std::string result(ss.str());
    return result.substr(0, result.size() - token.size());
}

std::pair<std::string, std::string> StrUtils::simpleSplit(const std::string& text, const char token) {
    unsigned long pos = text.find(token, 0);
    return std::make_pair<std::string, std::string>(text.substr(0, pos), text.substr(pos+1));
}

std::pair<std::string, std::string> StrUtils::simpleSplit(const std::string& text, const std::string& token) {
    unsigned long pos = text.find(token, 0);
    return std::make_pair<std::string, std::string>(text.substr(0, pos), text.substr(pos+token.length()));
}
