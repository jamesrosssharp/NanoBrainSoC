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

    Notes
    =====

    Intermediate representation, use for compiling expressions into
    a form which can be optimised to ensure CPU pipeline is utilised
    to optimal capacity etc.

=============================================================================*/

#pragma once

#include "codegendefs.h"
#include "types.h"
#include "variablestore.h"
#include "functionstore.h"

#include <iostream>
#include <stdint.h>

namespace IntRep
{
    class IntRep;
}

std::ostream& operator << (std::ostream& os, const IntRep::IntRep& i);

class IntRepCompiler;

namespace IntRep
{
    enum class ElementType
    {
        kDeclareTemporary,
        kLoadImm,
        kDeleteTemporary,
        kOutput,
        kAdd,

        kPushFunctionArg,
        kCallFunction
    };

    struct Element
    {
        ElementType element;

        VariableStore::Var v1;
        VariableStore::Var v2;
        VariableStore::Var v3;

        ImmediateValue     immval;

        FunctionStore::Func    f1;

        Element() : v1(0), v2(0), v3(0), f1(0) {}

    };

    class IntRep
    {
    public:

        VariableStore::Var  declareTemporary();
        void addVar(const VariableStore::Var& left, const VariableStore::Var& right, const VariableStore::Var& out);

        void loadImm(const VariableStore::Var& var, int32_t intval);
        void loadImm(const VariableStore::Var& var, uint32_t intval);

        void deleteTemporary(const VariableStore::Var& temp);

        void output(const VariableStore::Var& out);

        void genFunctionCall(const VariableStore::Var& out, const FunctionStore::FunctionHandle f, const std::vector<VariableStore::Var>& args);

        friend std::ostream& (::operator <<) (std::ostream& os, const IntRep& i);

    private:

        void loadImm(const VariableStore::Var& var, ImmediateValue& v);

        std::vector<Element> m_elements;

        friend class ::IntRepCompiler;

    };

}

