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

#include "cache.h"

Cache::Cache()
{
    cache_file = cfg.getCacheFile();
    db = NULL;
}

Cache::~Cache() {
    if (db) {
        int retVal = sqlite3_close(db);
        if( SQLITE_OK != retVal ) {
            std::cerr << "Oops: failed to properly close database: " << sqlite3_errmsg(db) << std::endl;
        }
    }
}

bool Cache::finalizeSqlite() {
    int retVal = sqlite3_finalize(stmt);
    if(SQLITE_OK != retVal) {
        std::cerr << "Unable to finalize: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    return true;
}

bool Cache::stepSqlite(const std::string &errMsg) {
    int retVal = sqlite3_step(stmt);
    if(SQLITE_DONE != retVal) {
        std::cerr << errMsg << ": " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    return true;
}

bool Cache::prepSqlite(const std::string &query) {
    int retVal = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);
    if(SQLITE_OK != retVal) {
        std::cerr << "Failed to prepare statement: " << query << " - Message:" << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    return true;
}

std::vector<Person> Cache::findInCache(const std::string &query) {
    std::vector< Person > result;

    if(false == openDatabase())
        return result;

    prepSqlite(query);

    bool done = false;
    while(!done) {
        switch ( sqlite3_step( stmt ) ) {
        case SQLITE_ROW:
            {
                //std::string fn((const wchar_t*)sqlite3_column_text(stmt, 0));
                std::string fn    = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
                std::string ln    = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
                std::string email = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)));

                Person p;
                p.LastName = ln;
                p.FirstName = fn;
                p.Emails.push_back(email);

                result.push_back(p);
            }
            break;
        case SQLITE_DONE:
            done = true;
            break;
        default:
            throw("Unable to execute query");
            break;
        }
    }

    finalizeSqlite();
    return result;
}

std::string Cache::buildDateTimeString(const std::string &dtString) {
    std::string result = dtString;
    int pos = result.find('+');

    if(pos > 0) {
        result = result.substr(0, pos);
    }

    pos = result.find('T');
    std::string date = result.substr(0, pos);
    std::string time = result.substr(pos+1);

    if(date.length() != 10) {
        // hmm, invalid! what to to?
        date = "1971-01-01";
    }

    if(time.length() != 8) {
        time = "00:00:00";
    }

    result = date + "T" + time;
    return result;
}

bool Cache::initSqlite() {
    int retVal = sqlite3_initialize();
    if(SQLITE_OK != retVal) {
        std::cerr << "Can't initialize sqlite3: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    return true;
}

// explicit open - necessary on update or select (search)
bool Cache::openDatabase() {
    // allready open?
    if(db) return true;

    if(false == initSqlite())
        return false;

    int retVal = sqlite3_open_v2(cache_file.c_str(), &db, SQLITE_OPEN_READWRITE, NULL);
    if(SQLITE_OK != retVal) {
        std::cerr << "Can't open/create database (RW) in " << cache_file << ": " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    return true;
}

void Cache::addEmails(const std::vector<std::string> &emails, int rowID) {
    for(unsigned int i=0; i<emails.size(); i++) {
        std::string email = emails.at(i);

        std::stringstream ss;
        ss << "insert into emails (vcardid, mail) values(";
        ss << rowID;
        ss << ", '" << email << "')";

        prepSqlite(ss.str());
        stepSqlite("Failed to add email to database");
        finalizeSqlite();
    }
}

void Cache::addVCard(const std::string &fn, const std::string &ln, const std::vector< std::string > &emails, const std::string &data, const std::string &updatedAt) {
    if(fn.length() == 0) {
        std::cerr << "Firstname is empty!" << std::endl;
        return;
    }

    if(ln.length() == 0) {
        std::cerr << "Lastname is empty!" << std::endl;
        return;
    }

    if(emails.size() == 0) {
        std::cerr << "Email is empty!" << std::endl;
        return;
    }

    if(data.length() == 0) {
        std::cerr << "Data is empty!" << std::endl;
        return;
    }

    if(db == NULL) {
        std::cerr << "Database not open!" << std::endl;
        return;
    }

    std::string dt = buildDateTimeString(updatedAt);
    std::stringstream ss;

    // happy sql injecting ;)
    ss << "insert into vcards (FirstName, LastName, VCard, UpdatedAt) values (";
    ss << "'" << fn << "','" << ln << "','" << data << "','" << dt << "')";

    bool b = prepSqlite(ss.str());
    if(b) {
        b = stepSqlite("Failed to add new record to cache database");
        if(b) {
            sqlite3_int64 rowid = sqlite3_last_insert_rowid(db);
            addEmails(emails, rowid);
        } else {
            finalizeSqlite(); // finalize on error only
        }
    }
}

bool Cache::createDatabase() {

    // test if file exists
    if(FileUtils::fileExists(cache_file)) {
        // stdio standard function
        if( FileUtils::fileRemove(cache_file) ) {
            std::cout << "Old cache database removed" << std::endl;
        } else {
            std::cerr << "Failed to remove old cache database '" << cache_file << "'" << std::endl;
            return false;
        }
    }

    if(false == initSqlite())
        return false;

    int retVal = sqlite3_open_v2(cache_file.c_str(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
    if(SQLITE_OK != retVal) {
        std::cerr << "Can't open/create database in " << cache_file << ": " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    // create the main table
    bool b = prepSqlite("CREATE TABLE vcards(VCardID INTEGER PRIMARY KEY, FirstName STRING, LastName STRING, VCard TEXT, UpdatedAt STRING)");
    if(false == b) return b;
    b = stepSqlite("Can't step to create table 'vcards' in cache database");
    if(false == b) return b;
    b = finalizeSqlite();
    if(false == b) return b;

    // create the mail table
    b = prepSqlite("CREATE TABLE emails(ID INTEGER PRIMARY KEY, VCardID INTEGER, Mail STRING)");
    if(false == b) return b;
    b = stepSqlite("Can't step to create table 'emails' in cache database");
    if(false == b) return b;
    b = finalizeSqlite();
    if(false == b) return b;

    // index on raw vcard data
    b = prepSqlite("CREATE INDEX vcard_idx ON vcards (vcard)");
    if(false == b) return b;
    b = stepSqlite("Can't step on index for table 'vcards', column 'vcard'");
    if(false == b) return b;
    b = finalizeSqlite();
    if(false == b) return b;

    // index on email data
    b = prepSqlite("CREATE INDEX email_idx ON emails (mail)");
    if(false == b) return b;
    b = stepSqlite("Can't step on index for table 'emails', column 'mail'");
    if(false == b) return b;
    b = finalizeSqlite();
    if(false == b) return b;

    // index on first name
    b = prepSqlite("CREATE INDEX firstname_idx ON vcards (firstname)");
    if(false == b) return b;
    b = stepSqlite("Can't step on index for table 'vcards', column 'firstname'");
    if(false == b) return b;
    b = finalizeSqlite();
    if(false == b) return b;

    // index on last name
    b = prepSqlite("CREATE INDEX lastname_idx ON vcards (lastname)");
    if(false == b) return b;
    b = stepSqlite("Can't step on index for table 'vcards', column 'lastname'");
    if(false == b) return b;
    b = finalizeSqlite();
    if(false == b) return b;

    // index on updatedat
    b = prepSqlite("CREATE INDEX updatedat_idx ON vcards (updatedat)");
    if(false == b) return b;
    b = stepSqlite("Can't step on index for table 'vcards', column 'updatedat'");
    if(false == b) return b;
    b = finalizeSqlite();
    if(false == b) return b;

    std::cout << "New cache database created in '" << cache_file << "'" << std::endl;
    return b;
}
