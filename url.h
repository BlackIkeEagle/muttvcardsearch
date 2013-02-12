// encoding: ünicode
#ifndef URL_H
#define URL_H

#include <string>

class Url
{
public:
    Url();
    static std::string removePath(const std::string& url);
};

#endif // URL_H
