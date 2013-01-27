#ifndef CARDCURLER_H
#define CARDCURLER_H

#include <iostream>
using namespace std;

#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <QString>
#include <pugixml.hpp>
#include <pugiconfig.hpp>
#include <vcard/vcard.h>
#include "person.h"

class CardCurler
{
public:
    CardCurler(const QString &rawQuery);
    QList<Person> curlCard(const QString &url, const QString &username, const QString &password, const QString &query);

private:

    struct vcdata {
      char *ptr;
      size_t len;
    };

    struct report_data {
        char *data;
        size_t len;
    };

    CURL *curl;
    CURLcode res;
    QString _rawQuery;

    void init_vcard(struct vcdata *vc);
    static size_t writefunc(void *ptr, size_t size, size_t nmemb, struct vcdata *vc);
    static size_t readfunc(void *ptr, size_t size, size_t nmemb, struct report_data *userdata);
};

#endif // CARDCURLER_H
