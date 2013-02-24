/***************************************************************************
 *   Copyright (C) 2013 by Torsten Flammiger                               *
 *   github@netfg.net                                                      *
 *                                                                         *
 *   Based on the work of Emanuele Bertoldi (e.bertoldi@card-tech.it),     *
 *   The Author of libvcard - http://code.google.com/p/libvcard/           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "vCard/vcardparam.h"
#include "vCard/strutils.h"

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
    // resultset
    std::vector<vCardParam> params;

    std::string vcTypeToken(VC_TYPE_TOKEN);
    std::string vcEncodingToken(VC_ENCODING_TOKEN);
    std::string vcCharsetToken(VC_CHARSET_TOKEN);

    std::string tmp = StrUtils::simplify(data);
    std::vector<std::string> tokens;

    StrUtils::split(&tokens, tmp, VC_SEPARATOR_TOKEN);
    for (std::vector<std::string>::iterator it = tokens.begin() ; it != tokens.end(); ++it) {
        std::string token(*it);


        if(StrUtils::startWith(token, VC_TYPE_TOKEN)) {
            std::string type = token.substr(vcTypeToken.size() + 1 );
            params.push_back(vCardParam(type, vCardParam::Type));
        }

        else if(StrUtils::startWith(token, VC_ENCODING_TOKEN)) {
            std::string type = token.substr(vcEncodingToken.size() + 1 );
            params.push_back(vCardParam(type, vCardParam::Encoding));
        }

        else if(StrUtils::startWith(token, VC_CHARSET_TOKEN)) {
            std::string type = token.substr(vcCharsetToken.size() + 1 );
            params.push_back(vCardParam(type, vCardParam::Charset));
        }

        else
            params.push_back(vCardParam(token));
    }

    return params;
}
