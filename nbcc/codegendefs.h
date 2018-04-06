#pragma once

#include "types.h"

#include <iostream>
#include <sstream>
#include <deque>
#include <map>
#include <vector>
#include <utility>

#include <stdint.h>


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

    // Scopes have variables and functions. They will also have structs and unions at some future stage.
    class Scope
    {
    public:

        Scope(std::string name) :
            m_name(name)
        {

        }

        void addVariable(Variable&& v)
        {
            m_variables.insert(std::pair<std::string, Variable>(v.name, v));
        }

        Variable& variable(std::string name)
        {
            return m_variables.at(name);
        }

    private:
        std::map <std::string, Variable> m_variables;

        // functions can *only* be defined at global scope.
        std::map <std::string, Function> m_functions;

        std::string m_name;
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
    os << v.name << "|" << v.asmName << std::endl;

    return os;
}
