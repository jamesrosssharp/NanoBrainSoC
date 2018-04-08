#pragma once

#include "intrep.h"

namespace CodeGen
{
    struct Registers16;
}

class IntRepCompiler
{
public:

    static std::string GenerateAssembly(IntRep::IntRep i, CodeGen::Registers16& registers16 /*, m_registers32*/,
                                        bool isReturnStatement);

    static int FindNewReg(CodeGen::Registers16& registers16);
    static int FindRegForVar(CodeGen::Registers16& registers16, const VariableStore::Var& v, bool aboveReg8 = false);

};

