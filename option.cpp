#include "option.h"

Option::Option()
{
}

bool Option::isVerbose() {
    char* env = getenv("SEARCH_VERBOSE");

    if(! env) return false;
    if( strlen(env) == 1 && env[0] == '1') return true;
    return false;
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

