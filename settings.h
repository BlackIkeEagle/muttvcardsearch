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

#include <string>
#include <QCoreApplication>
#include <QSettings>
#include <QFileInfo>
#include <QDir>
#include "version.h"

#define CONFIG_DIR ".config/muttvcardsearch";

class Settings
{
public:
    static void SetApplicationProprties();
    void setProperty(std::string key, std::string& value);
    void sync();
    std::string getProperty(const std::string &key);
    const std::string getCacheFile();
    const std::string getConfigDir();
    const std::string getConfigFile();

private:
    QSettings cfg;
};

#endif // SETTINGS_H
