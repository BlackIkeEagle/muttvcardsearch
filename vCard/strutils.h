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

#ifndef STRUTILS_H
#define STRUTILS_H

#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>

class StrUtils
{
public:
    StrUtils();
    static std::string simplify(const std::string& input);
    static std::string trim(std::string& input);
    static std::string ltrim(std::string& input);
    static std::string rtrim(std::string& input);
    static std::string join(const std::vector< std::string >* values, const std::string& token);
    static std::string join(const std::vector< std::string >* values, const char token);

    static void split(std::vector<std::string>* result, const std::string& text, const std::string& token);
    static void split(std::vector<std::string>* result, const std::string& text, const char token);

    static std::pair<std::string, std::string> simpleSplit(const std::string& text, const char token);
    static std::pair<std::string, std::string> simpleSplit(const std::string& text, const std::string& token);

    static bool startWith(const std::string& text, const std::string& prefix);
};

#endif // STRUTILS_H
