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

#ifndef VCARD_H
#define VCARD_H

#include <string>
#include <vector>
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

    static std::vector<vCard> fromString(const std::string& data);
    static std::vector<vCard> fromFile(const std::string& filename);
};

typedef std::vector<vCard> vCardList;

#endif // VCARD_H
