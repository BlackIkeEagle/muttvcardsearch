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
#include "vCard/strutils.h"

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
    std::vector<std::string> tokens = StringUtils::split(s, href_begin);

    if(tokens.size() >= 1) {
        for(unsigned int i=1; i<tokens.size(); i++) {
            std::vector<std::string> inner = StringUtils::split(tokens.at(i), href_end);;
            if(inner.size() >= 1) {
                std::string url = inner.at(0);

                if(StringUtils::endsWith(url, "vcf")) {
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
        for(unsigned int i=0; i < cardUrls.size(); i++) {
            std::stringstream ss;
            std::string url(cardUrls.at(i));
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
                    std::vector<vCard> cards = vCard::fromString(card);

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

    for(vCardPropertyList::const_iterator it = vcPropertyList.begin(); it != vcPropertyList.end(); ++it ) {
        vCardProperty vcProperty(*it);
        std::string vcName(vcProperty.name());

        if(vcName == VC_EMAIL) {
            std::vector<std::string> emails = vcProperty.values();
            for(std::vector<std::string>::const_iterator it = emails.begin(); it != emails.end(); ++it) {
                std::string email(*it);
                p->Emails.push_back(email);
            }
        } else if(vcName == VC_NAME) {
            std::vector<std::string> values = vcProperty.values();
            if (values.size() >= 2) { // at least firstname and lastname should be there
                std::string fName = values.at(vCardProperty::Firstname);
                std::string lName = values.at(vCardProperty::Lastname);
                if(!fName.size() == 0 && !fName.size() == 0) {
                    p->FirstName = fName;
                    p->LastName = lName;
                }
            }
        } else if(vcName == VC_FORMATTED_NAME) {
            // not nice but there are entries in my carddav server
            // who dont have a N (i.e. VC_NAME property containing firstname and lastname separately)
            // but instead have only the formatted name property. So i will split this string
            // on each whitespace and combine it somewhat friendly... But what about a title?
            if(p->FirstName.size() == 0 || p->LastName.size() == 0) {
                if(vcProperty.values().size() > 0) {
                    std::string formattedName = vcProperty.values().at(0);
                    if(formattedName.size() == 0) {
                        std::cerr << "There is no vcard property at index 0. Therefore I can't read the value of formatted name (FN)." << endl;
                    } else {
                        std::vector<std::string> tokens;
                        formattedName = StrUtils::simplify(formattedName); // remove doubled whitespace
                        formattedName = StrUtils::trim(formattedName); // remove leading and trailing whitespace
                        StrUtils::split(&tokens, formattedName, ' ');
                        if( tokens.size() == 1 ) {
                            p->LastName = tokens.at(0);
                        }
                        else if( tokens.size() >= 2 ) {
                            p->FirstName = tokens.at(0);
                            p->LastName = tokens.at(1);
                        } else {
                            std::cerr << "VCard property 'FN' contains invalid value(s): more then 2 tokens or none at all!: " << formattedName << std::endl;
                        }
                    }
                } else {
                    std::cerr << "VCard has no values for property 'N'' and there are also no values for property 'FN'" << std::endl;
                }
            }
        } else if (vcName == VC_REVISION) {
            // append updated_at to the person
            std::vector<std::string> revs = vcProperty.values();
            if(!revs.size() == 0) {
                p->lastUpdatedAt = revs.at(0);
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
        if(StringUtils::contains(list->at(i), query)) {
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

    // Happy sql injecting... ;)
    std::stringstream ss;
    ss << "select v.firstname, v.lastname, e.mail from vcards v, emails e ";
    ss << "where e.vcardid = v.vcardid and (lower(v.firstname) like '%" << query << "%' ";
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
        isSOGO = true;
    }

    std::vector<std::string> list = StringUtils::split(http_result, vcardAddressBeginToken);
    if(list.size() > 0) {
        for(unsigned int i=1; i<list.size(); i++) {

            if(Option::isVerbose()) {
                cout << list.at(i) << endl;
            }

            std::vector<std::string> _list = StringUtils::split(list.at(i), vcardAddressEndToken);
            // _list contains 2 elements where the first element is a single vcard
            if(_list.size() == 2) {

                std::string s = _list.at(0);
                StringUtils::replace(&s, "&#13;", "");

                if(isSOGO)
                    fixHtml(&s);

                std::vector<vCard> vcards = vCard::fromString(s);

                if(vcards.size() > 0) {
                    for(unsigned int j = 0; j < vcards.size(); j++) {
                        // there is only one vcard in the list - every time ;)
                        Person p;
                        vCard c = vcards.at(j);
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
    StringUtils::replace(data, "&#13;", "");
    StringUtils::replace(data, "&#34;", "\"");
    StringUtils::replace(data, "&#38;", "&");
    StringUtils::replace(data, "&#60;", "<");

    StringUtils::replace(data, "&#62;", ">");
    StringUtils::replace(data, "&#160;", " ");
    StringUtils::replace(data, "&#161;", "¡");
    StringUtils::replace(data, "&#162;", "¢");

    StringUtils::replace(data, "&#163;", "£");
    StringUtils::replace(data, "&#164;", "¤");
    StringUtils::replace(data, "&#165;", "¥");
    StringUtils::replace(data, "&#166;", "¦");

    StringUtils::replace(data, "&#167;", "§");
    StringUtils::replace(data, "&#168;", "¨");
    StringUtils::replace(data, "&#169;", "©");
    StringUtils::replace(data, "&#170;", "ª");
    StringUtils::replace(data, "&#171;", "«");
    StringUtils::replace(data, "&#172;", "¬");
    StringUtils::replace(data, "&#173;", "");
    StringUtils::replace(data, "&#174;", "®");
    StringUtils::replace(data, "&#175;", "¯");
    StringUtils::replace(data, "&#176;", "°");
    StringUtils::replace(data, "&#177;", "±");
    StringUtils::replace(data, "&#178;", "²");
    StringUtils::replace(data, "&#179;", "³");
    StringUtils::replace(data, "&#180;", "´");
    StringUtils::replace(data, "&#181;", "µ");
    StringUtils::replace(data, "&#182;", "¶");
    StringUtils::replace(data, "&#183;", "·");
    StringUtils::replace(data, "&#184;", "¸");
    StringUtils::replace(data, "&#185;", "¹");
    StringUtils::replace(data, "&#186;", "º");
    StringUtils::replace(data, "&#187;", "»");
    StringUtils::replace(data, "&#188;", "¼");
    StringUtils::replace(data, "&#189;", "½");
    StringUtils::replace(data, "&#190;", "¾");
    StringUtils::replace(data, "&#191;", "¿");
    StringUtils::replace(data, "&#192;", "À");
    StringUtils::replace(data, "&#193;", "Á");
    StringUtils::replace(data, "&#194;", "Â");
    StringUtils::replace(data, "&#195;", "Ã");
    StringUtils::replace(data, "&#196;", "Ä");
    StringUtils::replace(data, "&#197;", "Å");
    StringUtils::replace(data, "&#198;", "Æ");
    StringUtils::replace(data, "&#199;", "Ç");
    StringUtils::replace(data, "&#200;", "È");
    StringUtils::replace(data, "&#201;", "É");
    StringUtils::replace(data, "&#202;", "Ê");
    StringUtils::replace(data, "&#203;", "Ë");
    StringUtils::replace(data, "&#204;", "Ì");
    StringUtils::replace(data, "&#205;", "Í");
    StringUtils::replace(data, "&#206;", "Î");
    StringUtils::replace(data, "&#207;", "Ï");
    StringUtils::replace(data, "&#208;", "Ð");
    StringUtils::replace(data, "&#209;", "Ñ");
    StringUtils::replace(data, "&#210;", "Ò");
    StringUtils::replace(data, "&#211;", "Ó");
    StringUtils::replace(data, "&#212;", "Ô");
    StringUtils::replace(data, "&#213;", "Õ");
    StringUtils::replace(data, "&#214;", "Ö");
    StringUtils::replace(data, "&#215;", "×");
    StringUtils::replace(data, "&#216;", "Ø");
    StringUtils::replace(data, "&#217;", "Ù");
    StringUtils::replace(data, "&#218;", "Ú");
    StringUtils::replace(data, "&#219;", "Û");
    StringUtils::replace(data, "&#220;", "Ü");
    StringUtils::replace(data, "&#221;", "Ý");
    StringUtils::replace(data, "&#222;", "Þ");
    StringUtils::replace(data, "&#223;", "ß");
    StringUtils::replace(data, "&#224;", "à");
    StringUtils::replace(data, "&#225;", "á");
    StringUtils::replace(data, "&#226;", "â");
    StringUtils::replace(data, "&#227;", "ã");
    StringUtils::replace(data, "&#228;", "ä");
    StringUtils::replace(data, "&#229;", "å");
    StringUtils::replace(data, "&#230;", "æ");
    StringUtils::replace(data, "&#231;", "ç");
    StringUtils::replace(data, "&#232;", "è");
    StringUtils::replace(data, "&#233;", "é");
    StringUtils::replace(data, "&#234;", "ê");
    StringUtils::replace(data, "&#235;", "ë");
    StringUtils::replace(data, "&#236;", "ì");
    StringUtils::replace(data, "&#237;", "í");
    StringUtils::replace(data, "&#238;", "î");
    StringUtils::replace(data, "&#239;", "ï");
    StringUtils::replace(data, "&#240;", "ð");
    StringUtils::replace(data, "&#241;", "ñ");
    StringUtils::replace(data, "&#242;", "ò");
    StringUtils::replace(data, "&#243;", "ó");
    StringUtils::replace(data, "&#244;", "ô");
    StringUtils::replace(data, "&#245;", "õ");
    StringUtils::replace(data, "&#246;", "ö");
    StringUtils::replace(data, "&#247;", "÷");
    StringUtils::replace(data, "&#248;", "ø");
    StringUtils::replace(data, "&#249;", "ù");
    StringUtils::replace(data, "&#250;", "ú");
    StringUtils::replace(data, "&#251;", "û");
    StringUtils::replace(data, "&#252;", "ü");
    StringUtils::replace(data, "&#253;", "ý");
    StringUtils::replace(data, "&#254;", "þ");
    StringUtils::replace(data, "&#255;", "ÿ");;
}

/*
 * The following helpers where copied from the internet.
 * These functions are duplications in itself. SO CLEANUP, STUPID!
 * I don't know any of those copyright owners. Sorry for that.
 */

void CardCurler::init_vcard(vcdata *vc) {
    vc->len = 0;
    vc->ptr = (char*)malloc(vc->len+1);
    if (vc->ptr == NULL) {
      std::cerr << "CardCurler::init_vcard: malloc() failed!" << std::endl;
      exit(EXIT_FAILURE);
    }
    vc->ptr[0] = '\0';
}

size_t CardCurler::writefunc(void *ptr, size_t size, size_t nmemb, vcdata *vc) {
    size_t new_len = vc->len + size * nmemb;
    vc->ptr = (char*)realloc(vc->ptr, new_len+1);

    if (vc->ptr == NULL) {
        std::cerr << "CardCurler::writefunc: realloc() failed!" << std::endl;
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
    std::cerr << "CardCurler::WriteMemoryCallback: realloc() failed!" << std::endl;
    exit(EXIT_FAILURE);
  }

  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;

  return realsize;
}
