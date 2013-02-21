#ifndef VCARDPARAM_H
#define VCARDPARAM_H

#include "vCard/vcard_globals.h"
#include <string>
#include <vector>

class vCardParam
{
public:
    enum vCardParamGroup
    {
        Type,
        Encoding,
        Charset,
        Undefined
    };

protected:
    vCardParamGroup m_group;
    std::string m_value;

public:
    vCardParam();
    vCardParam(const std::string& value, vCardParamGroup group = vCardParam::Undefined);
    ~vCardParam();

    vCardParamGroup group() const;
    std::string value() const;
    bool isValid() const;

    bool operator== (const vCardParam& param) const;
    bool operator!= (const vCardParam& param) const;

    std::string toString(vCardVersion version = VC_VER_2_1) const;

    static std::string toString(std::vector<vCardParam> params, vCardVersion version = VC_VER_2_1);
    static std::vector<vCardParam> fromString(const std::string& data);
};

typedef std::vector<vCardParam> vCardParamList;

#endif // VCARDPARAM_H
