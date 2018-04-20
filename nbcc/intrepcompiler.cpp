#include "intrepcompiler.h"
#include "codegenerator.h"

#include <sstream>


std::string IntRepCompiler::GenerateAssembly(const IntRep::IntRep& i,
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
                registers16.regs[reg].dirty = true;

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

                // Load op1 into r1

                ss << SPACES << "load r1, r" << std::dec << reg1 << std::endl;

                // Add op2 to r1

                ss << SPACES << "add  r1, r" << std::dec << reg2 << std::endl;

                // Store r1 to output register

                ss << SPACES << "load r" << std::dec << reg3 << ", r1" << std::endl;

                registers16.regs[1].dirty = true;

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

                // Load op1 into r1

                ss << SPACES << "load r1, r" << std::dec << reg1 << std::endl;

                // Add op2 to r1

                ss << SPACES << "sub  r1, r" << std::dec << reg2 << std::endl;

                // Store r0 to output register

                ss << SPACES << "load r" << std::dec << reg3 << ", r1" << std::endl;

                registers16.regs[1].dirty = true;

                break;
            }
            case IntRep::ElementType::kBsl:
            {
                // Find reg for op1

                int reg1 = FindRegForVar(registers16, e.v1);

                // Find reg for op2

                int reg2 = FindRegForVar(registers16, e.v2);

                if (reg1 == -1 || reg2 == -1)
                    throw std::runtime_error("Could not find register");

                // Load op1 into r1

                ss << SPACES << "load r1, r" << std::dec << reg1 << std::endl;

                // Add op2 to r1

                ss << SPACES << "bsl  r1, " << std::dec << (e.immval.v.uval & 0xf) << std::endl;

                // Store r0 to output register

                ss << SPACES << "load r" << std::dec << reg2 << ", r1" << std::endl;

                registers16.regs[1].dirty = true;

            }
            case IntRep::ElementType::kBsr:
            {
                // Find reg for op1

                int reg1 = FindRegForVar(registers16, e.v1);

                // Find reg for op2

                int reg2 = FindRegForVar(registers16, e.v2);

                if (reg1 == -1 || reg2 == -1)
                    throw std::runtime_error("Could not find register");

                // Load op1 into r1

                ss << SPACES << "load r1, r" << std::dec << reg1 << std::endl;

                // Add op2 to r1

                ss << SPACES << "bsr  r1, " << std::dec << (e.immval.v.uval & 0xf) << std::endl;

                // Store r0 to output register

                ss << SPACES << "load r" << std::dec << reg2 << ", r1" << std::endl;

                registers16.regs[1].dirty = true;

            }
            case IntRep::ElementType::kAnd:
            {
                // Find reg for op1

                int reg1 = FindRegForVar(registers16, e.v1);

                // Find reg for op2

                int reg2 = FindRegForVar(registers16, e.v2);

                // Find reg for output register

                int reg3 = FindRegForVar(registers16, e.v3);

                if (reg1 == -1 || reg2 == -1 || reg3 == -1)
                    throw std::runtime_error("Could not find register");

                // Load op1 into r1

                ss << SPACES << "load r1, r" << std::dec << reg1 << std::endl;

                // Add op2 to r1

                ss << SPACES << "and  r1, r" << std::dec << reg2 << std::endl;

                // Store r1 to output register

                ss << SPACES << "load r" << std::dec << reg3 << ", r1" << std::endl;

                registers16.regs[1].dirty = true;

                break;
            }
            case IntRep::ElementType::kOr:
            {
                // Find reg for op1

                int reg1 = FindRegForVar(registers16, e.v1);

                // Find reg for op2

                int reg2 = FindRegForVar(registers16, e.v2);

                // Find reg for output register

                int reg3 = FindRegForVar(registers16, e.v3);

                if (reg1 == -1 || reg2 == -1 || reg3 == -1)
                    throw std::runtime_error("Could not find register");

                // Load op1 into r1

                ss << SPACES << "load r1, r" << std::dec << reg1 << std::endl;

                // Add op2 to r1

                ss << SPACES << "or  r1, r" << std::dec << reg2 << std::endl;

                // Store r1 to output register

                ss << SPACES << "load r" << std::dec << reg3 << ", r1" << std::endl;

                registers16.regs[1].dirty = true;

                break;
            }
            case IntRep::ElementType::kXor:
            {
                // Find reg for op1

                int reg1 = FindRegForVar(registers16, e.v1);

                // Find reg for op2

                int reg2 = FindRegForVar(registers16, e.v2);

                // Find reg for output register

                int reg3 = FindRegForVar(registers16, e.v3);

                if (reg1 == -1 || reg2 == -1 || reg3 == -1)
                    throw std::runtime_error("Could not find register");

                // Load op1 into r1

                ss << SPACES << "load r1, r" << std::dec << reg1 << std::endl;

                // Add op2 to r1

                ss << SPACES << "xor  r1, r" << std::dec << reg2 << std::endl;

                // Store r1 to output register

                ss << SPACES << "load r" <<  std::dec << reg3 << ", r1" << std::endl;

                registers16.regs[1].dirty = true;

                break;
            }
            case IntRep::ElementType::kTest:
            {
                // Find reg for op1

                int reg1 = FindRegForVar(registers16, e.v1);

                if (reg1 == -1)
                    throw std::runtime_error("Could not find register");

                // Load op1 into r1

                ss << SPACES << "test r" << reg1 << ", 0x" << std::hex << e.immval.v.uval << std::endl;

                break;
            }
            case IntRep::ElementType::kJumpZ:
            {
                LabelStore::Label l = e.label;

                ss << SPACES << "jumpz " << l->asmName << std::endl;

                break;
            }
            case IntRep::ElementType::kJumpNZ:
            {
                LabelStore::Label l = e.label;

                ss << SPACES << "jumpnz " << l->asmName << std::endl;

                break;
            }
            case IntRep::ElementType::kLoadImm:
            {
                // Find reg to load into

                int reg = FindRegForVar(registers16, e.v1);

                // Generate instructions

                int immval = e.immval.v.uval;

                ss << SPACES << "load r" << std::dec << reg << ", 0x" << std::hex << immval << std::endl;

                break;
            }
            case IntRep::ElementType::kCallFunction:
            {

                // Load first four args into registers

                std::vector<int> saveregs;

                for (int reg = 4; reg < 8; reg++)
                {
                    if (reg - 4 >= e.args.size())
                        break;

                    if (registers16.regs[reg].use != CodeGen::RegisterUse::Empty)
                    {
                        saveregs.push_back(reg);
                        ss << SPACES << "stw r" << std::dec << reg << ", [s8, 0]" << std::endl;
                        ss << SPACES << "incw s8" << std::endl;
                    }

                    int regSrc = FindRegForVar(registers16, e.args[reg - 4]);

                    ss << SPACES << "load r" << std::dec << reg << ", r" << regSrc << std::endl;

                }

                // Push remaining args on the stack

                int reg = FindRegForVar(registers16, e.v1);

                FunctionStore::Func f = e.f1;

                ss << SPACES << "call " << f->asmName << std::endl;

                if (! (f->returnType.name == "void"))
                {
                    // Handle return type
                    ss << SPACES << "load r" << std::dec << reg << ", r0" << std::endl;
                }
                else
                {
                    registers16.regs[reg].var = VariableStore::Var(0);
                    registers16.regs[reg].use = CodeGen::RegisterUse::Empty;
                }

                // Restore registers

                for (auto iter = saveregs.rbegin(); iter != saveregs.rend(); ++iter)
                {
                    ss << SPACES << "decw s8" << std::endl;
                    ss << SPACES << "ldw  r" << *iter << ", [s8, 0]" << std::endl;
                }

                registers16.regs[0].dirty = true;

                break;
            }
            case IntRep::ElementType::kOutput:
            {
                if (isReturnStatement)
                {
                    int reg = FindRegForVar(registers16, e.v1);

                    if (reg == -1)
                        throw std::runtime_error("Could not find register!");

                    ss << SPACES << "load r0, r" << std::dec << reg << std::endl;

                    registers16.regs[0].dirty = true;
                }

                break;
            }
            case IntRep::ElementType::kLoadSym:
            {
                int reg1 = FindRegForVar(registers16, e.v1);

                // Search for the symbol. If it's not in a register already...
                int reg2 = FindRegForVar(registers16, e.v2);
                // ... search for it on the stack.
                    // ...
                // ... else it must be in the data section?

                    // ...
                if (reg1 == -1 || reg2 == -1)
                {
                    std::cout << registers16 << std::endl;
                    throw std::runtime_error("Could not find variables!");
                }

                ss << SPACES "load r" << std::dec << reg1 << ", r" << reg2 << std::endl;

                break;
            }
            case IntRep::ElementType::kInc:
            {
                int reg1 = FindRegForVar(registers16, e.v1);

                if (reg1 == -1)
                    throw std::runtime_error("Could not find a variable");

                ss << SPACES "add r" << reg1 << ", 1" << std::endl;

                break;
            }
            case IntRep::ElementType::kDec:
            {
                int reg1 = FindRegForVar(registers16, e.v1);

                if (reg1 == -1)
                    throw std::runtime_error("Could not find a variable");

                ss << SPACES "sub r" << reg1 << ", 1" << std::endl;

                break;
            }
            case IntRep::ElementType::kLoadVar:
            {
                int reg1 = FindRegForVar(registers16, e.v1);
                int reg2 = FindRegForVar(registers16, e.v2);

                if (reg1 == -1 || reg2 == -1)
                    throw std::runtime_error("Could not find registers!");

                ss << SPACES "load r" << reg1 << ", r" << reg2 << std::endl;

                break;
            }
            case IntRep::ElementType::kStoreSym:
            {
                int reg1 = FindRegForVar(registers16, e.v1);

                int reg2 = FindRegForVar(registers16, e.v2);

                if (reg1 == -1)
                    throw std::runtime_error("Could not find register!");

                // If reg2 isn't in a register look for it on the stack or in the data section

                if (reg2 == -1)
                {
                    // TODO
                    throw std::runtime_error("Could not find register!");
                }

                ss << SPACES "load r" << reg2 << ", r" << reg1 << std::endl;

                break;
            }
            default:
                throw std::runtime_error("Unknown intrep element!");
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
