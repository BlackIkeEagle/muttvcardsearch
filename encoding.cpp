#include "encoding.h"

using namespace std;

Encoding::Encoding()
{
}

std::wstring Encoding::char2Wide(const char *chars) {
    setlocale(LC_ALL, "");

    // get the length of the string to convert
    int len = mbstowcs(NULL, chars, 0) + 1;

    wchar_t* result = new wchar_t[len];
    len = mbstowcs(result, chars, len);

    std::wstring s(result);
    return s;
}

std::string Encoding::wide2std(const std::wstring &str) {
    ostringstream stm;
    stm.imbue(std::locale("de_DE"));
    const ctype<char>& ctfacet = use_facet< ctype<char> >( stm.getloc() ) ;

    for( size_t i=0 ; i<str.size() ; ++i )
        stm << ctfacet.narrow( str[i], 0 ) ;

    return stm.str();
}

std::wstring Encoding::std2wide(const std::string &str) {
    wostringstream wstm;
    wstm.imbue(std::locale("de_DE.UTF-8"));
    const ctype<wchar_t>& ctfacet = use_facet< ctype<wchar_t> >( wstm.getloc() );

    for( size_t i=0 ; i<str.size() ; ++i )
        wstm << ctfacet.widen( str[i] ) ;

    return wstm.str() ;
}
