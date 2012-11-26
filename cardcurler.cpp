#include "cardcurler.h"

CardCurler::CardCurler()
{

}

QList<Person> CardCurler::curlCard(const QString &url, const QString &username, const QString &password, const QString &query) {

    // Result
    QList<Person> persons;

    // init_card breaks if it failes
    vcdata *vc = new vcdata();
    init_vcard(vc);

    // open the query file
    FILE *fd;
    struct stat file_info;

    fd = fopen(query.toStdString().c_str(), "rb");
    if(fd)
    {
        if(fstat(fileno(fd), &file_info) != 0)
        {
            fprintf(stderr, "cant get query file size\n");
        }
        else
        {
            CURL *curl;
            CURLcode res;

            curl_global_init(CURL_GLOBAL_DEFAULT);
            curl = curl_easy_init();

            if(curl)
            {
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

                curl_easy_setopt(curl, CURLOPT_READDATA, fd);
                curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)file_info.st_size);
                curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
                curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &CardCurler::writefunc);
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, vc);

                res = curl_easy_perform(curl);
                if(res == CURLE_OK)
                {
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

                                    vCardProperty name_prop = c.property(VC_NAME);
                                    QStringList values = name_prop.values();
                                    if (!values.isEmpty())
                                    {
                                        p.FirstName = values.at(vCardProperty::Firstname);
                                        p.LastName  = values.at(vCardProperty::Lastname);
                                    }

                                    vCardProperty mail_prop = c.property(VC_EMAIL);
                                    if(mail_prop.isValid()) {
                                       p.Emails.append(mail_prop.values());
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
            }
        }

        curl_global_cleanup();
        fclose(fd);
    }
    else
    {
        cout << "cant open query file " << query.toStdString() << endl;
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
