#-------------------------------------------------
#
# Project created by QtCreator 2012-10-29T18:25:47
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = qvcardsearch
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app
target.path = /usr/bin
INSTALLS += target

SOURCES += main.cpp \
    cardcurler.cpp \
    person.cpp \
    settings.cpp \
    option.cpp

LIBS += -lcurl -lpugixml -lvcard

HEADERS += \
    cardcurler.h \
    person.h \
    settings.h \
    option.h

RESOURCES += \
    carddavquery.qrc

