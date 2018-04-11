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


Notes:

r0 - r3 are general purpose registers, used by the compiler for temporaries etc. r0 holds return value (16 bit).
        Larger types are returned on the stack

r4 - r8 are for passing parameters. 32 bit parameters are passed in 2 registers, 64 bit parameters are
        passed in 4 registers. If a function has more than 4 arguments, arguments must be passed on the stack.

r9 - r15 are for holding variables. Two registers will be used for 32 bit variables, 4 registers for 64 bit variables.

s8      stack pointer
s9      "base pointer" - used to create a stack frame from the stack in the function prologue
s10 - s11   used as temprary index registers to access e.g. structures

The Expression class is used to simplify expressions and generate an intermediate representation for assembly.

=============================================================================*/

#include "codegenerator.h"
#include "expression.h"
#include "variablestore.h"
#include "functionstore.h"

#include "intrep.h"
#include "intrepcompiler.h"

#include <iostream>
#include <fstream>

using namespace CodeGen;

CodeGenerator::CodeGenerator(Syntax::Syntagma* toplevel)
{

    m_toplevel = dynamic_cast<Syntax::TopLevel*>(toplevel);

    if (m_toplevel == nullptr)
        throw std::runtime_error("toplevel is not a Top Level!\n");

    m_inGlobalScope = true;

}


void CodeGenerator::generate(std::string fname)
{

    std::ofstream of;

    of.open(fname);

    // start generation

    for (Syntax::Syntagma* s : m_toplevel->declarations())
    {
        generateCode(s);
    }

    // Emit prologue

    of << "\t.org 0h"               << std::endl;
    of                              << std::endl;
    of << "_reset:" << std::endl << SPACES "jump    _start" << std::endl;
    of << "             .align 4"   << std::endl;
    of << "_interrupt:" << std::endl << SPACES  "nop"        << std::endl;
    of << "             nop"        << std::endl;
    of << "_exception:" << std::endl << SPACES  "nop"        << std::endl;
    of << "             nop"        << std::endl;
    of << "_svc:"        << std::endl << SPACES "nop"        << std::endl;
    of << "             nop"        << std::endl;

    of << std::endl << std::endl;

    of << "_start:"                 << std::endl;

    of << "             load r0, (_stack_end & 0x0000ffff)" << std::endl;
    of << "             load r1, (_stack_end & 0xffff0000) >> 16" << std::endl;
    of << "             ldspr s8, r0" << std::endl;

    of << "             call f_main" << std::endl;

    of << "_die:        " << std::endl << SPACES "sleep"      << std::endl;
    of << "             jump _die"  << std::endl;

    of << std::endl << std::endl;


    // Emit data, bss, and text sections


    of << "// --------------- Text ---------------" << std::endl << std::endl;
    of << m_text.str();

    of << "// --------------- Data ---------------" << std::endl << std::endl;
    of << m_data.str();

    of << m_bss.str();

    // Emit stack

    of << std::endl << std::endl;

    of << "             .align 100h" << std::endl;
    of << "_stack_start:"            << std::endl;
    of << "             .times 100h dw 0x0" << std::endl;
    of << "_stack_end:  "            << std::endl;

    // Emit epilogue

    of << "             .end" << std::endl;

}

void CodeGenerator::generateCode(Syntax::Syntagma *syntagma)
{
    switch (syntagma->type())
    {
        case Syntax::ElementType::VariableDeclaration:
            generateVariableDeclaration(dynamic_cast<Syntax::VariableDeclaration*>(syntagma));
            break;
        case Syntax::ElementType::FunctionDeclaration:
            generateFunctionDeclaration(dynamic_cast<Syntax::FunctionDeclaration*>(syntagma));
            break;
    }
}

bool CodeGenerator::inGlobalScope()
{
    return m_inGlobalScope;
}

void CodeGenerator::generateFunctionDeclaration(Syntax::FunctionDeclaration* func)
{

    if (! inGlobalScope())
    {
        std::stringstream errtext;
        errtext << "Function declaration not in global scope " << func->linenum();
        throw std::runtime_error(errtext.str());
    }

    m_inGlobalScope = false;

    Function f;

    f.name = func->functionName();

    // To do: return type, arguments

    std::string funcname = "f_";
    funcname += f.name;

    f.asmName = funcname;

    FunctionStore* fstore = FunctionStore::getInstance();

    fstore->declareFunction(f);

    m_text << funcname << ":" << std::endl;

    // Create a new scope for the function and put the parameters in it

    VariableStore* vstore = VariableStore::getInstance();

    vstore->pushScope(funcname);

    const Syntax::FunctionArguments* args = func->arguments();

    int reg = 4;    // first argument is r4

    m_registers16.reset();

    for (Syntax::Syntagma* a : args->arguments())
    {
        Syntax::Variable* v = dynamic_cast<Syntax::Variable*>(a);

        if (v == nullptr)
            throw std::runtime_error("Function argument not a variable!");

        // Determine type of variable

        const Syntax::Type* t = v->varType();

        Type type = generateType(t, nullptr);

        std::string name = v->name();

        VariableStore::Var var = vstore->declareVar(name, type);

        if (reg < 8)
        {
            switch(var->type.type)
            {
                case BuiltInType::kInt:
                case BuiltInType::kUInt:
                {    // must store these in two 16 bit registers

                    Reg16& r = m_registers16.regs[reg];

                    r.use = RegisterUse::Variable32Low;
                    r.var = var;

                    reg ++;

                    Reg16& r2 = m_registers16.regs[reg];

                    r2.use = RegisterUse::Variable32High;
                    r2.var = var;

                    break;
                }
                case BuiltInType::kShort:
                case BuiltInType::kUShort:
                case BuiltInType::kChar:
                case BuiltInType::kUChar:
                {
                    // can store these in a single 16 bit register

                    Reg16& r = m_registers16.regs[reg];

                    r.use = RegisterUse::Variable16;
                    r.var = var;

                    reg ++;

                    break;
                }
                default:
                {
                    // any types we don't support yet
                    std::stringstream ss;
                    ss << "Unsupported type for parameter: " << var->type.name;
                    throw std::runtime_error(ss.str());
                }
            }

        }
        else
        {
            // Variable passed on the stack.

            throw std::runtime_error("Function has too many arguments: stack based parameter passing not yet supported");
        }


    }

    // Generate function body - this will determine if any functions are called

    std::stringstream functionBody;
    bool functionCallsFunctions = false;
    bool lastStatementIsReturn = false;
    int  scopeId = 0;

    const Syntax::Block* body = func->body();

    generateBlock(body, functionBody, functionCallsFunctions, lastStatementIsReturn, scopeId);

    // pop scope

    vstore->popScope();

    // Generate function prologue. If this function calls any other functions we have
    // to preserve the link register

    bool lrOnStack = false;

    if (functionCallsFunctions)
    {
        m_registers16.regs[0].dirty = true;
        m_registers16.regs[1].dirty = true;

        lrOnStack = true;

    }

        // Check which registers are dirty and preserve them.

    std::vector<int> dirtyRegisters16;

    for (int i = 1; i < 16; i ++)
    {
        if (m_registers16.regs[i].dirty)
            dirtyRegisters16.push_back(i);
    }

        // adjust stack

        // push registers

    for (int reg : dirtyRegisters16)
    {
        m_text << SPACES "stw r" << reg << ", [s8,0]" << std::endl;
        m_text << SPACES "incw s8" << std::endl;
    }

    if (lrOnStack)
    {
        m_text << SPACES "stspr r0, s8" << std::endl;
        m_text << SPACES "stw r0, [s8,0]" << std::endl;
        m_text << SPACES "incw s8" << std::endl;
        m_text << SPACES "stw r1, [s8,0]" << std::endl;
        m_text << SPACES "incw s8" << std::endl;
    }


    // Generate function epilogue. If this function calls any other function we have to
    // restore the link register.

    // final return

    if (! lastStatementIsReturn)
    {
        // Check return type. If it not "void", generate an error / warning

        functionBody << "%EMIT_CLEAN_AND_RET%" << std::endl;
    }

    // Compute clean up epilogue

    std::stringstream epilogue;

        // restore all dirty registers

    if (lrOnStack)
    {
        epilogue << SPACES "decw s8"          << std::endl;
        epilogue << SPACES "ldw r3, [s8,0]"   << std::endl;
        epilogue << SPACES "decw s8"          << std::endl;
        epilogue << SPACES "ldw r2, [s8,0]"   << std::endl;
        epilogue << SPACES "ldspr s1, r2"     << std::endl;
    }

    for (auto i = dirtyRegisters16.rbegin(); i != dirtyRegisters16.rend(); i++)
    {
        epilogue << SPACES "decw s8" << std::endl;
        epilogue << SPACES "ldw r" << *i << ", [s8, 0]" << std::endl;
    }

    epilogue << SPACES "ret" << std::endl;

    // Replace all instances of %EMIT_CLEAN_AND_RET% with clean up epilogue

    std::string bodyStr = functionBody.str();

    int pos;
    while ((pos = bodyStr.find("%EMIT_CLEAN_AND_RET%")) != std::string::npos)
    {
        bodyStr.replace(pos, 20, epilogue.str());
    }


    m_text << bodyStr;


    m_text << std::endl;


    m_inGlobalScope = true;

}

void CodeGenerator::generateVariableDeclaration(Syntax::VariableDeclaration* var)
{

    // Add variable to current scope

    // If current scope is global scope, declare variable and reserve space for it

    if (inGlobalScope())
    {
        // Deduce type of var

        Type t = generateType(var->vartype(), var->decorator());

        // Create variable declaration

        Variable v;

        v.type = t;
        v.name = var->name();

        // Calculate initial value for variable

        if (! reduceExpression(v, var->expression()))
        {
            std::stringstream errtext;
            errtext << "Initializer not constant, line " << var->linenum();
            throw std::runtime_error(errtext.str());
        }

        // Generate declaration of variable in the data section

        std::string varname = "g_";
        varname += v.name;

        v.asmName = varname;

        varname += ":";

        m_data << varname << std::endl;

        switch (v.type.type)
        {
            case BuiltInType::kInt:
                m_data << "\tdd 0x" << std::hex << (uint32_t)v.initialValue.i << std::endl;
                break;
            case BuiltInType::kShort:
                m_data << "\tdw 0x" << std::hex << (uint16_t)v.initialValue.i << std::endl;
                break;
        }



        // add to global scope.
        // TODO: Do this


    }
    else
    {
        // Else if current scope is local scope, examine decorator to see if variable is declared as
        // "static" or "auto" (default).

        // static : jump over declaration and leave space for variable in the function itself.


        // non-static: is there a register free which can hold the variable?

        //              - no free registers

    }


}

Type CodeGenerator::generateType(const Syntax::Type* type, const Syntax::DecoratorList* decorators)
{
    Type t;

    // Process decorators

    t.isStatic = false;
    t.isConst = false;
    t.isVolatile = false;

    if (decorators != nullptr)  // decorator list might be null.
        for (const Syntax::Decorator* d : decorators->decorators())
        {
            switch (d->decoratorType())
            {
                case Syntax::DecoratorType::Static:
                    t.isStatic = true;
                    break;
                case Syntax::DecoratorType::Const:
                    t.isConst = true;
                    break;
                case Syntax::DecoratorType::Volatile:
                    t.isVolatile = true;
                    break;
                default:
                    break;
            }
        }

    if (type->typeName() == "int")
    {
        t.type = BuiltInType::kInt;
    }
    else if (type->typeName() == "unsigned int")
    {
        t.type = BuiltInType::kUInt;
    }
    else if (type->typeName() == "short")
    {
        t.type = BuiltInType::kShort;
    }
    else if (type->typeName() == "unsigned short")
    {
        t.type = BuiltInType::kUShort;
    }
    else if (type->typeName() == "char")
    {
        t.type = BuiltInType::kChar;
    }
    else if (type->typeName() == "unsigned char")
    {
        t.type = BuiltInType::kUChar;
    }
    else if (type->typeName() == "long")
    {
        t.type = BuiltInType::kLong;
    }
    else if (type->typeName() == "unsigned long")
    {
        t.type = BuiltInType::kULong;
    }
    else
    {
        t.type = BuiltInType::kCustom;
    }

    t.name = type->typeName();

    return t;

}

bool CodeGenerator::reduceExpression(Variable& var, const Syntax::Syntagma* expression)
{
    // To reduce expression, first convert from AST representation to flattened representation, which is easier
    // to process according to operator precedence rules

    Expr::Expression e;
    Variable vout;

    e.fromSyntaxTree(expression);

    if (! e.reduceToConstant(vout)) return false;

    // check type of vout. If it doesn't match the type of var,
    // need to implicitly convert - which may or may not be possible

    switch (var.type.type)
    {
        case BuiltInType::kInt:
            switch (vout.type.type)
            {
                case BuiltInType::kInt:
                case BuiltInType::kShort:
                case BuiltInType::kChar:
                    var.initialValue.i = vout.initialValue.i;
                    break;
                default:
                    goto bail;
            }

            break;
        case BuiltInType::kShort:
            switch (vout.type.type)
            {
                case BuiltInType::kInt:
                case BuiltInType::kShort:
                case BuiltInType::kChar:
                    var.initialValue.i = vout.initialValue.i;
                    break;
                default:
                    goto bail;
            }
            break;
         default:
            goto bail;

    }

    return true;

bail:
    std::stringstream s;
    s << "No implicit conversion from " << vout.type << " to " << var.type;
    throw std::runtime_error(s.str());
}


void CodeGenerator::generateBlock(const Syntax::Block *b, std::stringstream &ss,
                                  bool &callsFunctions, bool& lastStatementIsReturn, int& scopeId)
{

    // Create a scope for this block

    std::stringstream scopeSs;
    scopeSs << "block" << scopeId;

    VariableStore::getInstance()->pushScope(scopeSs.str());

    // process each statement in the block in turn and handle it appropriately

    for (Syntax::Syntagma* statement : b->statements())
    {

        lastStatementIsReturn = false; // this will be set if we get a return statement

        switch (statement->type())
        {
            case Syntax::ElementType::ReturnStatement:
            {
                lastStatementIsReturn = true;

                Syntax::ReturnStatement* r = dynamic_cast<Syntax::ReturnStatement*>(statement);

                if (r == nullptr)
                    throw std::runtime_error("Could not cast return statement!");

                generateReturnStatement(r, ss, callsFunctions);

                break;
            }
            case Syntax::ElementType::AsmStatement:
            {

                Syntax::AsmStatement* a = dynamic_cast<Syntax::AsmStatement*>(statement);

                if (a == nullptr)
                    throw std::runtime_error("Could not cast to an asm statement!");

                generateAsmStatement(a, ss);

                break;

            }
            case Syntax::ElementType::FunctionCall:
            {
                Syntax::FunctionCall* f = dynamic_cast<Syntax::FunctionCall*>(statement);

                if (f == nullptr)
                    throw std::runtime_error("Could not cast to a function call!");

                generateFunctionCall(f, ss);

                callsFunctions = true;  // Need to set this so that the link register will be preserved in function
                                        // prologue for correct operation

                break;
            }
            default:
            {
                std::stringstream err;

                err << "Unsupported statement: " << (uint32_t)statement->type();
                throw std::runtime_error(err.str());
            }
        }

    }

    VariableStore::getInstance()->popScope();

}


void CodeGenerator::generateReturnStatement(const Syntax::ReturnStatement* r,
                                            std::stringstream& ss,
                                            bool& callsFunctions)
{
    // create the expression for the statement

    Expr::Expression e;
    e.fromSyntaxTree(r->expression());

    IntRep::IntRep i = e.generateIntRep();

    // TODO: Add support for 32 bit registers
    ss << IntRepCompiler::GenerateAssembly(i, m_registers16 /*, m_registers32*/, true);

    // restore all clobbered registers


    // emit sequence which will be replaced with final cleanup and ret instruction (which may be iret or eret)
    // once register layout is known.

    ss << "%EMIT_CLEAN_AND_RET%" << std::endl;

}

void CodeGenerator::generateAsmStatement(const Syntax::AsmStatement* a, std::stringstream& ss)
{

    // Process which registers are used

    //  - input registers

    const Syntax::RegisterList* inRegs = a->getInRegisters();

    std::vector<int> registers;

    for (Syntax::RegisterDescription* r : inRegs->getRegisterDescriptions())
    {
        //std::cout << "RegDesc:" << r->symbol() << " " << r->reg() << std::endl;

        if (r->reg() != "r")
            throw std::runtime_error("Inline assembly only supports register parameters");

        // find the register

        VariableStore::Var v = VariableStore::getInstance()->findVar(r->symbol());

        if (v == VariableStore::Var{0})
            throw std::runtime_error("Could not find parameter!");

        int reg = IntRepCompiler::FindRegForVar(m_registers16, v);

        registers.push_back(reg);
    }

    //  - clobber registers

    const Syntax::RegisterList* clobberRegs = a->getClobberRegisters();

    std::vector<int> clobberList;

    for (Syntax::RegisterDescription* r : clobberRegs->getRegisterDescriptions())
    {
        std::cout << "RegDesc:" << r->reg() << std::endl;

        Register reg = convertReg(const_cast<char*>(r->reg().c_str()));

        if (reg == Register::None)
            throw std::runtime_error("Could not parse register in inline asm clobber list!");

        if ((int)reg < 16)
            clobberList.push_back((int)reg);
        else
            throw std::runtime_error("Clobbering SPRs is not yet supported!");
    }


    // Preserve clobbered registers if not empty

    for (int reg : clobberList)
    {
        m_registers16.regs[reg].dirty = true;
    }

    // Set up inputs


    // massage asm: replace all "\n" with proper newline

    std::string asmStr = a->getAsm();

    int pos;
    while ((pos = asmStr.find("\\n")) != std::string::npos)
    {
        asmStr.replace(pos, 2, "\n" SPACES);
    }

    // replace input registers

    int i = 0;
    for (int reg : registers)
    {
        std::stringstream ss;
        ss << "%" << i;

        std::stringstream ss2;
        ss2 << "r" << reg;

        while ((pos = asmStr.find(ss.str())) != std::string::npos)
        {
            asmStr.replace(pos, 2, ss2.str());
        }

        i ++;
    }

    // Dump asm

    ss << SPACES << "//--- Begin inline asm" << std::endl << SPACES << asmStr << std::endl << SPACES "//--- End inline asm" << std::endl;

}

Register CodeGenerator::convertReg(char* reg)
{

    std::string s(reg);

    for (auto & c: s)
        c = tolower(c);

    if (s == "r0")
    {
        return Register::r0;
    }
    else if (s == "r1")
    {
        return Register::r1;
    }
    else if (s == "r2")
    {
        return Register::r2;
    }
    else if (s == "r3")
    {
        return Register::r3;
    }
    else if (s == "r4")
    {
        return Register::r4;
    }
    else if (s == "r5")
    {
        return Register::r5;
    }
    else if (s == "r6")
    {
        return Register::r6;
    }
    else if (s == "r7")
    {
        return Register::r7;
    }
    else if (s == "r8")
    {
        return Register::r8;
    }
    else if (s == "r9")
    {
        return Register::r9;
    }
    else if (s == "r10")
    {
        return Register::r10;
    }
    else if (s == "r11")
    {
        return Register::r11;
    }
    else if (s == "r12")
    {
        return Register::r12;
    }
    else if (s == "r13")
    {
        return Register::r13;
    }
    else if (s == "r14")
    {
        return Register::r14;
    }
    else if (s == "r15")
    {
        return Register::r15;
    }
    else if (s == "s0")
    {
        return Register::s0;
    }
    else if (s == "s1")
    {
        return Register::s1;
    }
    else if (s == "s2")
    {
        return Register::s2;
    }
    else if (s == "s3")
    {
        return Register::s3;
    }
    else if (s == "s4")
    {
        return Register::s4;
    }
    else if (s == "s5")
    {
        return Register::s5;
    }
    else if (s == "s6")
    {
        return Register::s6;
    }
    else if (s == "s7")
    {
        return Register::s7;
    }
    else if (s == "s8")
    {
        return Register::s8;
    }
    else if (s == "s9")
    {
        return Register::s9;
    }
    else if (s == "s10")
    {
        return Register::s10;
    }
    else if (s == "s11")
    {
        return Register::s11;
    }
    else if (s == "s12")
    {
        return Register::s12;
    }
    else if (s == "s13")
    {
        return Register::s13;
    }
    else if (s == "s14")
    {
        return Register::s14;
    }
    else if (s == "s15")
    {
        return Register::s15;
    }

    return Register::None;
}

void CodeGenerator::generateFunctionCall(const Syntax::FunctionCall* f, std::stringstream& ss)
{

    IntRep::IntRep i;

    Expr::Expression e;

    e.fromSyntaxTree(f);

    i = e.generateIntRep();

    ss << IntRepCompiler::GenerateAssembly(i, m_registers16, false);

}

