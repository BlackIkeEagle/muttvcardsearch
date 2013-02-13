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

#include "stringutils.h"

vector<string> MVCS::StringUtils::split(const string &s, const string &token)
{
    // return this
    vector<string> result;

    // at pos begins the first char of the token...
    unsigned long pos = s.find(token, 0);

    // return original string if the token was not found
    if ( pos == std::string::npos ) {
        result.push_back(s);
        return result;
    }

    std::string tmp;
    while(pos != std::string::npos) {

        // first iteration
        if(tmp.length() == 0)
            tmp = s;

        result.push_back(tmp.substr(0, pos));

        tmp = tmp.substr(pos + token.length()); // fast forward to skip the token itself
        pos = tmp.find(token, 0);

        // only one match found?
        if(pos == std::string::npos) {
            result.push_back(tmp);
        }
    }

    return result;
}

bool MVCS::StringUtils::endsWith(const string &text, string suffix) {
    if( text.length() == 0 || suffix.length() == 0 )
        return false;

    if( suffix.length() > text.length() )
        return false;

    string tmp = text.substr(text.length() - suffix.length());

    // transform both to lowercase for testing, and thus, ignores case
    std::transform(tmp.begin(), tmp.end(), tmp.begin(), ::tolower);
    std::transform(suffix.begin(), suffix.end(), suffix.begin(), ::tolower);

    if(tmp == suffix) return true;
    return false;
}

bool MVCS::StringUtils::contains(const string &text, const string& pattern) {
    if( text.length() == 0 || pattern.length() == 0 )
        return false;

    if( pattern.length() > text.length() )
        return false;

    unsigned long pos = text.find(pattern, 0);

    if(pos > 0) return true;
    return false;
}

void MVCS::StringUtils::replace(string *text, const string &from, const string &to) {
    unsigned long pos = text->find(from);

    // text contains something to replace
    while(pos != std::string::npos) {
        *text = text->replace(pos, from.length(), to);
        pos = text->find(from);
    }
}
