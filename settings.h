#ifndef SETTINGS_H
#define SETTINGS_H

#include <QCoreApplication>
#include <QSettings>

class Settings
{
public:
    Settings();
    static void SetApplicationProprties();
    void setProperty(QString key, QVariant value);
    QString getProperty(QString key);

private:
    QSettings cfg;
};

#endif // SETTINGS_H
