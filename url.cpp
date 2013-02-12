// encoding: ünicode
#include "url.h"

Url::Url()
{
}

std::string Url::removePath(const std::string &url) {
    int pos = url.find("//");
    std::string scheme = url.substr(0, pos-1);

    std::string hostpart = url.substr(pos+2);
    pos = hostpart.find("/");
    hostpart = hostpart.substr(0, pos);

    std::string result = scheme.append("://").append(hostpart);
    return result;
}
