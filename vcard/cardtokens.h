#ifndef VCARDTOKENS_H
#define VCARDTOKENS_H

namespace vCard {
    #define VC_SEPARATOR_TOKEN      ';'
    #define VC_ASSIGNMENT_TOKEN     ':'
    #define VC_TYPE_SEP_TOKEN       ','
    #define VC_END_LINE_TOKEN       '\n'
    #define VC_BEGIN_TOKEN          "BEGIN:VCARD"
    #define VC_END_TOKEN            "END:VCARD"
    #define VC_ADDRESS              "ADR"
    #define VC_AGENT                "AGENT"
    #define VC_BIRTHDAY             "BDAY"
    #define VC_CATEGORIES           "CATEGORIES"
    #define VC_CLASS                "CLASS"
    #define VC_DELIVERY_LABEL       "LABEL"
    #define VC_EMAIL                "EMAIL"
    #define VC_FORMATTED_NAME       "FN"
    #define VC_GEOGRAPHIC_POSITION  "GEO"
    #define VC_KEY                  "KEY"
    #define VC_LOGO                 "LOGO"
    #define VC_MAILER               "MAILER"
    #define VC_NAME                 "N"
    #define VC_NICKNAME             "NICKNAME"
    #define VC_NOTE                 "NOTE"
    #define VC_ORGANIZATION         "ORG"
    #define VC_PHOTO                "PHOTO"
    #define VC_PRODUCT_IDENTIFIER   "PRODID"
    #define VC_REVISION             "REV"
    #define VC_ROLE                 "ROLE"
    #define VC_SORT_STRING          "SORT-STRING"
    #define VC_SOUND                "SOUND"
    #define VC_TELEPHONE            "TEL"
    #define VC_TIME_ZONE            "TZ"
    #define VC_TITLE                "TITLE"
    #define VC_URL                  "URL"
    #define VC_GROUP_TOKEN          "%1=%2"
    #define VC_TYPE_TOKEN           "TYPE"
    #define VC_ENCODING_TOKEN       "ENCODING"
    #define VC_CHARSET_TOKEN        "CHARSET"

    // should only contain tokens defined between BEGIN and END tokens
    typedef std::vector<std::string> VC_TOKEN_LIST;
    static VC_TOKEN_LIST createTokenList() {
        VC_TOKEN_LIST vc_tokenlist;
        vc_tokenlist.push_back(VC_ADDRESS);
        vc_tokenlist.push_back(VC_AGENT);
        vc_tokenlist.push_back(VC_BIRTHDAY);
        vc_tokenlist.push_back(VC_CATEGORIES);
        vc_tokenlist.push_back(VC_CLASS);
        vc_tokenlist.push_back(VC_DELIVERY_LABEL);
        vc_tokenlist.push_back(VC_EMAIL);
        vc_tokenlist.push_back(VC_FORMATTED_NAME);
        vc_tokenlist.push_back(VC_GEOGRAPHIC_POSITION);
        vc_tokenlist.push_back(VC_KEY);
        vc_tokenlist.push_back(VC_LOGO);
        vc_tokenlist.push_back(VC_MAILER);
        vc_tokenlist.push_back(VC_NAME);
        vc_tokenlist.push_back(VC_NICKNAME);
        vc_tokenlist.push_back(VC_NOTE);
        vc_tokenlist.push_back(VC_ORGANIZATION);
        vc_tokenlist.push_back(VC_PHOTO);
        vc_tokenlist.push_back(VC_PRODUCT_IDENTIFIER);
        vc_tokenlist.push_back(VC_REVISION);
        vc_tokenlist.push_back(VC_ROLE);
        vc_tokenlist.push_back(VC_SORT_STRING);
        vc_tokenlist.push_back(VC_SOUND);
        vc_tokenlist.push_back(VC_TELEPHONE);
        vc_tokenlist.push_back(VC_TIME_ZONE);
        vc_tokenlist.push_back(VC_TITLE);
        vc_tokenlist.push_back(VC_URL);
        return vc_tokenlist;
    }
}

#endif // VCARDTOKENS_H
