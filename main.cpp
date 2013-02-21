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

#include <vcard/card.h>
#include <vector>
#include <sys/stat.h>
#include "option.h"
#include "cardcurler.h"
#include "settings.h"
#include "version.h"
#include "url.h"
#include "cache.h"
#include "fileutils.h"
#include "searchtemplates.h"

void printError(const std::string &detail) {
    cout << detail << endl << endl;

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

int main(int argc, char *argv[])
{
    std::vector<vCard::vCardItem> list = vCard::vCardItem::fromFile("/home/archie/source/vcards.txt");

    Settings cfg;
    Option opt(argc, argv);

    if(argc == 4) {
        std::string tmp;

        tmp = opt.getOption("--server");
        if(!tmp.length() <= 0) {
            //opt.trimQuotes(&tmp);
            cfg.setProperty("server", tmp);
        } else {
            printError("property --server=xxx missing");
            return 1;
        }

        tmp = opt.getOption("--username");
        if(!tmp.length() <= 0) {
            //opt.trimQuotes(&tmp);
            cfg.setProperty("username", tmp);
        } else {
            printError("property --username=xxx missing");
            return 1;
        }

        tmp = opt.getOption("--password");
        if(!tmp.length() <= 0) {
            //opt.trimQuotes(&tmp);
            cfg.setProperty("password", tmp);
        } else {
            printError("property --password=xxx missing");
            return 1;
        }

        // chmod go-a to the config file, ignore the results
        chmod(cfg.getConfigDir().c_str(), S_IRUSR | S_IWUSR | S_IXUSR);
        chmod(cfg.getConfigFile().c_str(), S_IRUSR | S_IWUSR);

        return 0;
    } else if ( argc != 2) {
        printError("invalid or missing arguments");
        return 1;
    }

    if(false == cfg.isValid()) {
        cout << "WARN: Invalid configuration or can't open '" << cfg.getConfigFile() << "'" << endl;
        cout << "WARN: Please create a valid configuration first.'" << endl;
        return 1;
    }

    // contains default search template strings for searching and exporting vcards
    SearchTemplates st;

    // preset search template
    bool doCache = false;

    std::string query = st.getSearchTemplate();
    if(opt.hasOption("--create-local-cache")) {
        query = st.getExportTemplate();

        // overwrite the default export template with one the user provided
        std::string templateFile = FileUtils::getHomeDir() + "/" + cfg.getConfigDir() + "/export.xml";
        if(FileUtils::fileExists(templateFile)) {
            if(false == FileUtils::getFileContent(templateFile, &query))
                return 1;
        }

        doCache = true;

    } else {
        // overwrite the default search template with one the user provided
        std::string templateFile = FileUtils::getHomeDir() + "/" + cfg.getConfigDir() + "/search.xml";
        if(FileUtils::fileExists(templateFile)) {
            if(false == FileUtils::getFileContent(templateFile, &query))
                return 1;
        }
    }

    // The worker. will read from your owncloud server as well as from your local cache.
    // In the future, it will most probably also write to your owncloud ;)
    CardCurler cc(cfg.getProperty("username"), cfg.getProperty("password"), cfg.getProperty("server"), argv[1]);

    // there is the cache ;)
    std::string cachefile = cfg.getCacheFile();
    std::vector<Person> people;

    if(true == doCache) {
        if(FileUtils::fileExists(cachefile)) {
            if(FileUtils::fileRemove(cachefile)) {
                cout << "Old cache deleted" << endl;
            } else {
                cerr << "Failed to remove old cache database: " << cachefile << endl;
                return 1;
            }
        }

        std::string url(Url::removePath(cfg.getProperty("server")));
        people = cc.getAllCards(url, query);

        if(people.size() > 0 ) {
            Cache cache;
            if(false == cache.createDatabase())
                return 1;

            int numRecords = 0;
            std::cout << "Importing vcards" << std::endl;

            for(unsigned int i=0; i<people.size(); i++) {
                Person p = people.at(i);
                cache.addVCard(
                            p.FirstName,
                            p.LastName,
                            p.Emails,
                            p.rawCardData,
                            p.lastUpdatedAt
                );
                numRecords++;
            }

            chmod(cachefile.c_str(), S_IRUSR | S_IWUSR);
            cout << "Cache created (" << numRecords << " records)" << endl;
        } else {
            cout << "Export failed, nothing found" << endl;
        }
    } else {
        // 1. look into the cache
        bool cacheMiss = false;
        if(FileUtils::fileExists(cachefile)) {
            people = cc.curlCache(std::string(argv[1]));
        }

        // nothing found in cache? => search online
        if(people.size() == 0) {
           cacheMiss = true;
           StringUtils::replace(&query, "%s", std::string(argv[1]));
           people = cc.curlCard(query);
        }

        if(people.size() > 0) {
            cout << endl; // needed by mutt
            for(unsigned int i=0; i<people.size(); i++) {
                Person p = people.at(i);
                for(unsigned int j=0; j < p.Emails.size(); j++) {
                    std::cout << p.Emails.at(j) << "\t" << p.FirstName.c_str() << " " << p.LastName.c_str() << std::endl;
                }
            }

            // now update the cache
            if(cacheMiss) {
                Cache cache;
                cache.openDatabase();
                for(unsigned int i=0; i<people.size(); i++) {
                    Person p = people.at(i);
                    cache.addVCard(p.FirstName, p.LastName, p.Emails, p.rawCardData, p.lastUpdatedAt);
                }
            }

        } else {
            cout << "Search returned no results" << endl;
        }
    }

    return 0;
}
