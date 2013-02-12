#include "fileutils.h"

FileUtils::FileUtils()
{
}

bool FileUtils::fileExists(const string &file) {
    bool exists = false;

    std::ifstream f(file.c_str());
    if(f) {
        exists = true;
        f.close();
    }

    return exists;
}

bool FileUtils::fileRemove(const string &file) {
    if( remove ( file.c_str() ) != 0 )
        return false;

    return true;
}
