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
    CardCurler(const QString &username, const QString &password, const QString &url, const QString &rawQuery);
    QList<Person> curlCard(const QString &query);
    QList<Person> curlCache(const QString &query); // query should be the raw query string as we dont query the server
    QList<Person> getAllCards(const QString& server, const QString &query);

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

    QString _url;
    QString _username;
    QString _password;
    QString _rawQuery;

    QString getVCards(const QStringList& urls);
    QString get(const QString& requestType, const QString& query = QString());
    QStringList getvCardURLs(const QString &query);
    void fixHtml(QString* data);
    void init_vcard(struct vcdata *vc);
    void createPerson(const vCard *vcdata, Person *p);
    bool listContainsQuery(const QStringList *list, const QString &query);
    static size_t writefunc(void *ptr, size_t size, size_t nmemb, struct vcdata *vc);
    static size_t readfunc(void *ptr, size_t size, size_t nmemb, void *stream);
    static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp);
};

#endif // CARDCURLER_H
