#ifndef VCARD_H
#define VCARD_H

#include <string>
#include <vector>
#include <list>
#include "vcardproperty.h"

class vCard
{
protected:
    vCardPropertyList m_properties;
    std::string m_raw;

public:
    vCard();
    vCard(const vCard& vcard); // copy constructor
    vCard(const vCardPropertyList& properties);
    ~vCard();

    void setRawData(const std::string& data);
    std::string getRawData() const;
    void addProperty(const vCardProperty& property);
    void removeProperty(const vCardProperty& property);
    void addProperties(const vCardPropertyList& properties);
    vCardPropertyList properties() const;
    vCardProperty property(const std::string& name, const vCardParamList& params = vCardParamList(), bool strict = false) const;
    bool contains(const std::string& property, const vCardParamList& params = vCardParamList(), bool strict = false) const;
    bool contains(const vCardProperty& property) const;
    bool isValid() const;

    int count() const;
    std::string toString(vCardVersion version = VC_VER_2_1) const;

    bool saveToFile(const std::string& filename) const;

    static std::list<vCard> fromString(const std::string& data);
    static std::list<vCard> fromFile(const std::string& filename);
};

typedef std::vector<vCard> vCardList;

#endif // VCARD_H
