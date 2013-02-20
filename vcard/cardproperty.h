#ifndef CARDPROPERTY_H
#define CARDPROPERTY_H

#include <string>
#include <vector>
#include <algorithm>

class CardProperty
{
public:
    CardProperty();
    CardProperty(const std::string& propertyName);
    bool operator==(const CardProperty& property) const;

private:
    std::string propertyName;
};

#endif // CARDPROPERTY_H
