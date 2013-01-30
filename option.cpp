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

#include "option.h"

Option::Option(int argc, char **argv)
{
    _argc = argc;
    _argv = argv;
}

bool Option::isVerbose() {
    char* env = getenv("SEARCH_VERBOSE");

    if(! env) return false;
    if( strlen(env) == 1 && env[0] == '1') return true;
    return false;
}

bool Option::hasOption(const QString &option) {
    for(int i=1; i<_argc; i++) {
        QString argument(_argv[i]);
        if(argument == option)
            return true;
    }

    return false;
}

QString Option::getOption(const QString &option) {
    for(int i=1; i<_argc; i++) {
        QString argument(_argv[i]);

        QStringList tokens = argument.split("=");
        if(tokens.isEmpty()) return QString();

        // sanitize me!
        if(tokens.at(0) == option) return tokens.at(1);
    }

    return QString();
}

void Option::trimChar(QString *s, const QChar &c) {
    if(s->startsWith(c)) {
        *s = s->mid(1);
    }

    if(s->endsWith(c)) {
        *s = s->mid(0, s->length() - 1);
    }
}

void Option::trimQuotes(QString *s) {
    trimChar(s, '\'');
    trimChar(s, '"');
}

