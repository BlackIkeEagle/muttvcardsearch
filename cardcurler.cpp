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

#include "cardcurler.h"
#include "option.h"
#include "cache.h"

/*
 * CTOR
 */
CardCurler::CardCurler(const std::string &username, const std::string &password, const std::string &url, const string &rawQuery)
{
    _url      = url;
    _username = username;
    _password = password;
    _rawQuery = rawQuery;

    exportMode = false;
    isSOGO = false;
}

/*
 * Private method: used to detect the URL's a carddav server has to offer
 * Returns a QStringList of all url's found by the XML query given
 *
 * @query : the xml snippet the carddav server expects to receive
 * @return: a QStringList of raw vcard strings (BEGIN:VCARD ... END:VCARD)
 */
std::vector<std::string> CardCurler::getvCardURLs(const std::string &query) {
    std::string s = get("PROPFIND", query);

    // check xml case D:href (SOGo) or d:href (owncloud)
    std::string href_begin = "<d:href>";
    std::string href_end = "</d:href>";
    if( s.find("D:href", 0) != std::string::npos ) {
        href_begin = "<D:href>";
        href_end   = "</D:href>";
        isSOGO = true;
    }

    std::vector<std::string> result;
    std::vector<std::string> tokens = MVCS::StringUtils::split(s, href_begin);

    if(tokens.size() >= 1) {
        for(unsigned int i=1; i<tokens.size(); i++) {
            std::vector<std::string> inner = MVCS::StringUtils::split(tokens.at(i), href_end);;
            if(inner.size() >= 1) {
                std::string url = inner.at(0);

                if(MVCS::StringUtils::endsWith(url, "vcf")) {
                    result.push_back(url);
                }
            }
        }
    }

    return result;
}

/*
 * This public method used to fetch all cards for a given account.
 *
 * First it will ask the carddav server for the url's and then
 * walk through each of the url's and download the vcard. If
 * possible it will not end the connection after each download.
 * It will cancel the action on any given failure given by libcurl
 * and return whatever vcards where read at the given moment.
 *
 * @server: a full qualified hostname with protocol spec, i.e. http(s)://www.johndoe.com
 * @query : the xml snippet the carddav server expects to receive
 *
 * @return: returns a Qlist of Person objects
 */
std::vector<Person> CardCurler::getAllCards(const std::string &server, const std::string &query) {
    std::vector<Person> persons;

    exportMode = true;

    CURL *curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    std::vector< std::string > cardUrls = getvCardURLs(query);

    if(curl && cardUrls.size() > 0) {

        // init memory chunk
        struct MemoryStruct chunk;;
        chunk.size = 0;
        chunk.memory = (char*)malloc(1);

        if(Option::isVerbose()) {
            curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        }

        std::string auth(_username + ":" + _password);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
        curl_easy_setopt(curl, CURLOPT_USERPWD, auth.c_str());
        curl_easy_setopt(curl, CURLOPT_HEADER, 0L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &CardCurler::WriteMemoryCallback); // directly from libcurl homepage
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

        //int j = 0;
        foreach(std::string url, cardUrls) {
            std::stringstream ss;
            ss << server << url;

            curl_easy_setopt(curl, CURLOPT_URL, ss.str().c_str());
            res = curl_easy_perform(curl);

            if(res != CURLE_OK) {
                std::cerr << "CardCurler::getVCard() failed on URL: "
                     << url
                     << ", Code: "
                     << curl_easy_strerror(res) << endl;

                break;
            }

            // get the result and reset the chunk
            if(chunk.memory) {
                std::string card(chunk.memory);

                free(chunk.memory);
                chunk.size = 0;
                chunk.memory = (char*)malloc(1); // reallocate after free!

                if(card.size() > 0) {
                    Person p;
                    QList<vCard> cards = vCard::fromByteArray(QString::fromStdString(card).toUtf8());

                    if(cards.size() == 1) {
                        createPerson(&cards[0], &p);
                        if(p.isValid()) {
                            std::cout << "fetched valid vcard from: " << server << url << endl;
                            p.rawCardData = card;
                            persons.push_back(p);
                            //j++;
                        }
                    }
                }
            }

            //if(j>12) break;
        }

        /* always cleanup */
        curl_easy_cleanup(curl);

        if(chunk.memory)
            free(chunk.memory);
    }

    curl_global_cleanup();
    return persons;
}

/*
 * Private method: parses a pointer to a vCard object and pushes
 * it's information to the Persons pointer.
 * Only Firstname, Lastname and a List of Emails are of interrest.
 *
 * @vcdata: a pointer to a vCard object
 * @p     : a pointer to a Person object
 *
 * @return: void()
 */
void CardCurler::createPerson(const vCard *vcdata, Person *p) {
    vCardPropertyList vcPropertyList = vcdata->properties();
    foreach(vCardProperty vcProperty, vcPropertyList) {
        if(vcProperty.name() == VC_EMAIL) {
            foreach(QString s, vcProperty.values()) {
                p->Emails.push_back(s.toStdString());
            }
        } else if(vcProperty.name() == VC_NAME) {
            QStringList values = vcProperty.values();
            if (!values.isEmpty()) {
                QString fName = values.at(vCardProperty::Firstname);
                QString lName = values.at(vCardProperty::Lastname);
                if(!fName.isNull() && !fName.isEmpty() && lName.isNull() && !lName.isEmpty()) {
                    p->FirstName = fName.toStdString();
                    p->LastName = lName.toStdString();
                }
            }
        } else if(vcProperty.name() == VC_FORMATTED_NAME) {
            // not nice but there are entries in my carddav server
            // who dont have a N (i.e. VC_NAME property containing firstname and lastname separately)
            // but instead have only the formatted name property. So i will split this string
            // on each whitespace and combine it somewhat friendly... But what about a title?
            if(p->FirstName.size() == 0 || p->LastName.size() == 0) {
                QString formattedName = vcProperty.values().at(0);
                if(formattedName.isNull() || formattedName.isEmpty()) {
                    cerr << "There is no vcard property at index 0. Therefore I cant read the value of formatted name (FN)." << endl;
                } else {
                    QStringList tokens = formattedName.split(QRegExp("\\s"));
                    if(!tokens.isEmpty() && tokens.size() == 2) {
                        p->FirstName = tokens.at(0).toStdString();
                        p->LastName = tokens.at(1).toStdString();
                    } else {
                        cerr << "VCard property 'FN' contains invalid value(s): more then 2 tokens or none at all!: " << formattedName.toStdString() << endl;
                    }
                }
            }
        } else if (vcProperty.name() == VC_REVISION) {
            // append updated_at to the person
            QStringList revs = vcProperty.values();
            if(!revs.empty()) {
                p->lastUpdatedAt = revs.at(0).toStdString();
            }
        }

        if(false == exportMode) {
            std::vector<std::string> emails = p->Emails;
            if(emails.size() > 1) {
                int counter = 0;
                foreach(std::string email, emails) {
                    unsigned long match = email.find(_rawQuery );
                    if( match != std::string::npos ) {
                        p->Emails.erase(p->Emails.begin() + counter);
                    }
                    counter++;
                }
            }
        }
    }
}

/*
 * Helper method to check if a given list of strings contains a string
 *
 * @list..: pointer to a QStringList to check
 * @query.: the QString to check for
 *
 * @return: TRUE if the list contains the query, FALSE otherwise
 */
bool CardCurler::listContainsQuery(const std::vector<string> *list, const string &query) {
    if(list->size() == 0) return false;
    for(unsigned int i=0; i<list->size(); i++) {
        if(MVCS::StringUtils::contains(list->at(i), query)) {
            return true;
        }
    }

    return false;
}

// get server resource using libcurl
std::string CardCurler::get(const string &requestType, const std::string& query) {
    std::string result;

    // init_card breaks if it failes
    vcdata *vc = new vcdata();
    init_vcard(vc);

    // prepare the data structure from which curl reads the query which is then send to the peer
    char *data = (char *)(query.c_str());

    // read-data (the query)
    postdata pdata;
    pdata.body_pos = 0;
    pdata.body_size = strlen(data);
    pdata.data = data;

    CURL *curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if(curl) {
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Depth: 1");
        headers = curl_slist_append(headers, "Content-Type: text/xml; charset=utf-8");

        std::string auth(_username + ":" + _password);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_URL, _url.c_str());
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
        curl_easy_setopt(curl, CURLOPT_USERPWD, auth.c_str());
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, requestType.c_str());

        curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)pdata.body_size);
        curl_easy_setopt(curl, CURLOPT_READDATA, &pdata);
        curl_easy_setopt(curl, CURLOPT_READFUNCTION, &CardCurler::readfunc);
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &CardCurler::writefunc);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, vc);

        if(Option::isVerbose()) {
            curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        } else {
            curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
        }

        res = curl_easy_perform(curl);
        if(res == CURLE_OK) {
            result = vc->ptr;
        }

        curl_easy_cleanup(curl);
    }

    if(vc->ptr) free(vc->ptr);
    curl_global_cleanup();

    return result;
}

std::vector<Person> CardCurler::curlCache(const std::string &query) {

    std::stringstream ss;
    ss << "select v.firstname, v.lastname, e.mail from vcards v, emails e ";
    ss << "where e.vcardid = v.vcardid and (lower(v.firstname) like '%"<< query << "%' ";
    ss << "or lower(v.firstname) like '%"<< query << "%' ";
    ss << "or lower(v.lastname) like '%"<< query << "%' ";
    ss << "or lower(e.mail) like '%"<< query << "%')";

    Cache cache;
    return cache.findInCache(ss.str());
}

// curl a card online
std::vector<Person> CardCurler::curlCard(const std::string &query) {

    // Result
    std::vector<Person> people;

    // execute the query!
    std::string http_result = get("REPORT", query);

    std::string vcardAddressBeginToken = "<card:address-data>"; // defaults to Owncloud
    std::string vcardAddressEndToken = "</card:address-data>";

    if(http_result.find("<C:address-data>") != std::string::npos) {
        vcardAddressBeginToken = "<C:address-data>";
        vcardAddressEndToken   = "</C:address-data>";
    }

    std::vector<std::string> list = MVCS::StringUtils::split(http_result, vcardAddressBeginToken);
    if(list.size() > 0) {
        for(unsigned int i=1; i<list.size(); i++) {

            if(Option::isVerbose()) {
                cout << list.at(i) << endl;
            }

            std::vector<std::string> _list = MVCS::StringUtils::split(list.at(i), vcardAddressEndToken);
            // _list contains 2 elements where the first element is a single vcard
            if(_list.size() == 2) {

                std::string s = _list.at(0);
                MVCS::StringUtils::replace(&s, "&#13;", "");

                if(isSOGO)
                    fixHtml(&s);

                QList<vCard> vcards = vCard::fromByteArray(QString::fromStdString(s).toUtf8());

                if(!vcards.isEmpty()) {
                    foreach(vCard c, vcards) {
                        // there is only one vcard in the list - every time ;)
                        Person p;
                        createPerson(&c, &p);

                        if(p.isValid()) {
                            p.rawCardData = s;
                            people.push_back(p);
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

    return people;
}

/*
 * This nice method tries to replace all html entities by its real char
 * It will fail sometime! - or from time to time ;)
 */
void CardCurler::fixHtml(string* data) {
    MVCS::StringUtils::replace(data, "&#13;", "");
    MVCS::StringUtils::replace(data, "&#34;", "\"");
    MVCS::StringUtils::replace(data, "&#38;", "&");
    MVCS::StringUtils::replace(data, "&#60;", "<");

    MVCS::StringUtils::replace(data, "&#62;", ">");
    MVCS::StringUtils::replace(data, "&#160;", " ");
    MVCS::StringUtils::replace(data, "&#161;", "¡");
    MVCS::StringUtils::replace(data, "&#162;", "¢");

    MVCS::StringUtils::replace(data, "&#163;", "£");
    MVCS::StringUtils::replace(data, "&#164;", "¤");
    MVCS::StringUtils::replace(data, "&#165;", "¥");
    MVCS::StringUtils::replace(data, "&#166;", "¦");

    MVCS::StringUtils::replace(data, "&#167;", "§");
    MVCS::StringUtils::replace(data, "&#168;", "¨");
    MVCS::StringUtils::replace(data, "&#169;", "©");
    MVCS::StringUtils::replace(data, "&#170;", "ª");
    MVCS::StringUtils::replace(data, "&#171;", "«");
    MVCS::StringUtils::replace(data, "&#172;", "¬");
    MVCS::StringUtils::replace(data, "&#173;", "");
    MVCS::StringUtils::replace(data, "&#174;", "®");
    MVCS::StringUtils::replace(data, "&#175;", "¯");
    MVCS::StringUtils::replace(data, "&#176;", "°");
    MVCS::StringUtils::replace(data, "&#177;", "±");
    MVCS::StringUtils::replace(data, "&#178;", "²");
    MVCS::StringUtils::replace(data, "&#179;", "³");
    MVCS::StringUtils::replace(data, "&#180;", "´");
    MVCS::StringUtils::replace(data, "&#181;", "µ");
    MVCS::StringUtils::replace(data, "&#182;", "¶");
    MVCS::StringUtils::replace(data, "&#183;", "·");
    MVCS::StringUtils::replace(data, "&#184;", "¸");
    MVCS::StringUtils::replace(data, "&#185;", "¹");
    MVCS::StringUtils::replace(data, "&#186;", "º");
    MVCS::StringUtils::replace(data, "&#187;", "»");
    MVCS::StringUtils::replace(data, "&#188;", "¼");
    MVCS::StringUtils::replace(data, "&#189;", "½");
    MVCS::StringUtils::replace(data, "&#190;", "¾");
    MVCS::StringUtils::replace(data, "&#191;", "¿");
    MVCS::StringUtils::replace(data, "&#192;", "À");
    MVCS::StringUtils::replace(data, "&#193;", "Á");
    MVCS::StringUtils::replace(data, "&#194;", "Â");
    MVCS::StringUtils::replace(data, "&#195;", "Ã");
    MVCS::StringUtils::replace(data, "&#196;", "Ä");
    MVCS::StringUtils::replace(data, "&#197;", "Å");
    MVCS::StringUtils::replace(data, "&#198;", "Æ");
    MVCS::StringUtils::replace(data, "&#199;", "Ç");
    MVCS::StringUtils::replace(data, "&#200;", "È");
    MVCS::StringUtils::replace(data, "&#201;", "É");
    MVCS::StringUtils::replace(data, "&#202;", "Ê");
    MVCS::StringUtils::replace(data, "&#203;", "Ë");
    MVCS::StringUtils::replace(data, "&#204;", "Ì");
    MVCS::StringUtils::replace(data, "&#205;", "Í");
    MVCS::StringUtils::replace(data, "&#206;", "Î");
    MVCS::StringUtils::replace(data, "&#207;", "Ï");
    MVCS::StringUtils::replace(data, "&#208;", "Ð");
    MVCS::StringUtils::replace(data, "&#209;", "Ñ");
    MVCS::StringUtils::replace(data, "&#210;", "Ò");
    MVCS::StringUtils::replace(data, "&#211;", "Ó");
    MVCS::StringUtils::replace(data, "&#212;", "Ô");
    MVCS::StringUtils::replace(data, "&#213;", "Õ");
    MVCS::StringUtils::replace(data, "&#214;", "Ö");
    MVCS::StringUtils::replace(data, "&#215;", "×");
    MVCS::StringUtils::replace(data, "&#216;", "Ø");
    MVCS::StringUtils::replace(data, "&#217;", "Ù");
    MVCS::StringUtils::replace(data, "&#218;", "Ú");
    MVCS::StringUtils::replace(data, "&#219;", "Û");
    MVCS::StringUtils::replace(data, "&#220;", "Ü");
    MVCS::StringUtils::replace(data, "&#221;", "Ý");
    MVCS::StringUtils::replace(data, "&#222;", "Þ");
    MVCS::StringUtils::replace(data, "&#223;", "ß");
    MVCS::StringUtils::replace(data, "&#224;", "à");
    MVCS::StringUtils::replace(data, "&#225;", "á");
    MVCS::StringUtils::replace(data, "&#226;", "â");
    MVCS::StringUtils::replace(data, "&#227;", "ã");
    MVCS::StringUtils::replace(data, "&#228;", "ä");
    MVCS::StringUtils::replace(data, "&#229;", "å");
    MVCS::StringUtils::replace(data, "&#230;", "æ");
    MVCS::StringUtils::replace(data, "&#231;", "ç");
    MVCS::StringUtils::replace(data, "&#232;", "è");
    MVCS::StringUtils::replace(data, "&#233;", "é");
    MVCS::StringUtils::replace(data, "&#234;", "ê");
    MVCS::StringUtils::replace(data, "&#235;", "ë");
    MVCS::StringUtils::replace(data, "&#236;", "ì");
    MVCS::StringUtils::replace(data, "&#237;", "í");
    MVCS::StringUtils::replace(data, "&#238;", "î");
    MVCS::StringUtils::replace(data, "&#239;", "ï");
    MVCS::StringUtils::replace(data, "&#240;", "ð");
    MVCS::StringUtils::replace(data, "&#241;", "ñ");
    MVCS::StringUtils::replace(data, "&#242;", "ò");
    MVCS::StringUtils::replace(data, "&#243;", "ó");
    MVCS::StringUtils::replace(data, "&#244;", "ô");
    MVCS::StringUtils::replace(data, "&#245;", "õ");
    MVCS::StringUtils::replace(data, "&#246;", "ö");
    MVCS::StringUtils::replace(data, "&#247;", "÷");
    MVCS::StringUtils::replace(data, "&#248;", "ø");
    MVCS::StringUtils::replace(data, "&#249;", "ù");
    MVCS::StringUtils::replace(data, "&#250;", "ú");
    MVCS::StringUtils::replace(data, "&#251;", "û");
    MVCS::StringUtils::replace(data, "&#252;", "ü");
    MVCS::StringUtils::replace(data, "&#253;", "ý");
    MVCS::StringUtils::replace(data, "&#254;", "þ");
    MVCS::StringUtils::replace(data, "&#255;", "ÿ");;
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

size_t CardCurler::readfunc(void *ptr, size_t size, size_t nmemb, void *stream)
{
    if (stream)
    {
        postdata *ud = (postdata*) stream;
        int curlSize = size * nmemb;
        int available = (ud->body_size - ud->body_pos);

        if (available > 0)
        {
            int written = std::min( curlSize, available );
            memcpy(ptr, ((char*)(ud->data)) + ud->body_pos, written);
            ud->body_pos += written;
            return written;
        }
    }

    return 0;
}

size_t CardCurler::WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *)userp;

  mem->memory = (char*)realloc(mem->memory, mem->size + realsize + 1);
  if (mem->memory == NULL) {
    /* out of memory! */
    printf("not enough memory (realloc returned NULL)\n");
    exit(EXIT_FAILURE);
  }

  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;

  return realsize;
}
