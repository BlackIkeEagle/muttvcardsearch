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
    CardCurler();
    QList<Person> curlCard(const QString &url, const QString &username, const QString &password, const QString &query);

private:

    struct vcdata {
      char *ptr;
      size_t len;
    };

    CURL *curl;
    CURLcode res;

    void init_vcard(struct vcdata *vc);
    static size_t writefunc(void *ptr, size_t size, size_t nmemb, struct vcdata *vc);

};

#endif // CARDCURLER_H
