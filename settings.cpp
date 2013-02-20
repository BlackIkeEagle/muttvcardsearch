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

#include "settings.h"
#include "stringutils.h"

// CTOR creates settings hash
Settings::Settings() {
    valid = false;
    changed = false;

    std::string line;
    std::string filename (getConfigFile());
    std::ifstream f (filename.c_str());

    if(f.is_open()) {
        int numSettings = 0; // we have 3
        while(getline(f, line)) {
            std::vector<std::string> tokens = StringUtils::split(line, "=");
            if(tokens.size() == 2) {
                cfg[tokens.at(0)] = tokens.at(1);
                numSettings++;
            }
        }
        f.close();

        if(numSettings == 3)
            valid = true;
    }
}

Settings::~Settings() {
    if(changed) {
        std::ofstream o;
        o.open(getConfigFile().c_str(), ios::out | ios::trunc);

        if(o.is_open()) {
            CfgMap::iterator end = cfg.end();
            for (CfgMap::const_iterator it = cfg.begin(); it != end; ++it) {
                o << it->first << "=" << it->second << endl;
            }

            o.close();

        } else {
            cerr << "Can't update/write config file '" << getConfigFile() << "'" << endl;
        }
    }

    cfg.clear();
}

bool Settings::isValid() {
    return valid;
}

void Settings::setProperty(std::string key, std::string &value) {
    cfg[key] = value;
    changed = true;
}

std::string Settings::getProperty(const std::string& key) {
    std::string res;
    CfgMap::const_iterator it = cfg.begin();
    it = cfg.find(key);
    if(it != cfg.end())
        res = it->second;

    return res;
}

const std::string Settings::getConfigDir() {
    std::string s = CONFIG_DIR;
    return s;
}

const std::string Settings::getConfigFile() {
    std::string s = FileUtils::getHomeDir();
    s.append("/").append(CONFIG_DIR).append("/").append(CONFIG_FILE);
    return s;
}

const std::string Settings::getCacheFile() {
    std::string s = FileUtils::getHomeDir();
    s.append("/").append(CONFIG_DIR).append("/cache.sqlite3");
    return s;
}
