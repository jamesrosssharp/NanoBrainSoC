#pragma once

#include "types.h"

#include <iostream>
#include <sstream>
#include <deque>
#include <map>
#include <vector>
#include <utility>

#include <stdint.h>

#define SPACES "             "

namespace CodeGen
{

    struct Type
    {
        bool isPointer;
        bool isConst;
        bool isVolatile;
        bool isStatic;

        std::string name;

        BuiltInType type;
    };


    struct Variable
    {
        std::string name;
        std::string asmName;
        Type type;

        union
        {
            int32_t  i;
            uint32_t u;
            float    f;
        } initialValue;
    };

    struct Function
    {
        std::string name;
        std::string asmName;
        Type returnType;
        std::vector<Variable> arguments;
    };

    enum class Register : uint32_t
    {
        r0 = 0,
        r1,
        r2,
        r3,
        r4,
        r5,
        r6,
        r7,
        r8,
        r9,
        r10,
        r11,
        r12,
        r13,
        r14,
        r15,
        s0 = 16,
        s1,
        s2,
        s3,
        s4,
        s5,
        s6,
        s7,
        s8,
        s9,
        s10,
        s11,
        s12,
        s13,
        s14,
        s15,
        None = 0xff
    };


}

inline std::ostream& operator << (std::ostream& os, const CodeGen::Type& t)
{
    switch (t.type)
    {
        case BuiltInType::kInt:
            os << "int";
            break;
        case BuiltInType::kChar:
            os << "char";
            break;
        case BuiltInType::kLong:
            os << "long";
            break;
        case BuiltInType::kShort:
            os << "short";
            break;
        case BuiltInType::kUInt:
            os << "unsigned int";
            break;
        case BuiltInType::kUChar:
            os << "unsigned char";
            break;
        case BuiltInType::kULong:
            os << "unsigned long";
            break;
        case BuiltInType::kUShort:
            os << "unsigned short";
            break;
        case BuiltInType::kVoid:
            os << "void";
            break;
        case BuiltInType::kCustom:
            os << t.name;
            break;
    }

    return os;
}

inline std::ostream& operator << (std::ostream& os, const CodeGen::Variable& v)
{
    os << v.name << "|" << v.asmName;

    return os;
}

inline std::ostream& operator << (std::ostream& os, const CodeGen::Function& f)
{
    os << f.name;

    return os;
}

