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

#include "fileutils.h"

FileUtils::FileUtils()
{
}

bool FileUtils::fileExists(const string &file) {
    bool exists = false;

    std::ifstream f(file.c_str());
    if(f) {
        exists = true;
        f.close();
    }

    return exists;
}

bool FileUtils::dirExists(const string &dir) {
    struct stat st;
    if(stat(dir.c_str(), &st) == 0) {
        if(S_ISDIR(st.st_mode))
            return true;
    }

    return false;
}

bool FileUtils::fileRemove(const string &file) {
    if( remove ( file.c_str() ) != 0 )
        return false;

    return true;
}

std::string FileUtils::getFileContent(const string &path) {
    std::ifstream s;
    s.open(path.c_str());

    if(false == s.is_open()) {
        cerr << "Unable to open file '"<< path << "'" << endl;
        return "";
    }

    std::stringstream buffer;
    buffer << s.rdbuf();
    s.close();

    return buffer.str();
}

bool FileUtils::putFileContent(const std::string& path, const std::string &content) {
    std::ofstream s;
    s.open(path.c_str(), ios::out | ios::trunc);

    if(false == s.is_open()) {
        cerr << "Cant write to file '" << path << "' Unable to open!" << endl;
        return false;
    }

    s << content;
    s.close();
    return true;
}

std::string FileUtils::getHomeDir() {
    struct passwd *pw = getpwuid(getuid());
    std::string d1(pw->pw_dir);
    std::string d2(getenv("HOME"));

    // if set ENV['HOME'] and it differs from the one in /etc/passwd
    // then return the value from ENV['HOME'] instead from /etc/passwd
    if(d2.length() > 0 && d2 != d1 && dirExists(d2))
        return d2;

    return d1;
}
