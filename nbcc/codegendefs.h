#pragma once

#include <iostream>
#include <sstream>
#include <deque>
#include <map>
#include <vector>
#include <utility>

#include <stdint.h>


namespace CodeGen
{

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
        case CodeGen::BuiltInType::kInt:
            os << "int";
            break;
        case CodeGen::BuiltInType::kChar:
            os << "char";
            break;
        case CodeGen::BuiltInType::kLong:
            os << "long";
            break;
        case CodeGen::BuiltInType::kShort:
            os << "short";
            break;
        case CodeGen::BuiltInType::kUInt:
            os << "unsigned int";
            break;
        case CodeGen::BuiltInType::kUChar:
            os << "unsigned char";
            break;
        case CodeGen::BuiltInType::kULong:
            os << "unsigned long";
            break;
        case CodeGen::BuiltInType::kUShort:
            os << "unsigned short";
            break;
        case CodeGen::BuiltInType::kVoid:
            os << "void";
            break;
        case CodeGen::BuiltInType::kCustom:
            os << t.name;
            break;
    }

    return os;
}
