#ifndef OPTION_H
#define OPTION_H

#include <QString>
#include <QStringList>

class Option
{
public:
    Option();
    QString getOption(int argc, char** argv, const QString &option);
    bool hasOption(int argc, char **argv, const QString &option);
    void trimQuotes(QString *s);
    void trimChar(QString *s, const QChar &c);
};

#endif // OPTION_H
