/***************************************************************************
 *   Copyright (C) 2013 by Torsten Flammiger                               *
 *   github@netfg.net                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

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
