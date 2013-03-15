/***************************************************************************
 *   Copyright (C) 2013 by Torsten Flammiger                               *
 *   github@netfg.net                                                      *
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

#include "url.h"

Url::Url()
{
}

std::string Url::removePath(const std::string &url) {
    if(url.size() < 10) {
        std::cerr << "Invalid URL: '" << url << "'' An url of length '" << url.size() << "' can't really be valid." << std::endl;
        return "";
    }

    size_t pos = url.find("//");
    if(pos == std::string::npos) {
        std::cerr << "Invalid URL: '" << url << "'' It has no '//' to distinguish between scheme and host part" << std::endl;
        return "";
    }

    std::string scheme   = url.substr(0, pos-1);
    std::string hostpart = url.substr(pos+2);

    pos = hostpart.find("/");
    if(pos != std::string::npos) {
        hostpart = hostpart.substr(0, pos);
    }

    std::string result = scheme.append("://").append(hostpart);
    return result;
}
