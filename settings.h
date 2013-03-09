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

#ifndef SETTINGS_H
#define SETTINGS_H

#include "version.h"
#include "fileutils.h"
#include <map>
#include <vector>

#define CONFIG_DIR ".config/muttvcardsearch"
#define CONFIG_FILE "muttvcardsearch.conf"
typedef std::map <std::string, std::map <std::string, std::string> > CfgMap;

class Settings
{
public:
    Settings();
    ~Settings();
    void setSection(const std::string& section);
    void setProperty(const std::string& section, std::string key, std::string& value);
    std::string getProperty(const std::string& section, const std::string &key);
    std::vector<std::string> getSections();
    const std::string getCacheFile();
    const std::string getConfigDir();
    const std::string getConfigFile();
    bool isValid();

private:
    CfgMap cfg;
    bool changed;
    bool valid;
};

#endif // SETTINGS_H
