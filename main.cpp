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

#include <QFile>
#include <QUrl>
#include <sys/stat.h>
#include <vcard/vcard.h>
#include "option.h"
#include "cardcurler.h"
#include "settings.h"
#include "version.h"

#include "cache.h"

void printError(QString detail) {
    cout << detail.toStdString() << endl << endl;

    cout << "###############################" << endl;
    cout << "   **** " << APPNAME << " ****" << endl;
    cout << "###############################" << endl;
    cout << "          Version " << MAJOR << "." << MINOR << endl;
    cout << "*******************************" << endl << endl;

    cout << ":::::::: ABOUT ::::" << endl;
    cout << APPNAME << " is a small adressbook utility for the mutt mailclient to read your contacts" << endl;
    cout << "from an owncloud or SOGo carddav list. (C) Torsten Flammiger, 2013, Licensed under GPLv2." << endl;
    cout << "See https://github.com/tfl/" << APPNAME << " for updates and source." << endl << endl;

    cout << ":::: CONFIGURE ::::" << endl;
    cout << "$ " << APPNAME << "  --server=OWNCLOUD|SOGo-CARDDAV-URL \\" << endl;
    cout << "                   --username=USERNAME \\" << endl;
    cout << "                   --password=PASSWORD \\" << endl << endl;

    cout << ":::::::: CACHE ::::" << endl;
    cout << "$ " << APPNAME << " --create-local-cache" << endl;
    cout << APPNAME << " will then create a local cache of all your vcards and will return data from" << endl;
    cout << "the cache first. If no data was found '" << APPNAME << "' will then query the server." << endl << endl;

    cout << "::::::: SEARCH ::::" << endl;
    cout << "$ " << APPNAME << " <query>" << endl;
    cout << "where <query> is part of the fullname or email to search. Dont use wildcards, like *" << endl << endl;

    cout << "NOTE: enclose the parameter values in single or double quotes only if they contain whitespace" << endl;
    cout << "NOTE: a valid Owncloud 4.5 carddav resource url looks something like this:" << endl;
    cout << "      http(s)://<YOUR SERVER.COM>/remote.php/carddav/addressbooks/<USERNAME>/<NAME OF YOUR LIST>" << endl;
    cout << "NOTE: the configuration is stored under ~/.config/" << APPNAME << endl;
    cout << "NOTE: your password will not be encrypted! Therefore a chmod go-rwx on ~/.config/" << APPNAME << endl;
    cout << "      will be executed everytime you configure the application" << endl;
    cout << endl;
}

std::vector<std::string> qStringList2Vector(const QStringList& list) {
    std::vector<std::string> result;
    foreach(QString s, list) {
        result.push_back(s.toStdString());
    }

    return result;
}

int main(int argc, char *argv[])
{
    Settings::SetApplicationProprties();
    Settings cfg;
    Option opt(argc, argv);

    if(argc == 4) {
        QString tmp;

        tmp = opt.getOption("--server");
        if(!tmp.isNull() && !tmp.isEmpty()) {
            opt.trimQuotes(&tmp);
            cfg.setProperty("server", tmp);
        } else {
            printError("property --server=xxx missing");
            return 1;
        }

        tmp = opt.getOption("--username");
        if(!tmp.isNull() && !tmp.isEmpty()) {
            opt.trimQuotes(&tmp);
            cfg.setProperty("username", tmp);
        } else {
            printError("property --username=xxx missing");
            return 1;
        }

        tmp = opt.getOption("--password");
        if(!tmp.isNull() && !tmp.isEmpty()) {
            opt.trimQuotes(&tmp);
            cfg.setProperty("password", tmp);
        } else {
            printError("property --password=xxx missing");
            return 1;
        }

        // sync to make sure the config file exists
        cfg.sync();

        // chmod go-a to the config file, ignore the results
        chmod(cfg.getConfigDir(), S_IRUSR | S_IWUSR | S_IXUSR);
        chmod(cfg.getConfigFile(), S_IRUSR | S_IWUSR);

        return 0;
    } else if ( argc != 2) {
        printError("invalid or missing arguments");
        return 1;
    }

    // preset search template
    bool doExport = false;
    QString searchTemplate(":/cardsearch.xml");
    if(opt.hasOption("--create-local-cache")) {
        searchTemplate = ":/carddavexport.xml";
        doExport = true;
    }

    // open the search template file from the executable resource
    QFile input(searchTemplate);
    input.open(QIODevice::ReadOnly);
    QString query(input.readAll());
    input.close();

    // The worker. will read from your owncloud server as well as from your local cache.
    // In the future, it will most probably also write to your owncloud ;)
    QString _arg = QString::fromLatin1(argv[1]);
    CardCurler cc(cfg.getProperty("username"), cfg.getProperty("password"), cfg.getProperty("server"), _arg);

    // there is the cache ;)
    QString cachefile = cfg.getCacheFile();

    if(false == doExport) {
        query = query.arg(_arg).arg(_arg);
    } else {
        if(QFile::exists(cachefile)) {
            if(QFile::remove(cachefile)) {
                cout << "Old cache deleted" << endl;
            }
        }
    }

    QList<Person> persons;

    if(doExport) {
        QUrl url(cfg.getProperty("server"));
        persons = cc.getAllCards(url.toString(QUrl::RemovePath), query);

        if(persons.size() > 0 ) {
            Cache cache;
            if(false == cache.createDatabase())
                return 1;

            //int numRecords = 0;
            foreach(Person p, persons) {
                cache.addVCard(
                            p.FirstName.toStdWString(),
                            p.LastName.toStdWString(),
                            qStringList2Vector(p.Emails),
                            p.rawCardData.toStdWString(),
                            p.lastUpdatedAt.toStdWString()
                );
            }

//            QFile file(cachefile);
//            if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
//                int numRecords = 0;
//                foreach(Person person, persons) {
//                    //cout << person.rawCardData.toStdString() << endl;
//                    file.write(person.rawCardData.toUtf8());
//                    file.write("\n");
//                    numRecords += 1;
//                }
//                file.close();
//                cout << "Cache created (" << numRecords << " records)" << endl;
//            } else {
//                cout << "Failed to create locale cache in " << cachefile.toStdString() << endl;
//            }
        } else {
            cout << "Export failed, nothing found" << endl;
        }
    } else {
        if(QFile::exists(cachefile)) {
            persons = cc.curlCache(QString::fromUtf8(argv[1]));
        }

        if(persons.size() == 0) {
            persons = cc.curlCard(query);
        }

        if(persons.size() > 0) {
            cout << endl; // needed by mutt
            foreach(Person person, persons) {
                foreach(QString email, person.Emails) {
                    // allthough a QString is allready UTF8 we need to convert it into something a
                    // linux console can display without loosing special chars.. like german umlauts for instance
                    // I tested toUtf8().data() successfully with KDE's konsole and a simple xterm and thus it fits my needs
                    cout << email.toUtf8().data() << "\t" << person.FirstName.toUtf8().data() << " " << person.LastName.toUtf8().data() << endl;
                }
            }
        } else {
            cout << "Search returned no results" << endl;
        }
    }

    return 0;
}
