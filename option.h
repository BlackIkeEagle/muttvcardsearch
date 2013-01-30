#ifndef OPTION_H
#define OPTION_H

#include <QString>
#include <QStringList>
#include <stdlib.h>

class Option
{
public:
    Option(int argc, char **argv);
    QString getOption(const QString &option);
    bool hasOption(const QString &option);
    void trimQuotes(QString *s);
    void trimChar(QString *s, const QChar &c);

    static bool isVerbose();

private:
    int _argc;
    char **_argv;
};

#endif // OPTION_H
