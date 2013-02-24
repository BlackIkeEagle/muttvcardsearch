#ifndef VCARD_GLOBALS_H
#define VCARD_GLOBALS_H

#define VC_ASSIGNMENT_TOKEN ':'
#define VC_SEPARATOR_TOKEN ';'
#define VC_TYPE_ASSIGNMENT_TOKEN '='
#define VC_END_LINE_TOKEN '\n'
#define VC_BEGIN_TOKEN "BEGIN:VCARD"
#define VC_END_TOKEN "END:VCARD"

enum vCardVersion
{
    VC_VER_2_1,
    VC_VER_3_0
};

#endif // VCARD_GLOBALS_H
