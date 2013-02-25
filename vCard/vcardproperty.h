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

#ifndef VCARDPROPERTY_H
#define VCARDPROPERTY_H

#include "vCard/vcardparam.h"
#include <string>
#include <vector>
#include <complex>

#define VC_ADDRESS              "ADR"
#define VC_AGENT                "AGENT"
#define VC_BIRTHDAY             "BDAY"
#define VC_CATEGORIES           "CATEGORIES"
#define VC_CLASS                "CLASS"
#define VC_DELIVERY_LABEL       "LABEL"
#define VC_EMAIL                "EMAIL"
#define VC_FORMATTED_NAME       "FN"
#define VC_GEOGRAPHIC_POSITION  "GEO"
#define VC_KEY                  "KEY"
#define VC_LOGO                 "LOGO"
#define VC_MAILER               "MAILER"
#define VC_NAME                 "N"
#define VC_NICKNAME             "NICKNAME"
#define VC_NOTE                 "NOTE"
#define VC_ORGANIZATION         "ORG"
#define VC_PHOTO                "PHOTO"
#define VC_PRODUCT_IDENTIFIER   "PRODID"
#define VC_REVISION             "REV"
#define VC_ROLE                 "ROLE"
#define VC_SORT_STRING          "SORT-STRING"
#define VC_SOUND                "SOUND"
#define VC_TELEPHONE            "TEL"
#define VC_TIME_ZONE            "TZ"
#define VC_TITLE                "TITLE"
#define VC_URL                  "URL"
#define VC_VERSION              "VERSION"

class vCardProperty
{
public:
    enum GenericFields
    {
        DefaultValue = 0
    };

    enum AddressFields
    {
        PostOfficeBox = 0,
        ExtendedAddress,
        Street,
        Locality,           // e.g. City.
        Region,             // e.g. State or province.
        PostalCode,
        Country
    };

    enum NameFields
    {
        Lastname = 0,
        Firstname,
        Additional,
        Prefix,
        Suffix
    };

    enum GeographicPositionFields
    {
        Latitude = 0,
        Longitude
    };

protected:
    std::string m_name;
    std::vector< std::string > m_values;
    vCardParamList m_params;

public:
    vCardProperty();
    vCardProperty(const std::string& name, const std::string& value, const vCardParamList& params = vCardParamList());
    vCardProperty(const std::string& name, const std::vector< std::string >& values, const vCardParamList& params = vCardParamList());
    vCardProperty(const std::string& name, const std::string& value, const std::string& params);
    vCardProperty(const std::string& name, const std::vector< std::string >& values, const std::string& params);
    ~vCardProperty();

    std::string name() const;
    std::string value() const;
    std::vector<std::string> values() const;
    vCardParamList params() const;
    bool isValid() const;

    bool operator== (const vCardProperty& param) const;
    bool operator!= (const vCardProperty& param) const;

    std::string toString(vCardVersion version = VC_VER_2_1) const;

    static std::vector<vCardProperty> fromString(const std::string& data);

    static vCardProperty createAddress(const std::string& street, const std::string& locality, const std::string& region, const std::string& postal_code, const std::string& country, const std::string& post_office_box = "", const std::string& ext_address = "", const vCardParamList& params = vCardParamList());
    static vCardProperty createBirthday(const time_t& birthday, const vCardParamList& params = vCardParamList());
    //static vCardProperty createGeographicPosition(const std::complex& latitude, const std::complex& longitude, const vCardParamList& params = vCardParamList());
    static vCardProperty createName(const std::string& firstname, const std::string& lastname, const std::string& additional = "", const std::string& prefix = "", const std::string& suffix = "", const vCardParamList& params = vCardParamList());
    //static vCardProperty createOrganization(const std::string& name, const std::vector<std::string>& levels = std::vector<std::string>(), const vCardParamList& params = vCardParamList());
};

typedef std::vector<vCardProperty> vCardPropertyList;

#endif // VCARDPROPERTY_H

