#include "vCard/vcardproperty.h"
#include <fstream>
#include <sstream>

vCardProperty::vCardProperty()
{
}

vCardProperty::vCardProperty(const std::string& name, const std::string& value, const vCardParamList& params)
    :   m_name(name),
        m_params(params)
{
    //m_values = value.split(VC_SEPARATOR_TOKEN);
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
//    m_values = value.split(VC_SEPARATOR_TOKEN);
//    m_params = vCardParam::fromByteArray(params.toUtf8());
}

vCardProperty::vCardProperty(const std::string& name, const std::vector< std::string >& values, const std::string& params)
    :   m_name(name),
        m_values(values)
{
    //m_params = vCardParam::fromByteArray(params.toUtf8());
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
    return std::string();
    //return m_values.join(QString(VC_SEPARATOR_TOKEN));
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
//    if (m_name.isEmpty())
//        return false;

//    if (m_values.isEmpty())
//        return false;

//    foreach (vCardParam param, m_params)
//        if (!param.isValid())
//            return false;

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

        size_t pos = data.find(VC_ASSIGNMENT_TOKEN, 0);
        if(pos != std::string::npos) {
            std::string name   = data.substr(0, pos);
            std::string params = data.substr(pos+1);

            if(name != VC_VERSION) {
                vCardParamList paramList = vCardParam::fromString(params);
                properties.push_back(vCardProperty(name, params, paramList));
            }
        }
    }

    return properties;
//    std::vector<vCardProperty> properties;

//    std::vector<std::string> lines = QString::fromUtf8(data).split(VC_END_LINE_TOKEN);
//    foreach (QString line, lines)
//    {
//        if (line == VC_BEGIN_TOKEN || line == VC_END_TOKEN)
//            break;

//        QStringList tokens = line.split(VC_ASSIGNMENT_TOKEN);
//        if (tokens.count() >= 2)
//        {
//            QStringList property_tokens = tokens.at(0).split(VC_SEPARATOR_TOKEN);
//            QString name = property_tokens.takeAt(0);

//            if (name != VC_VERSION)
//            {
//                vCardParamList params = vCardParam::fromByteArray(property_tokens.join(QString(VC_SEPARATOR_TOKEN)).toUtf8());

//                properties.append(vCardProperty(name, tokens.at(1), params));
//            }
//        }
//    }

//    return properties;
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


