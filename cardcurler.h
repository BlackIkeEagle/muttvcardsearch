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

#ifndef CARDCURLER_H
#define CARDCURLER_H

#include "vCard/vcard.h"
#include <iostream>
#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/stat.h>
#include <fcntl.h>
#include <vector>

//#include <vcard/vcard.h>
#include "person.h"
#include "settings.h"
#include "stringutils.h"

using namespace std;

class CardCurler
{
public:
    CardCurler(const std::string &username, const std::string &password, const std::string &url, const std::string &rawQuery);
    std::vector<Person> curlCard(const std::string &query);
    std::vector<Person> curlCache(const std::string &query); // query should be the raw query string as we dont query the server
    std::vector<Person> getAllCards(const std::string &server, const std::string &query);

private:

    // write-data, result
    struct vcdata {
      char *ptr;
      size_t len;
    };

    // a nice duplication
    struct MemoryStruct {
      char *memory;
      size_t size;
    };

    // read-data, query
    typedef struct
    {
        void *data;
        int body_size;
        int body_pos;
    } postdata;

    CURL *curl;
    CURLcode res;

    // if this becomes TRUE, createPerson will not
    // use _rawQuery to remove unwanted emails
    bool exportMode;

    // indicates SOGO if TRUE
    bool isSOGO;

    std::string _url;
    std::string _username;
    std::string _password;
    std::string _rawQuery;

    std::string get(const std::string& requestType, const std::string &query = std::string());
    std::vector< std::string > getvCardURLs(const std::string &query);
    void fixHtml(string *data);
    void init_vcard(struct vcdata *vc);
    void createPerson(const vCard *vcdata, Person *p);
    bool listContainsQuery(const std::vector<std::string> *list, const std::string &query);
    static size_t writefunc(void *ptr, size_t size, size_t nmemb, struct vcdata *vc);
    static size_t readfunc(void *ptr, size_t size, size_t nmemb, void *stream);
    static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp);
};

#endif // CARDCURLER_H
