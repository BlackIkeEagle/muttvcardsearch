#ifndef FILEUTILS_H
#define FILEUTILS_H

#include <string>
#include <fstream>
#include <stdio.h>

using namespace std;

class FileUtils
{
public:
    FileUtils();
    static bool fileExists(const std::string& file);
    static bool fileRemove(const std::string& file);
};

#endif // FILEUTILS_H
