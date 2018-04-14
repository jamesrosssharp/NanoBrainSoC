/*=============================================================================
                                                    _
         .-==-.                   ..--/\/\--..     (_)
    __      ___    __      ____  ___  ____  ___    ____
   /  \  /\/ _ \/\/  \  /\/ _  \/ _ \/ _  \/ _ \/\/ /  \  /\
  / /\ \/ / (_)  / /\ \/ / (_) / (_)/ (_) / (_)  / / /\ \/ /
  \/  \__/\__/\_/\/  \__/\____/ (_) \ __ (\__/\_/_/\/  \__/
                              \_____//  \_\
   .-==-.           --===/\/\/\===---             ..--/\/\--..

    NanoBrain C Compiler (c) 2017 James Ross Sharp


=============================================================================*/

#pragma once

#include "syntax.h"
#include "codegendefs.h"

#include "variablestore.h"
#include "functionstore.h"

#include <iostream>
#include <sstream>
#include <deque>
#include <map>
#include <vector>
#include <utility>

#include <stdint.h>

namespace CodeGen
{

    // We keep an array of Reg16 structures to keep track of which register contains
    // which variable.

    enum class RegisterUse
    {
        Variable16,
        Variable32Low,
        Variable32High,
        Variable64W1,
        Variable64W2,
        Variable64W3,
        Variable64W4,
        Empty
    };

    struct Reg16
    {
        RegisterUse use;
        VariableStore::Var var;
        bool dirty;

        Reg16() : var(0) {}
    };

    struct Registers16
    {
        Reg16 regs[16];

        void reset()
        {
            for (Reg16& r : regs)
            {
                r.use = RegisterUse::Empty;
                r.var = VariableStore::Var{0};
                r.dirty = false;
            }
        }
    };

    class CodeGenerator
    {
    public:
        CodeGenerator(Syntax::Syntagma* toplevel);

        void generate(std::string fname);

    private:

        void generateCode(Syntax::Syntagma* syntagma);
        void generateFunctionDeclaration(Syntax::FunctionDeclaration* func);
        void generateVariableDeclaration(Syntax::VariableDeclaration* var);

        Type generateType(const Syntax::Type* type, const Syntax::DecoratorList* decorators);

        void generateBlock(const Syntax::Block* b, std::stringstream& ss,
                           bool& callsFunctions, bool& lastStatementIsReturn);

        void generateReturnStatement(const Syntax::ReturnStatement* r, std::stringstream& ss,
                                     bool& callsFunctions);

        void generateIfStatement(const Syntax::IfStatement* i, std::stringstream& ss,
                                     bool& callsFunctions);

        void generateAsmStatement(const Syntax::AsmStatement* a, std::stringstream& ss);
        void generateFunctionCall(const Syntax::FunctionCall* f, std::stringstream& ss);

        bool reduceExpression(Variable& var, const Syntax::Syntagma* expression);

        bool inGlobalScope();

        Register convertReg(char* reg);

        void pushScope(std::string scope);
        void popScope();

        Syntax::TopLevel* m_toplevel;

        // we will generate code in these sections
        std::stringstream m_text;
        std::stringstream m_data;
        std::stringstream m_bss;

        Registers16 m_registers16;

        bool m_inGlobalScope;

    };

}

inline std::ostream& operator << (std::ostream& os, const CodeGen::Reg16& r)
{
    os << "REG16 ";
    os << "Use: ";
    switch (r.use)
    {
        case CodeGen::RegisterUse::Empty:
            os << "Empty ";
            break;
        case CodeGen::RegisterUse::Variable16:
            os << "VAR16 ";
            break;
        default:
            os << "unknown ";
            break;
    }

    os << "dirty: " << r.dirty << " ";

    if (r.use != CodeGen::RegisterUse::Empty)
        os << "var: " << *r.var;

    return os;

}

inline std::ostream& operator << (std::ostream& os, const CodeGen::Registers16& regs)
{
    int i = 0;
    for (auto r : regs.regs)
    {
        os << "r" << i++ << " : ";
        os << r << std::endl;
    }

    return os;
}


