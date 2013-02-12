// encoding: ünicode
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

#ifndef OPTION_H
#define OPTION_H

// fixme and remove these two!
#include <QString>
#include <QStringList>

#include <stdlib.h>

class Option
{
public:
    Option(int argc, char **argv);
    std::string getOption(const std::string &option);
    bool hasOption(const std::string &option);
    void trimQuotes(QString *s);
    void trimChar(QString *s, const QChar &c);

    static bool isVerbose();

private:
    int _argc;
    char **_argv;
};

#endif // OPTION_H
