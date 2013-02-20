#ifndef VCARDPP_H
#define VCARDPP_H

#include <vector>
#include <string>
#include <algorithm>
#include <sstream>

#include "cardtokens.h"
#include "cardproperty.h"

namespace vCard {

    class vCardItem
    {
    public:
        void setProperty(const CardProperty& property);
        void setProperties(const std::vector< CardProperty >& properties);
        static std::vector<vCardItem> fromString(const std::string& data);
        static std::vector<vCardItem> fromFile(const std::string& fileName);

    private:
        // functions
        static void parse(vCardItem* vcard, const std::string& data);
        bool isEmpty() const;

        // member vars
        std::vector< CardProperty > itemProperties;
    };

}

#endif // VCARDPP_H
