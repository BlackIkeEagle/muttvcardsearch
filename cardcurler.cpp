#include "cardcurler.h"

CardCurler::CardCurler(const QString &rawQuery)
{
    _rawQuery = rawQuery;
    _export = false;
}

void CardCurler::setExport() {
    _export = true;
}

void CardCurler::createPerson(const vCard *vcdata, Person *p) {
    vCardPropertyList vcPropertyList = vcdata->properties();
    foreach(vCardProperty vcProperty, vcPropertyList) {
        if(vcProperty.name() == VC_EMAIL) {
            p->Emails.append(vcProperty.values());
        } else if(vcProperty.name() == VC_NAME) {
            QStringList values = vcProperty.values();
            if (!values.isEmpty()) {
                QString fName = values.at(vCardProperty::Firstname);
                QString lName = values.at(vCardProperty::Lastname);
                if(!fName.isNull() && !fName.isEmpty() && lName.isNull() && !lName.isEmpty()) {
                    p->FirstName = fName;
                    p->LastName = lName;
                }
            }
        } else if(vcProperty.name() == VC_FORMATTED_NAME) {
            // not nice but there are entries in my carddav server
            // who dont have a N (i.e. VC_NAME property containing firstname and lastname separately)
            // but instead have only the formatted name property. So i will split this string
            // on each whitespace and combine it somewhat friendly... But what about a title?
            if(p->FirstName.isEmpty() || p->LastName.isEmpty()) {
                QString formattedName = vcProperty.values().at(0);
                if(formattedName.isNull() || formattedName.isEmpty()) {
                    cerr << "There is no vcard property at index 0. Therefore I cant read the value of formatted name (FN)." << endl;
                } else {
                    QStringList tokens = formattedName.split(QRegExp("\\s"));
                    if(!tokens.isEmpty() && tokens.size() == 2) {
                        p->FirstName = tokens.at(0);
                        p->LastName = tokens.at(1);
                    } else {
                        cerr << "VCard property 'FN' contains invalid value(s): more then 2 tokens or none at all!: " << formattedName.toStdString() << endl;
                    }
                }
            }
        }

        // remove addresses the user did probably not search for...
        // do this only when not exporting!
        if(false == _export) {
            QStringList emails = p->Emails;
            if(emails.size() > 1) {
                foreach(QString email, emails) {
                    if(!email.contains(_rawQuery)) {
                        p->Emails.removeOne(email);
                    }
                }
            }
        }
    }
}

bool CardCurler::listContainsQuery(const QStringList *list, const QString &query) {
    if(list->size() == 0) return false;
    for(int i=0; i<list->size(); i++) {
        if(list->at(i).contains(query)) {
            return true;
        }
    }

    return false;
}

QList<Person> CardCurler::curlCardCache(const QString &query) {
    QList<Person> persons;
    Settings cfg;
    QString cachefile = cfg.getCacheFile();

    if(QFile::exists(cachefile)) {
        QString line, vcard;

        QFile input(cachefile);
        if( input.open(QFile::ReadOnly | QFile::Text) ) {
            QTextStream in(&input);
            in.setCodec("UTF-8");

            line = in.readLine();
            vcard = line;
            vcard.append('\n');

            while (!line.isNull()) {
                line = in.readLine();
                vcard += line;
                vcard.append('\n');

                if(line == "END:VCARD") {
                    QByteArray utf8card;
                    utf8card.append(vcard.toUtf8());
                    QList<vCard> vcards = vCard::fromByteArray(utf8card);
                    vcard = ""; // reset!

                    if(!vcards.isEmpty()) {
                        foreach(vCard vc, vcards) {
                            Person p;
                            createPerson(&vc, &p);

                            if(p.LastName.contains(query, Qt::CaseInsensitive)  || p.FirstName.contains(query, Qt::CaseInsensitive) || listContainsQuery(&p.Emails, query)) {
                                persons.append(p);
                            }
                        }
                    }
                }
            }

            input.close();
        }
    }

    return persons;
}

QList<Person> CardCurler::curlCard(const QString &url, const QString &username, const QString &password, const QString &query) {

    // Result
    QList<Person> persons;

    // init_card breaks if it failes
    vcdata *vc = new vcdata();
    init_vcard(vc);

    // prepare the data structure from which curl reads the query to send to the peer
    vcdata *rd = new vcdata();
    QByteArray _ba = query.toAscii();
    char *data = _ba.data();
    int len = sizeof(data);
    len = len * query.length();
    rd->ptr = data;
    rd->len = len;

    CURL *curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if(curl) {
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Depth: 1");
        headers = curl_slist_append(headers, "Content-Type: text/xml; charset=utf-8");

        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_URL, url.toStdString().c_str());
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
        curl_easy_setopt(curl, CURLOPT_USERPWD, (QString("%1:%2").arg(username).arg(password)).toStdString().c_str());
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "REPORT");

        curl_easy_setopt(curl, CURLOPT_READDATA, rd);
        curl_easy_setopt(curl, CURLOPT_READFUNCTION, &CardCurler::readfunc);
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &CardCurler::writefunc);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, vc);

        res = curl_easy_perform(curl);
        if(res == CURLE_OK) {

//
//      for some reason this does not work at all, allthough the query is valid and evaluates properly; so I dropped it ;)
//
//            QString x_namespaced_query;
//            x_namespaced_query.append("declare namespace d=\"DAV\";\n");
//            x_namespaced_query.append("declare namespace card=\"urn:ietf:params:xml:ns:carddav\";\n");
//            x_namespaced_query.append("/d:multistatus/d:response/d:propstat/d:prop/card:address-data/string()");

//            QXmlQuery xmlquery;
//            QStringList xmlstringlist;
//            QString httpresult = QString::fromUtf8(vc->ptr);
//            xmlquery.setFocus(httpresult);
//            cout << vc->ptr << endl;
//            xmlquery.setQuery(x_namespaced_query);

//            if ( xmlquery.isValid() ) {
//               if( xmlquery.evaluateTo(&xmlstringlist) ) {
//                   cout << xmlstringlist.size() << endl;
//               }
//            }

            QString http_result = QString::fromLatin1(vc->ptr);
            QStringList list = http_result.split("<card:address-data>");
            if(list.size() > 0) {
                for(int i=1; i<list.size(); i++) {
                    QStringList _list = list.at(i).split("</card:address-data>");
                    // _list contains 2 elements where the first element is a single vcard
                    if(_list.size() == 2) {
                        QString s = _list.at(0);
                        s = s.replace("&#13;", "");

                        QList<vCard> vcards = vCard::fromByteArray(s.toStdString().c_str());
                        if(!vcards.isEmpty()) {
                            foreach(vCard c, vcards) {
                                // there is only one vcard in the list - every time ;)
                                Person p;
                                createPerson(&c, &p);

                                if(p.isValid()) {
                                    p.rawCardData = s;
                                    persons.append(p);
                                }
                            }
                        }
                    }
                }
            }
        }
        else
        {
            cout << "failed to get vcard data. code: " << res << endl;
        }

        curl_easy_cleanup(curl);
        curl_global_cleanup();
    }

    return persons;
}

void CardCurler::init_vcard(vcdata *vc) {
    vc->len = 0;
    vc->ptr = (char*)malloc(vc->len+1);
    if (vc->ptr == NULL) {
      fprintf(stderr, "malloc() failed\n");
      exit(EXIT_FAILURE);
    }
    vc->ptr[0] = '\0';
}

size_t CardCurler::writefunc(void *ptr, size_t size, size_t nmemb, vcdata *vc) {
    size_t new_len = vc->len + size * nmemb;
    vc->ptr = (char*)realloc(vc->ptr, new_len+1);

    if (vc->ptr == NULL) {
        fprintf(stderr, "realloc() failed\n");
        exit(EXIT_FAILURE);
    }

    memcpy(vc->ptr + vc->len, ptr, size*nmemb);
    vc->ptr[new_len] = '\0';
    vc->len = new_len;

    return size * nmemb;
}

size_t CardCurler::readfunc(void *ptr, size_t size, size_t nmemb, report_data *userdata) {
    size_t curl_size = nmemb * size;
    size_t to_copy = (userdata->len < curl_size) ? userdata->len : curl_size;
    memcpy(ptr, userdata->data, to_copy);
    userdata->len -= to_copy;
    userdata->data += to_copy;
    return to_copy;
}
