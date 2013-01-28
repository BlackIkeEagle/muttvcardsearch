#include "settings.h"

void Settings::SetApplicationProprties() {
    QCoreApplication::setApplicationName(APPNAME);
    QCoreApplication::setOrganizationName(APPNAME);
}

void Settings::setProperty(QString key, QVariant value) {
    cfg.setValue(key, value);
}

void Settings::sync() {
    cfg.sync();
}

QString Settings::getProperty(QString key) {
    return cfg.value(key).toString();
}

const char* Settings::getConfigDir() {
    QDir dir = QFileInfo(cfg.fileName()).absolutePath();
    return dir.absolutePath().toStdString().c_str();
}

const char* Settings::getConfigFile() {
    return cfg.fileName().toStdString().c_str();
}

const QString Settings::getCacheFile() {
    QDir dir = QFileInfo(cfg.fileName()).absolutePath();
    QString s(dir.absolutePath());
    s.append("/cache.dat");
    return s;
}
