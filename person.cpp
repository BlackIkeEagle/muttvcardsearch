#include "person.h"

Person::Person()
{
}

bool Person::isValid() {
    if( this->Emails.size() > 0 && !this->FirstName.isEmpty() && !this->LastName.isEmpty() ) {
        return true;
    } else {
        return false;
    }
}
