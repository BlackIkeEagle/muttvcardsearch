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
};

#endif // OPTION_H
