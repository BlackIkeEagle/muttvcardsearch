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
#include <QStringList>
#include <QTextStream>
#include <QDir>
#include <vcard/vcard.h>
#include "person.h"
#include "settings.h"

class CardCurler
{
public:
    CardCurler(const QString &rawQuery);
    QList<Person> curlCard(const QString &url, const QString &username, const QString &password, const QString &query);
    QList<Person> curlCardCache(const QString &query); // query should be the raw query string as we dont query the server
    void setExport();

private:

    struct vcdata {
      char *ptr;
      size_t len;
    };

    typedef struct
    {
        void *data;
        int body_size;
        int body_pos;
    } postdata;

    CURL *curl;
    CURLcode res;

    QString _rawQuery;
    bool _export;

    void fixHtml(QString* data);
    void init_vcard(struct vcdata *vc);
    void createPerson(const vCard *vcdata, Person *p);
    bool listContainsQuery(const QStringList *list, const QString &query);
    static size_t writefunc(void *ptr, size_t size, size_t nmemb, struct vcdata *vc);
    static size_t readfunc(void *ptr, size_t size, size_t nmemb, void *stream);
};

#endif // CARDCURLER_H
