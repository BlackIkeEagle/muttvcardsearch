#ifndef CACHE_H
#define CACHE_H

#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <iostream>
#include <sstream>
#include <string>
#include <clocale>
#include <locale>
#include <vector>

#include "settings.h"
#include "unicode.h"

class Cache
{
public:
    Cache();
    ~Cache();

    bool openDatabase();
    bool createDatabase();
    void addVCard(const std::wstring& fn, const std::wstring& ln, const std::vector< std::string > &emails, const std::wstring& data, const std::wstring& updatedAt);

private:
    Settings cfg;
    sqlite3* db;
    sqlite3_stmt *stmt;
    std::string cache_file;

    bool initSqlite();
    bool prepSqlite(const std::string &query);
    bool stepSqlite(const std::string &errMsg);
    bool finalizeSqlite();

    void addEmails(const std::vector< std::string > &emails, int rowID);

    std::wstring buildDateTimeString(const std::wstring& dtString);
    std::string toNarrow(const std::wstring& text);
};

#endif // CACHE_H
