#ifndef PERSON_H
#define PERSON_H

#include <QString>
#include <QStringList>

class Person
{
public:
    Person();
    QString FirstName;
    QString LastName;
    QStringList Emails;
};

#endif // PERSON_H
