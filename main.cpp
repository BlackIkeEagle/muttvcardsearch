#include <QFile>
#include <sys/stat.h>
#include <vcard/vcard.h>
#include "option.h"
#include "cardcurler.h"
#include "settings.h"
#include "version.h"

void printError(QString detail) {
    cout << detail.toStdString() << endl << endl;

    cout << "###############################" << endl;
    cout << "   **** " << APPNAME << " ****" << endl;
    cout << "###############################" << endl;
    cout << "          Version " << MAJOR << "." << MINOR << endl;
    cout << "*******************************" << endl << endl;

    cout << ":::::::: ABOUT ::::" << endl;
    cout << APPNAME << " is a small adressbook utility for the mutt mailclient to read your contacts" << endl;
    cout << "from an owncloud carddav list. (C) Torsten Flammiger, 2013, Licensed under GPLv2." << endl;
    cout << "See https://github.com/tfl/qvcardsearch for updates and source." << endl << endl;

    cout << ":::: CONFIGURE ::::" << endl;
    cout << "$ " << APPNAME << "  --server=OWNCLOUD-CARDDAV-URL \\" << endl;
    cout << "                   --username=USERNAME \\" << endl;
    cout << "                   --password=PASSWORD \\" << endl << endl;

    cout << ":::::::: CACHE ::::" << endl;
    cout << "$ " << APPNAME << " --create-local-cache" << endl;
    cout << APPNAME << " will then create a local cache of all your vcards and will return data from" << endl;
    cout << "the cache first. If no data was found '" << APPNAME << "' will then query the server." << endl << endl;

    cout << "::::::: SEARCH ::::" << endl;
    cout << "$ " << APPNAME << " <query>" << endl;
    cout << "where <query> is part of the fullname or email to search. Dont use wildcards like *" << endl << endl;

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

        // chmod go-a to the config file
        cfg.sync();
        chmod(cfg.getConfigDir(), S_IRUSR | S_IWUSR | S_IXUSR);
        chmod(cfg.getConfigFile(), S_IRUSR | S_IWUSR);

        return 0;
    } else if ( argc != 2) {
        printError("invalid or missing arguments");
        return 1;
    }

    // preset search template
    bool _export = false;
    QString searchTemplate(":/cardsearch.xml");
    if(opt.hasOption("--create-local-cache")) {
        searchTemplate = ":/carddavexport.xml";
        _export = true;
    }

    // open the search template file from the executable resource
    QFile input(searchTemplate);
    input.open(QIODevice::ReadOnly);
    QString query(input.readAll());
    input.close();

    // The worker. will read from your owncloud server as well as from your local cache.
    // In the future, it will most probably also write to your owncloud.
    QString _arg = QString::fromLatin1(argv[1]);
    CardCurler cc(_arg);

    // there is the cache ;)
    QString cachefile = cfg.getCacheFile();

    if(false == _export) {
        query = query.arg(_arg).arg(_arg);
    } else {
        if(QFile::exists(cachefile)) {
            if(QFile::remove(cachefile)) {
                cout << "Old cache deleted" << endl;
            }
        }
    }

    QList<Person> persons;

    // only ask the cache if
    // 1) no export was requested
    // 2) the cachefile exists
    if(!_export && QFile::exists(cachefile)) {
        persons = cc.curlCardCache(QString::fromUtf8(argv[1]));
    }

    // if we (still) found no persons, ask the server
    if(persons.size() == 0) {
        if(_export) cc.setExport();

        persons = cc.curlCard(
            cfg.getProperty("server"),
            cfg.getProperty("username"),
            cfg.getProperty("password"),
            QString(query)
        );
    }

    if(persons.size() > 0) {
        if(false == _export) {
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
            QFile file(cachefile);
            if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                foreach(Person person, persons) {
                    //cout << person.rawCardData.toStdString() << endl;
                    file.write(person.rawCardData.toStdString().c_str());
                    file.write("\n");
                }
                file.close();
                cout << "Cache created" << endl;
            } else {
                cout << "Failed to create locale cache in " << cachefile.toStdString() << endl;
            }
        }
    } else {
        cout << "Nothing found at all" << endl;
    }

    return 0;
}
