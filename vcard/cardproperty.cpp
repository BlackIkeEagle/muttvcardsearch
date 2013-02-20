#include "cardproperty.h"

CardProperty::CardProperty()
{
}

CardProperty::CardProperty(const std::string &propertyName) {
    this->propertyName = propertyName;
}

bool CardProperty::operator ==(const CardProperty& property) const {
    return this->propertyName == property.propertyName ? true : false;
}
