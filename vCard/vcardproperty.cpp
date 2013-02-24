#include "vCard/vcardproperty.h"
#include "vCard/strutils.h"
#include <fstream>
#include <sstream>

vCardProperty::vCardProperty()
{
}

vCardProperty::vCardProperty(const std::string& name, const std::string& value, const vCardParamList& params)
    :   m_name(name),
        m_params(params)
{
    StrUtils::split(&m_values, value, VC_SEPARATOR_TOKEN);
}

vCardProperty::vCardProperty(const std::string& name, const std::vector< std::string >& values, const vCardParamList& params)
    :   m_name(name),
        m_values(values),
        m_params(params)
{
}

vCardProperty::vCardProperty(const std::string& name, const std::string& value, const std::string& params)
    :   m_name(name)
{
    StrUtils::split(&m_values, value, VC_SEPARATOR_TOKEN);
    m_params = vCardParam::fromString(params);
}

vCardProperty::vCardProperty(const std::string& name, const std::vector< std::string >& values, const std::string& params)
    :   m_name(name),
        m_values(values)
{
    m_params = vCardParam::fromString(params);
}

vCardProperty::~vCardProperty()
{
}

std::string vCardProperty::name() const
{
    return m_name;
}

std::string vCardProperty::value() const
{
    return StrUtils::join(&m_values, VC_SEPARATOR_TOKEN);
}

std::vector<std::string> vCardProperty::values() const
{
    return m_values;
}

vCardParamList vCardProperty::params() const
{
    return m_params;
}

bool vCardProperty::isValid() const
{
    if(m_name.size() == 0)
        return false;

    if(m_values.empty())
        return false;

    for (unsigned int i=0; i<m_params.size(); i++) {
        if (!m_params.at(i).isValid())
            return false;
    }

    return true;
}

bool vCardProperty::operator== (const vCardProperty& prop) const
{
    return ((m_name == prop.name()) && (m_values == prop.values()));
}

bool vCardProperty::operator!= (const vCardProperty& prop) const
{
    return ((m_name != prop.name()) || (m_values != prop.values()));
}

std::string vCardProperty::toString(vCardVersion version) const
{
    return std::string();
//    QByteArray buffer;

//    switch (version)
//    {
//        case VC_VER_2_1:
//        case VC_VER_3_0:
//        {
//            buffer.append(m_name).toUpper();
//            QByteArray params = vCardParam::toByteArray(m_params, version);

//            if (!params.isEmpty())
//            {
//                buffer.append(VC_SEPARATOR_TOKEN);
//                buffer.append(params);
//            }

//            buffer.append(QString(VC_ASSIGNMENT_TOKEN));
//            buffer.append(m_values.join(QString(VC_SEPARATOR_TOKEN)));
//        }
//        break;

//        default:
//            break;
//    }

//    return buffer;
}

std::vector<vCardProperty> vCardProperty::fromString(const std::string& data)
{
    std::vector<vCardProperty> properties;

    std::string line;
    std::stringstream ss(data);
    while(std::getline(ss, line)) {
        if (line == VC_BEGIN_TOKEN || line == VC_END_TOKEN)
            break;

        // tokens conatins a pair of key and value per line
        std::pair<std::string, std::string> tokens = StrUtils::simpleSplit(line,VC_ASSIGNMENT_TOKEN);
        if(tokens.first.size() == 0 && tokens.second.size() == 0)
            break;

        // check if the name has params
        bool hasModifiers = false;
        std::string paramName = tokens.first;
        size_t pos = tokens.first.find(VC_SEPARATOR_TOKEN, 0);
        if(pos != std::string::npos) { // not a single param name without modifiers
            paramName = tokens.first.substr(0, pos);
            hasModifiers = true;
        }

        // the value after the first occurance of VC_ASSIGNMENT_TOKEN
        std::string paramValue = StrUtils::trim(tokens.second);

        if (paramName != VC_VERSION) {
            if(hasModifiers == false) { // there are no properties in the patameterName, like "FN:John Doe"
                properties.push_back( vCardProperty(paramName, paramValue) );
            }

            else {
                vCardParamList paramList = vCardParam::fromString(tokens.first.substr(pos+1));
                properties.push_back( vCardProperty(paramName, paramValue, paramList) );
            }
        }
    }

    return properties;
}

vCardProperty vCardProperty::createAddress(const std::string& street, const std::string& locality, const std::string& region, const std::string& postal_code, const std::string& country, const std::string& post_office_box, const std::string& ext_address, const vCardParamList& params)
{
    std::vector<std::string> values;
    std::vector<std::string>::iterator it = values.begin();

    values.insert(it + vCardProperty::PostOfficeBox, post_office_box);
    values.insert(it + vCardProperty::ExtendedAddress, ext_address);
    values.insert(it + vCardProperty::Street, street);
    values.insert(it + vCardProperty::Locality, locality);
    values.insert(it + vCardProperty::Region, region);
    values.insert(it + vCardProperty::PostalCode, postal_code);
    values.insert(it + vCardProperty::Country, country);

    return vCardProperty(VC_ADDRESS, values, params);
}

//vCardProperty vCardProperty::createBirthday(const QDate& birthday, const vCardParamList& params)
//{
//    return vCardProperty(VC_BIRTHDAY, birthday.toString("yyyy-MM-dd"), params);
//}

//vCardProperty vCardProperty::createBirthday(const QDateTime& birthday, const vCardParamList& params)
//{
//    return vCardProperty(VC_BIRTHDAY, birthday.toString("yyyy-MM-ddThh:mm:ssZ"), params);
//}

//vCardProperty vCardProperty::createGeographicPosition(qreal latitude, qreal longitude, const vCardParamList& params)
//{
//    QStringList values;
//    values.insert(vCardProperty::Latitude, QString("%1").arg(latitude));
//    values.insert(vCardProperty::Longitude, QString("%1").arg(longitude));

//    return vCardProperty(VC_GEOGRAPHIC_POSITION, values, params);
//}

vCardProperty vCardProperty::createName(const std::string& firstname, const std::string& lastname, const std::string& additional, const std::string& prefix, const std::string& suffix, const vCardParamList& params)
{
    std::vector<std::string> values;
    std::vector<std::string>::iterator it = values.begin();

    values.insert(it + vCardProperty::Lastname, lastname);
    values.insert(it + vCardProperty::Firstname, firstname);
    values.insert(it + vCardProperty::Additional, additional);
    values.insert(it + vCardProperty::Prefix, prefix);
    values.insert(it + vCardProperty::Suffix, suffix);

    return vCardProperty(VC_NAME, values, params);
}

//vCardProperty vCardProperty::createOrganization(const std::string& name, const std::vector<std::string>& levels, const vCardParamList& params)
//{
//    std::vector<std::string> values;
//    values.push_back(name);
//    values.push_back(levels);

//    return vCardProperty(VC_ORGANIZATION, values, params);
//}


