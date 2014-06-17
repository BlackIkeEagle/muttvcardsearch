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
        std::string section;
        bool enterSection = false; // only valid for the first section
        while(getline(f, line)) {
            if(StringUtils::startsWith(line, "[") && StringUtils::endsWith(line, "]")) {
                std::string s1 = line.substr(1);
                std::string s2 = s1.substr(0, s1.length()-1);
                section = s2;

                std::map< std::string, std::string > _line;
                cfg[section] = _line;

                enterSection = true; // at least one section was found

            } else {
                if(enterSection == false) continue;

                if(line.size() > 0) {
                    std::vector<std::string> tokens = StringUtils::split(line, "=");
                    if(tokens.size() >= 2) {
                        std::string v;
                        if(tokens.size() > 2) {
                            // combine 1 to end
                            for(int a = 1; a < tokens.size(); ++a) {
								if(a > 1) {
									v += "=";
								}
                                v += tokens.at(a);
                            }
                        } else {
                            v = tokens.at(1);
                        }
                        std::map< std::string, std::string > _map = cfg[section];
                        _map.insert(std::pair<std::string, std::string>(tokens.at(0), v));
                        cfg[section] = _map;
                        numSettings++;
                    }
                }
            }
        }
        f.close();

        if( numSettings > 0 && (numSettings % 3) == 0)
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
                std::string section(it->first);
                if(section.size() != 0) {
                    o << "[" << section << "]" << endl;

                    std::map <std::string, std::string> m = it->second;
                    for(std::map <std::string, std::string>::iterator _it = m.begin(); _it != m.end(); _it++) {
                        o << _it->first << "=" << _it->second << endl;
                    }

                    o << endl;
                }
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

std::vector<std::string> Settings::getSections() {
    std::vector<std::string> result;
    CfgMap::iterator end = cfg.end();
    for (CfgMap::const_iterator it = cfg.begin(); it != end; ++it) {
        result.push_back(it->first);
    }
    return result;
}

void Settings::setSection(const string &section) {
    std::map <std::string, std::string> m;
    cfg[section] = m;
}

void Settings::setProperty(const std::string &section, std::string key, std::string &value) {
    std::map <std::string, std::string> m = cfg[section];
    m[key] = value;
    cfg[section] = m;
    changed = true;
}

std::string Settings::getProperty(const std::string& section, const string &key) {
    std::string res;

    std::map <std::string, std::string> m(cfg[section]);
    std::map <std::string, std::string>::const_iterator it = m.begin();
    it = m.find(key);
    if(it != m.end())
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
