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
#include <fstream>

#include "settings.h"
#include "person.h"
#include "fileutils.h"

class Cache
{
public:
    Cache();
    ~Cache();

    bool openDatabase();
    bool createDatabase();
    std::vector<Person> findInCache(const std::string &query);
    void addVCard(const std::string& fn, const std::string& ln, const std::vector< std::string > &emails, const std::string& data, const std::string& updatedAt);

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

    std::string buildDateTimeString(const std::string& dtString);
    std::string toNarrow(const std::string& text);
    std::string toWide(const std::string& text);
};

#endif // CACHE_H
