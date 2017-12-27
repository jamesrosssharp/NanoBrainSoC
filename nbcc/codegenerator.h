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
        Variable* var;
    };

    struct Registers16
    {
        Reg16 regs[16];

        void reset()
        {
            for (Reg16& r : regs)
            {
                r.use = RegisterUse::Empty;
                r.var = nullptr;
            }
        }
    };

    class CodeGenerator
    {
    public:
        CodeGenerator(Syntax::Syntagma* toplevel);

        void generate();

    private:

        void generateCode(Syntax::Syntagma* syntagma);
        void generateFunctionDeclaration(Syntax::FunctionDeclaration* func);
        void generateVariableDeclaration(Syntax::VariableDeclaration* var);

        Type generateType(const Syntax::Type* type, const Syntax::DecoratorList* decorators);

        void generateBlock(const Syntax::Block* b, std::stringstream& ss,
                           bool& callsFunctions, bool& lastStatementIsReturn, int& scopeId);

        void generateReturnStatement(const Syntax::ReturnStatement* r, std::stringstream& ss,
                                     bool& callsFunctions);


        bool reduceExpression(Variable& var, const Syntax::Syntagma* expression);

        bool inGlobalScope();

        Syntax::TopLevel* m_toplevel;

        // we will generate code in these sections
        std::stringstream m_text;
        std::stringstream m_data;
        std::stringstream m_bss;

        std::deque<Scope> m_scopes;

        Registers16 m_registers16;

    };

}
