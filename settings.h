#ifndef SETTINGS_H
#define SETTINGS_H

#include <QCoreApplication>
#include <QSettings>
#include <QFileInfo>
#include <QDir>
#include "version.h"

class Settings
{
public:
    static void SetApplicationProprties();
    void setProperty(QString key, QVariant value);
    void sync();
    QString getProperty(QString key);
    const QString getCacheFile();
    const char* getConfigDir();
    const char* getConfigFile();

private:
    QSettings cfg;
};

#endif // SETTINGS_H
