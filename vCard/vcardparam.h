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

#ifndef VCARDPARAM_H
#define VCARDPARAM_H

#include "vCard/vcard_globals.h"
#include <string>
#include <vector>

class vCardParam
{
public:
    enum vCardParamGroup
    {
        Type,
        Encoding,
        Charset,
        Undefined
    };

protected:
    vCardParamGroup m_group;
    std::string m_value;

public:
    vCardParam();
    vCardParam(const std::string& value, vCardParamGroup group = vCardParam::Undefined);
    ~vCardParam();

    vCardParamGroup group() const;
    std::string value() const;
    bool isValid() const;

    bool operator== (const vCardParam& param) const;
    bool operator!= (const vCardParam& param) const;

    std::string toString(vCardVersion version = VC_VER_2_1) const;

    static std::string toString(std::vector<vCardParam> params, vCardVersion version = VC_VER_2_1);
    static std::vector<vCardParam> fromString(const std::string& data);
};

typedef std::vector<vCardParam> vCardParamList;

#endif // VCARDPARAM_H
