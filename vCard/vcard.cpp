#include "vCard/vcard.h"
#include "vCard/strutils.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

vCard::vCard()
{
}

// Copy CTOR
vCard::vCard(const vCard& vcard)
{
    // don't forget to copy all member vars or they are lost if the
    // object goes out of scope after it was added to a list/vector
    m_properties = vcard.properties();
    m_raw = vcard.getRawData();
}

vCard::vCard(const vCardPropertyList& properties)
    :   m_properties(properties)
{
}

vCard::~vCard()
{
}

void vCard::addProperty(const vCardProperty& property)
{
    int count = m_properties.size();
    for (int i = 0; i < count; i++)
    {
        vCardProperty current = m_properties.at(i);
        if (current.name() == property.name() && current.params() == property.params())
        {
            m_properties[i] = property;
            return;
        }
    }

    m_properties.push_back(property);

}

void vCard::addProperties(const vCardPropertyList& properties)
{
    for(unsigned int i=0; i<properties.size(); i++) {
        this->addProperty(properties.at(i));
    }
}

vCardPropertyList vCard::properties() const
{
    return m_properties;
}

vCardProperty vCard::property(const std::string &name, const vCardParamList& params, bool strict) const
{
    int count = m_properties.size();
    for (int i = 0; i < count; i++)
    {
        vCardProperty current = m_properties.at(i);
        if (current.name() == name)
        {
            vCardParamList current_params = current.params();

            if (strict) {
                if (params != current_params)
                    continue;
            } else {
                for(unsigned int i=0; i<params.size(); i++) {
                    std::vector<vCardParam>::iterator match = std::find(current_params.begin(), current_params.end(), params.at(i));
                    if( match != current_params.end() ) {
                        continue;
                    }
                }
            }

            return current;
        }
    }

    return vCardProperty();
}

bool vCard::contains(const std::string &name, const vCardParamList& params, bool strict) const
{
    int count = m_properties.size();
    for (int i = 0; i < count; i++)
    {
        vCardProperty current = m_properties.at(i);
        if (current.name() != name)
            continue;

        vCardParamList current_params = current.params();

        if (strict)
        {
            if (params != current_params)
                continue;
        } else {
            for(unsigned int i=0; i<params.size(); i++) {
                std::vector<vCardParam>::iterator match = std::find(current_params.begin(), current_params.end(), params.at(i));
                if( match != current_params.end() ) {
                    continue;
                }
            }
        }

        return true;
    }

    return false;
}

bool vCard::contains(const vCardProperty& property) const
{
    std::vector<vCardProperty>::const_iterator match = std::find(m_properties.begin(), m_properties.end(), property);
    return match == m_properties.end() ? false : true;
}

bool vCard::isValid() const
{
    if (m_properties.size() == 0)
        return false;

    for(unsigned int i=0; i<m_properties.size(); i++) {
        vCardProperty prop(m_properties.at(i));
        if(!prop.isValid()) // we could return prop.isValid() but that will return at first iteration...
            return false;
    }

    return true;
}

int vCard::count() const
{
    return m_properties.size();
}

std::string vCard::toString(vCardVersion version) const
{
    std::vector<std::string> lines;
    lines.push_back(VC_BEGIN_TOKEN);

    switch (version)
    {
        case VC_VER_2_1:
            lines.push_back(vCardProperty(VC_VERSION, "2.1").toString());
            break;

        case VC_VER_3_0:
            lines.push_back(vCardProperty(VC_VERSION, "3.0").toString());
            break;

        default:
            return std::string();
    }

    for( unsigned int i=0; i < this->properties().size(); i++ ) {
        vCardProperty property(properties().at(i));
        lines.push_back( property.toString() );
    }

    lines.push_back(VC_END_TOKEN);

//    return lines.join(QString(VC_END_LINE_TOKEN)).toUtf8();
    return std::string();
}


std::vector<vCard> vCard::fromString(const std::string& data)
{
    std::vector<vCard> vcards;
    std::string beginToken (VC_BEGIN_TOKEN);
    std::string endToken (VC_END_TOKEN);

    std::string tmp;
    unsigned long pos = data.find(beginToken, 0);
    while(pos != std::string::npos) {
        if(tmp.size() == 0) tmp = data; // sure you can do better!

        tmp = tmp.substr(pos);
        pos = tmp.find(endToken);

        // extract the current vcard string
        std::string vcard = tmp.substr(0, pos + endToken.size());

        if(vcard.size() > 0) {
            std::string line, tmpLine;
            std::stringstream ss(vcard);

            vCard current;
            bool first = true;

            // REMEMBER: binary data seem to have a fixed line each separated
            // by '\n' and start with a blank sign. So before we add a new
            // property to the current card we iterate to the next line and
            // check if it starts with a single space. We then append this
            // line to the end of the previous one and start over with the next line.
            while(std::getline(ss, line)) {
                if(first) {
                    tmpLine = line;
                    first = false;
                } else {
                    if(StrUtils::startWith(line, " ")) {
                        tmpLine.append(line.data());
                    } else {
                        // write the last line
                        vCardPropertyList props = vCardProperty::fromString(tmpLine);
                        current.addProperties(props);

                        // create the property for the current line which MUST
                        // be different from the previous one.
                        props = vCardProperty::fromString(line);
                        current.addProperties(props);

                        // temporary save the next line again, i.e. skip the parent ELSE
                        first = true;
                    }
                }
            }

            current.setRawData(vcard);

            if(current.isValid())
                vcards.push_back(current);
        }

        tmp = tmp.substr(pos + endToken.size());
        pos = tmp.find(beginToken, 0);
    }

   return vcards;
}

std::vector<vCard> vCard::fromFile(const std::string& filename)
{
    std::ifstream ifs;
    ifs.open(filename.c_str());

    if( !ifs.is_open() ) {
        return std::vector<vCard>();
    }

    std::stringstream buffer;
    buffer << ifs.rdbuf();
    ifs.close();

    return vCard::fromString(buffer.str());
}

void vCard::setRawData(const std::string& data) {
    m_raw = data;
}

std::string vCard::getRawData() const {
    return m_raw;
}
