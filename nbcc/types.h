#pragma once

#include <iostream>

union ValueUnion
{
    char* string;
    unsigned int uval;
    int     sval;
    float   fval;
};

enum class BuiltInType
{
    kInt,
    kUInt,
    kShort,
    kUShort,
    kChar,
    kUChar,
    kLong,
    kULong,
    kVoid,
    kCustom
};

enum class ImmediateType
{
    kInt,
    kUInt,
    kShort,
    kUShort,
    kChar,
    kUChar,
    kLong,
    kULong,
    kFloat,
    kStringLiteral
};

struct ImmediateValue
{
    ImmediateType type;
    ValueUnion  v;
};

inline std::ostream& operator << (std::ostream& os, const ImmediateValue& v)
{
    switch(v.type)
    {
        case ImmediateType::kShort:
            os << "SHORT|" << v.v.sval;
            break;
        case ImmediateType::kUShort:
            os << "USHORT|" << v.v.uval;
            break;
        default:
            os << "UNKNOWN|" << v.v.sval;
            break;
    }

    return os;
}
