#include "intrepcompiler.h"
#include "codegenerator.h"

#include <sstream>


std::string IntRepCompiler::GenerateAssembly(IntRep::IntRep i,
                                             CodeGen::Registers16& registers16,
                                             bool isReturnStatement)
{

    std::stringstream ss;

    for (const IntRep::Element& e : i.m_elements)
    {
        switch (e.element)
        {
            case IntRep::ElementType::kDeclareTemporary:
            {
                int reg = FindNewReg(registers16);

                if (reg == -1)
                    throw std::runtime_error("ran out of registers");

                VariableStore::Var v = e.v1;

                registers16.regs[reg].var = v;
                registers16.regs[reg].use = CodeGen::RegisterUse::Variable16;

                break;
            }
            case IntRep::ElementType::kDeleteTemporary:
            {
                int reg = FindRegForVar(registers16, e.v1, true);

                if (reg == -1)
                    throw std::runtime_error("could not find temporary");

                registers16.regs[reg].var = VariableStore::Var(0);
                registers16.regs[reg].use = CodeGen::RegisterUse::Empty;

                break;
            }
            case IntRep::ElementType::kAdd:
            {
                // Find reg for op1

                int reg1 = FindRegForVar(registers16, e.v1);

                // Find reg for op2

                int reg2 = FindRegForVar(registers16, e.v2);

                // Find reg for output register

                int reg3 = FindRegForVar(registers16, e.v3);

                if (reg1 == -1 || reg2 == -1 || reg3 == -1)
                    throw std::runtime_error("Could not find register");

                // Load op1 into r0

                ss << SPACES << "load r0, r" << reg1 << std::endl;

                // Add op2 to r0

                ss << SPACES << "add  r0, r" << reg2 << std::endl;

                // Store r0 to output register

                ss << SPACES << "load r" << reg3 << ", r0" << std::endl;

                break;
            }
            case IntRep::ElementType::kSub:
            {
                // Find reg for op1

                int reg1 = FindRegForVar(registers16, e.v1);

                // Find reg for op2

                int reg2 = FindRegForVar(registers16, e.v2);

                // Find reg for output register

                int reg3 = FindRegForVar(registers16, e.v3);

                if (reg1 == -1 || reg2 == -1 || reg3 == -1)
                    throw std::runtime_error("Could not find register");

                // Load op1 into r0

                ss << SPACES << "load r0, r" << reg1 << std::endl;

                // Add op2 to r0

                ss << SPACES << "sub  r0, r" << reg2 << std::endl;

                // Store r0 to output register

                ss << SPACES << "load r" << reg3 << ", r0" << std::endl;

                break;
            }
            case IntRep::ElementType::kLoadImm:
            {
                // Find reg to load into

                int reg = FindRegForVar(registers16, e.v1);

                // Generate instructions

                int immval = e.immval.v.uval;

                if (immval >= 16)
                    ss << SPACES << "imm  " << ((immval & 0xfff0) >> 4) << std::endl;

                ss << SPACES << "load r" << reg << ", " << (immval & 0xf) << std::endl;

                break;
            }
            case IntRep::ElementType::kCallFunction:
            {

                // For each of the args supplied need to preserve registers

                    // TODO

                // Load first four args into registers

                for (int reg = 4; reg < 8; reg++)
                {
                    if (reg - 4 >= e.args.size())
                        break;

                    int regSrc = FindRegForVar(registers16, e.args[reg - 4]);

                    ss << SPACES << "load r" << reg << ", r" << regSrc << std::endl;

                    registers16.regs[reg].var = e.args[reg - 4];
                    registers16.regs[reg].dirty = true;
                    registers16.regs[reg].use = CodeGen::RegisterUse::Variable16;

                }

                // Push remaining args on the stack

                int reg = FindRegForVar(registers16, e.v1);

                FunctionStore::Func f = e.f1;

                ss << SPACES << "call " << f->asmName << std::endl;
                ss << SPACES << "load r" << reg << ", r0" << std::endl;

                // Restore registers

                    // TODO

                break;
            }
            case IntRep::ElementType::kOutput:
            {
                if (isReturnStatement)
                {
                    int reg = FindRegForVar(registers16, e.v1);

                    if (reg == -1)
                        throw std::runtime_error("Could not find register!");

                    ss << SPACES << "load r0, r" << reg << std::endl;
                }

                break;
            }
        }

    }


    // If this is for a return statement, we need to place the output
    // in r0 if 16 bit, or on the stack.

    return ss.str();
}

int IntRepCompiler::FindNewReg(CodeGen::Registers16& registers16)
{

    for (int reg = 8; reg < 16; reg++)
    {
        if (registers16.regs[reg].use == CodeGen::RegisterUse::Empty)
            return reg;
    }

    return -1;

}

int IntRepCompiler::FindRegForVar(CodeGen::Registers16& registers16, const VariableStore::Var& v, bool aboveReg8)
{
    int reg = 0;

    if (aboveReg8)
        reg = 8;

    for (reg; reg < 16; reg++)
    {
        if (registers16.regs[reg].var == v)
            return reg;
    }

    return -1;

}
