#include "card.h"
#include "fileutils.h"
#include "stringutils.h"

void vCard::vCardItem::setProperty(const CardProperty &property) {
    std::vector< CardProperty >::iterator it = std::find(itemProperties.begin(), itemProperties.end(), property);
    if( it == itemProperties.end() ) { // not found -> new
        itemProperties.push_back(property);
    } else {
        itemProperties.insert(it,property); // found -> replace
    }
}

void vCard::vCardItem::setProperties(const std::vector<CardProperty> &properties) {
    for(unsigned int i=0; i<properties.size(); i++) {
        setProperty(properties.at(i));
    }
}

// static function
std::vector< vCard::vCardItem > vCard::vCardItem::fromString(const std::string &data) {

    std::vector< vCard::vCardItem > vcards;
    std::vector<std::string> tokens = StringUtils::split(data, VC_BEGIN_TOKEN);
    if(tokens.size() > 0) {
        for(unsigned int i=0; i<tokens.size(); i++) {
            std::vector<std::string> cardStrings = StringUtils::split(tokens.at(i), VC_END_TOKEN);
            if(cardStrings.size() > 0) {
                for(unsigned int j=0; j<cardStrings.size(); j++) {
                    vCard::vCardItem vcard;
                    vCard::vCardItem::parse(&vcard, cardStrings.at(j));

                    if( ! vcard.isEmpty() )
                        vcards.push_back(vcard);
                }
            }
        }
    }

    return vcards;
}

// static function
std::vector< vCard::vCardItem > vCard::vCardItem::fromFile(const std::string &fileName) {
    if(FileUtils::fileExists(fileName)) {
        std::string fileData;
        if(FileUtils::getFileContent(fileName, &fileData)) { // writes its own message to cerr if open file failed
            return fromString(fileData);
        }
    } else {
        std::cerr << "Can't read from file '" << fileName << "'. File does not exist!" << std::endl;
    }

    return std::vector< vCard::vCardItem >();
}

// static function
void vCard::vCardItem::parse(vCard::vCardItem* vcard, const std::string &data) {
    std::stringstream ss(data);
    std::string line;

    // defined in cardtokens.h
    // Bjarne, is this usefull?
    VC_TOKEN_LIST vc_tokenlist = vCard::createTokenList();

    // getline will split (implicit) stringstream at newline
    while(std::getline(ss, line))
    {
       std::vector<std::string> lineTokens;
       if( line != "" ) {
           for(unsigned int j=0; j<vc_tokenlist.size(); j++) {
               std::string token (vc_tokenlist.at(j));
               lineTokens = StringUtils::split(line, token);
               if(lineTokens.size() == 2) {
                   CardProperty p(lineTokens.at(1));
                   vcard->setProperty(p); // TODO: param handling. I.e. a property EMAIL can have several sub-properties like PRIVATE, INTERNET...
               }
           }
       }
    }
}

bool vCard::vCardItem::isEmpty() const {
    return itemProperties.size() == 0 ? true : false;
}
