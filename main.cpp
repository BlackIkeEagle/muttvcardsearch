#include <QDebug>
#include <QFile>

#include <vcard/vcard.h>
#include "option.h"
#include "cardcurler.h"
#include "settings.h"

void printError(QString detail) {
    cout << detail.toStdString() << endl;
    cout << "qvCardSearch usage:" << endl;
    cout << endl;
    cout << "SEARCHING:" << endl;
    cout << "qvcardsearch <query>" << endl;
    cout << "where <query> is part of the fullname or email to search" << endl << endl;
    cout << "CONFIGURE:" << endl;
    cout << "qvcardsearch --server=OWNCLOUD-CARDDAV-URL \\" << endl;
    cout << "             --username=USERNAME \\" << endl;
    cout << "             --password=PASSWORD \\" << endl;
    cout << endl;
}

int main(int argc, char *argv[])
{
    Settings::SetApplicationProprties();
    Settings cfg;
    Option opt;

    if(argc == 4) {
        QString tmp;

        tmp = opt.getOption(argc, argv, "--server");
        if(!tmp.isNull() && !tmp.isEmpty()) {
            cfg.setProperty("server", tmp);
        } else {
            printError("property --server=xxx missing");
            return 1;
        }

        tmp = opt.getOption(argc, argv, "--username");
        if(!tmp.isNull() && !tmp.isEmpty()) {
            cfg.setProperty("username", tmp);
        } else {
            printError("property --username=xxx missing");
            return 1;
        }

        tmp = opt.getOption(argc, argv, "--password");
        if(!tmp.isNull() && !tmp.isEmpty()) {
            cfg.setProperty("password", tmp);
        } else {
            printError("property --password=xxx missing");
            return 1;
        }

        return 0;
    } else if ( argc != 2) {
        printError("invalid arguments");
        return 1;
    }

    // preset search template
    bool _export = false;
    QString searchTemplate(":/cardsearch.xml");
    if(opt.hasOption(argc, argv, "--create-local-cache")) {
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

    // TODO: use the local chache first!
    if(false == _export) {
        QList<Person> persons = cc.curlCard(
                    cfg.getProperty("server"),
                    cfg.getProperty("username"),
                    cfg.getProperty("password"),
                    QString(query.arg(_arg).arg(_arg))
        );

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
        }
    }

    return 0;
}
