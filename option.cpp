#include "option.h"

Option::Option()
{
}

bool Option::hasOption(int argc, char **argv, const QString &option) {
    for(int i=1; i<argc; i++) {
        QString argument(argv[i]);
        if(argument == option)
            return true;
    }

    return false;
}

QString Option::getOption(int argc, char **argv, const QString &option) {
    for(int i=1; i<argc; i++) {
        QString argument(argv[i]);

        QStringList tokens = argument.split("=");
        if(tokens.isEmpty()) return QString();

        // sanitize me!
        if(tokens.at(0) == option) return tokens.at(1);
    }

    return QString();
}

