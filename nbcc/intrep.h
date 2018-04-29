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
#include "labelstore.h"

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
        kSub,
        kBsr,
        kBsl,
        kAnd,
        kOr,
        kXor,
        kTest,
        kJumpZ,
        kJumpNZ,
        kJumpC,
        kJumpNC,
        kJump,
        kLoadSym,
        kStoreSym,
        kCallFunction,
        kInc,
        kDec,
        kLoadVar,   // Copy data from one variable into another
        kLabelDec,  // Place holder for a label
        kCompare,
    };

    struct Element
    {
        ElementType element;

        VariableStore::Var v1;
        VariableStore::Var v2;
        VariableStore::Var v3;

        LabelStore::Label label;

        ImmediateValue     immval;

        FunctionStore::Func    f1;

        std::vector<VariableStore::Var> args;

        Element() : v1(0), v2(0), v3(0), label(0), f1(0) {}

    };

    class IntRep
    {
    public:

        VariableStore::Var  declareTemporary();
        void addVar(const VariableStore::Var& left, const VariableStore::Var& right, const VariableStore::Var& out);
        void subVar(const VariableStore::Var& left, const VariableStore::Var& right, const VariableStore::Var& out);
        void bslVar(const VariableStore::Var& left, uint8_t shift, const VariableStore::Var& out);
        void bsrVar(const VariableStore::Var& left, uint8_t shift, const VariableStore::Var& out);
        void andVar(const VariableStore::Var& left, const VariableStore::Var& right, const VariableStore::Var& out);
        void xorVar(const VariableStore::Var& left, const VariableStore::Var& right, const VariableStore::Var& out);
        void orVar(const VariableStore::Var& left, const VariableStore::Var& right, const VariableStore::Var& out);

        void loadImm(const VariableStore::Var& var, int32_t intval);
        void loadImm(const VariableStore::Var& var, uint32_t intval);

        void loadSym(VariableStore::Var& temp, VariableStore::Var& sym);

        void test(const VariableStore::Var& v1, uint32_t mask);
        void jumpNZ(const LabelStore::Label& label);
        void jumpZ(const LabelStore::Label& label);
        void jumpNC(const LabelStore::Label& label);
        void jumpC(const LabelStore::Label& label);
        void jump(const LabelStore::Label& label);

        void assignSymbol(VariableStore::Var& v1, VariableStore::Var& v2, VariableStore::Var& out); // assign v2 to v1's shadow variable and also output it

        void deleteTemporary(const VariableStore::Var& temp);

        void output(const VariableStore::Var& out);

        void genFunctionCall(const VariableStore::Var& out, const FunctionStore::FunctionHandle f, const std::vector<VariableStore::Var>& args);

        friend std::ostream& (::operator <<) (std::ostream& os, const IntRep& i);

        VariableStore::Var getOutputVar();

        void assimilate(const IntRep& i);

        void postDecVar(const VariableStore::Var& temp, const VariableStore::Var& out);
        void postIncVar(const VariableStore::Var& temp, const VariableStore::Var& out);
        void preDecVar (const VariableStore::Var& temp, const VariableStore::Var& out);
        void preIncVar (const VariableStore::Var& temp, const VariableStore::Var& out);

        void logicalNotVar (const VariableStore::Var& temp, const VariableStore::Var& out);
        void bitwiseNotVar (const VariableStore::Var& temp, const VariableStore::Var& out);
        void negVar        (const VariableStore::Var& temp, const VariableStore::Var& out);

        void loadVar(const VariableStore::Var& src, const VariableStore::Var& dest);
        void incVar(const VariableStore::Var& v);
        void decVar(const VariableStore::Var& v);
        void storeSym(const VariableStore::Var& src, const VariableStore::Var& dest);

        void labelDec(const LabelStore::Label& l);
        void compare(const VariableStore::Var& v1, const VariableStore::Var& v2); // Inserts a compare operation, caller
                                                                                  // must take action on resulting flags

        void lessThanVar(const VariableStore::Var& left, const VariableStore::Var& right, const VariableStore::Var& out);
        void lessThanEqualVar(const VariableStore::Var& left, const VariableStore::Var& right, const VariableStore::Var& out);
        void greaterThanVar(const VariableStore::Var& left, const VariableStore::Var& right, const VariableStore::Var& out);
        void greaterThanEqualVar(const VariableStore::Var& left, const VariableStore::Var& right, const VariableStore::Var& out);
        void equalVar(const VariableStore::Var& left, const VariableStore::Var& right, const VariableStore::Var& out);
        void notEqualVar(const VariableStore::Var& left, const VariableStore::Var& right, const VariableStore::Var& out);

    private:

        void loadImm(const VariableStore::Var& var, ImmediateValue& v);

        std::vector<Element> m_elements;

        friend class ::IntRepCompiler;

    };

}

