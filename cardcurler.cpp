#include "cardcurler.h"

CardCurler::CardCurler(const QString &rawQuery)
{
    _rawQuery = rawQuery;
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
            pugi::xml_document doc;
            pugi::xml_parse_result result = doc.load_buffer_inplace(vc->ptr, vc->len);
            if(result) {
                pugi::xpath_node_set tools = doc.select_nodes("d:multistatus/d:response/d:propstat/d:prop/card:address-data");
                for (pugi::xpath_node_set::const_iterator it = tools.begin(); it != tools.end(); ++it)
                {
                   pugi::xpath_node node = *it;
                   //cout << node.node().text().get() << endl;
                   QString data(node.node().text().get());
                   QList<vCard> vcards = vCard::fromByteArray(data.toStdString().c_str());
                   if(!vcards.isEmpty()) {
                       foreach(vCard c, vcards) {
                           Person p;
                           vCardPropertyList vcPropertyList = c.properties();
                           foreach(vCardProperty vcProperty, vcPropertyList) {
                               if(vcProperty.name() == VC_EMAIL) {
                                   p.Emails.append(vcProperty.values());
                               } else if(vcProperty.name() == VC_NAME) {
                                   QStringList values = vcProperty.values();
                                   if (!values.isEmpty())
                                   {
                                       QString fName = values.at(vCardProperty::Firstname);
                                       QString lName = values.at(vCardProperty::Lastname);
                                       if(!fName.isNull() && !fName.isEmpty() && lName.isNull() && !lName.isEmpty()) {
                                           p.FirstName = fName;
                                           p.LastName = lName;
                                       }
                                   }
                               } else if(vcProperty.name() == VC_FORMATTED_NAME) {
                                   // not nice but there are entries in my carddav server
                                   // who dont have a N (i.e. VC_NAME property containing firstname and lastname separately)
                                   // but instead have only the formatted name property. So i will split this string
                                   // on each whitespace and combine it somewhat friendly... But what about a title?
                                   if(p.FirstName.isEmpty() || p.LastName.isEmpty()) {
                                       QString formattedName = vcProperty.values().at(0);
                                       if(formattedName.isNull() || formattedName.isEmpty()) {
                                           cerr << "There is no vcard property at index 0. Therefore I cant read the value of formatted name (FN)." << endl;
                                       } else {
                                           QStringList tokens = formattedName.split(QRegExp("\\s"));
                                           if(!tokens.isEmpty() && tokens.size() == 2) {
                                               p.FirstName = tokens.at(0);
                                               p.LastName = tokens.at(1);
                                           } else {
                                               cerr << "VCard property 'FN' contains invalid value(s): more then 2 tokens or none at all!: " << formattedName.toStdString() << endl;
                                           }
                                       }
                                   }
                               }
                           }
                           // remove addresses the user did probably not search for...
                           QStringList emails = p.Emails;
                           if(emails.size() > 1) {
                               foreach(QString email, emails) {
                                   if(!email.contains(_rawQuery)) {
                                       p.Emails.removeOne(email);
                                   }
                               }
                           }
                           persons.append(p);
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
