#include "settings.h"

Settings::Settings()
{

}

void Settings::SetApplicationProprties() {
    QCoreApplication::setApplicationName("qvcardsearch");
    QCoreApplication::setOrganizationName("qvcardsearch");
}

void Settings::setProperty(QString key, QVariant value) {
    cfg.setValue(key, value);
}

QString Settings::getProperty(QString key) {
    return cfg.value(key).toString();
}
