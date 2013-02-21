#include "vCard/vcardparam.h"

#define VC_GROUP_TOKEN "%1=%2"
#define VC_TYPE_TOKEN "TYPE"
#define VC_TYPE_SEP_TOKEN ','
#define VC_ENCODING_TOKEN "ENCODING"
#define VC_CHARSET_TOKEN "CHARSET"

vCardParam::vCardParam()
    :   m_group(vCardParam::Undefined)
{
}

vCardParam::vCardParam(const std::string& value, vCardParamGroup group)
    :   m_group(group),
        m_value(value)
{
}

vCardParam::~vCardParam()
{
}

vCardParam::vCardParamGroup vCardParam::group() const
{
    return m_group;
}

std::string vCardParam::value() const
{
    return m_value;
}

bool vCardParam::isValid() const
{
    return m_value.size() == 0 ? false : true;
}

bool vCardParam::operator== (const vCardParam& param) const
{
    return ((m_group == param.group()) && (m_value == param.value()));
}

bool vCardParam::operator!= (const vCardParam& param) const
{
    return ((m_group != param.group()) || (m_value != param.value()));
}

std::string vCardParam::toString(vCardVersion version) const
{
    return std::string();
//    QByteArray buffer;

//    switch (version)
//    {
//        case VC_VER_2_1:
//        {
//            switch (m_group)
//            {
//                case vCardParam::Charset:
//                    buffer.append(QString(VC_GROUP_TOKEN).arg(VC_CHARSET_TOKEN).arg(m_value));
//                    break;

//                case vCardParam::Encoding:
//                    buffer.append(QString(VC_GROUP_TOKEN).arg(VC_ENCODING_TOKEN).arg(m_value));
//                    break;

//                default:
//                    buffer.append(m_value);
//                    break;
//            }
//            break;
//        }
//        break;

//        case VC_VER_3_0:
//        {
//            switch (m_group)
//            {
//                case vCardParam::Type:
//                    buffer.append(QString(VC_GROUP_TOKEN).arg(VC_TYPE_TOKEN).arg(m_value));
//                    break;

//                case vCardParam::Charset:
//                    buffer.append(QString(VC_GROUP_TOKEN).arg(VC_CHARSET_TOKEN).arg(m_value));
//                    break;

//                case vCardParam::Encoding:
//                    buffer.append(QString(VC_GROUP_TOKEN).arg(VC_ENCODING_TOKEN).arg(m_value));
//                    break;

//                default:
//                    buffer.append(m_value);
//                    break;
//            }
//        }
//        break;

//        default:
//            break;
//    }

//    return buffer.toUpper();
}

std::string vCardParam::toString(std::vector<vCardParam> params, vCardVersion version)
{
    return std::string();
//    QByteArray buffer;

//    switch (version)
//    {
//        case VC_VER_2_1:
//        {
//            QStringList ps;
//            foreach (vCardParam param, params)
//                ps.append(param.toByteArray(VC_VER_2_1));
//            buffer.append(ps.join(QString(VC_SEPARATOR_TOKEN)));
//        }
//        break;

//        case VC_VER_3_0:
//        {
//            QStringList types;
//            QStringList encodings;
//            QStringList charsets;
//            QStringList unknowns;
//            foreach (vCardParam param, params)
//            {
//                QByteArray param_str = param.toByteArray(VC_VER_2_1);
//                switch (param.group())
//                {
//                    case Type:
//                        types.append(param_str);
//                        break;

//                    case Encoding:
//                        encodings.append(param_str);
//                        break;

//                    case Charset:
//                        charsets.append(param_str);
//                        break;

//                    default:
//                        unknowns.append(param_str);
//                }
//            }

//            unknowns += charsets;
//            unknowns += encodings;

//            if (!types.isEmpty())
//                unknowns.prepend(QString(VC_GROUP_TOKEN).arg(VC_TYPE_TOKEN).arg(types.join(QString(VC_TYPE_SEP_TOKEN))));

//            if (!unknowns.isEmpty())
//                buffer.append(unknowns.join(QString(VC_SEPARATOR_TOKEN)));
//        }
//        break;

//        default:
//            break;
//    }

//    return buffer.toUpper();
}

std::vector<vCardParam> vCardParam::fromString(const std::string& data)
{
    return std::vector<vCardParam>();
//    QList<vCardParam> params;

//    QStringList tokens = QString::fromUtf8(data).simplified().split(VC_SEPARATOR_TOKEN);
//    foreach (QString token, tokens)
//    {
//	int token_size = token.count();
//        if (token.startsWith(VC_TYPE_TOKEN))
//            foreach (QString type, token.right(token_size-5).split(VC_TYPE_SEP_TOKEN))
//                params.append(vCardParam(type, vCardParam::Type));

//        else if (token.startsWith(VC_ENCODING_TOKEN))
//            params.append(vCardParam(token.right(token_size-9), vCardParam::Encoding));

//        else if (token.startsWith(VC_CHARSET_TOKEN))
//            params.append(vCardParam(token.right(token_size-8), vCardParam::Charset));

//        else
//            params.append(vCardParam(token));
//    }

//    return params;
}
