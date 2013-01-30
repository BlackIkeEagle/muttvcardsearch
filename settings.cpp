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

void Settings::SetApplicationProprties() {
    QCoreApplication::setApplicationName(APPNAME);
    QCoreApplication::setOrganizationName(APPNAME);
}

void Settings::setProperty(QString key, QVariant value) {
    cfg.setValue(key, value);
}

void Settings::sync() {
    cfg.sync();
}

QString Settings::getProperty(QString key) {
    return cfg.value(key).toString();
}

const char* Settings::getConfigDir() {
    QDir dir = QFileInfo(cfg.fileName()).absolutePath();
    return dir.absolutePath().toStdString().c_str();
}

const char* Settings::getConfigFile() {
    return cfg.fileName().toStdString().c_str();
}

const QString Settings::getCacheFile() {
    QDir dir = QFileInfo(cfg.fileName()).absolutePath();
    QString s(dir.absolutePath());
    s.append("/cache.dat");
    return s;
}
