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
#include "labelstore.h"

#include "intrep.h"
#include "intrepcompiler.h"

#include <iostream>
#include <fstream>
#include <utility>

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

    std::stringstream ss;


    // start generation

    for (Syntax::Syntagma* s : m_toplevel->declarations())
    {
        generateCode(s);
    }

    // Emit prologue

    ss << "\t.org 0h"               << std::endl;
    ss                              << std::endl;
    ss << "_reset:" << std::endl << SPACES "jump    _start" << std::endl;
    ss << "             .align 4"   << std::endl;
    ss << "_interrupt:" << std::endl << SPACES  "nop"        << std::endl;
    ss << "             nop"        << std::endl;
    ss << "_exception:" << std::endl << SPACES  "nop"        << std::endl;
    ss << "             nop"        << std::endl;
    ss << "_svc:"        << std::endl << SPACES "nop"        << std::endl;
    ss << "             nop"        << std::endl;

    ss << std::endl << std::endl;

    ss << "_start:"                 << std::endl;

    ss << "             load r0, (_stack_start & 0x0000ffff)" << std::endl;
    ss << "             load r1, (_stack_start & 0xffff0000) >> 16" << std::endl;
    ss << "             ldspr s8, r0" << std::endl;

    ss << "             call f_main" << std::endl;

    ss << "_die:        " << std::endl << SPACES "sleep"      << std::endl;
    ss << "             jump _die"  << std::endl;

    ss << std::endl << std::endl;


    // Emit data, bss, and text sections

    ss << "// --------------- Text ---------------" << std::endl << std::endl;
    ss << m_text.str();

    ss << "// --------------- Data ---------------" << std::endl << std::endl;
    ss << m_data.str();

    ss << m_bss.str();

    // Emit stack

    ss << std::endl << std::endl;

    ss << "             .align 100h" << std::endl;
    ss << "_stack_start:"            << std::endl;
    ss << "             .times 100h dw 0x0" << std::endl;
    ss << "_stack_end:  "            << std::endl;

    // Emit epilogue

    ss << "             .end" << std::endl;

    // Write to file

    std::ofstream of;

    of.open(fname);

    of << ss.str();

    // Print to stdout

    std::cout << ss.str();

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

    const Syntax::Type* returnType = dynamic_cast<const Syntax::Type*>(func->returnType());

    Type rt = generateType(returnType, nullptr);

    f.returnType = rt;

    FunctionStore* fstore = FunctionStore::getInstance();

    fstore->declareFunction(f);

    m_text << funcname << ":" << std::endl;

    // Create a new scope for the function and put the parameters in it

    pushScope(funcname);

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

        VariableStore::Var var = VariableStore::getInstance()->declareVar(name, type);

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

    const Syntax::Block* body = func->body();

    generateBlock(body, functionBody, functionCallsFunctions, lastStatementIsReturn);

    // pop scope

    popScope();

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
        m_text << SPACES "stspr r0, s1" << std::endl;
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
                                  bool &callsFunctions, bool& lastStatementIsReturn)
{

    // Create a scope for this block

    pushScope("b");

    // process each statement in the block in turn and handle it appropriately

    for (Syntax::Syntagma* statement : b->statements())
    {
        generateStatement(statement, ss, callsFunctions, lastStatementIsReturn);
    }

    popScope();

}

void CodeGenerator::generateStatement(Syntax::Syntagma* statement, std::stringstream& ss,
                   bool& callsFunctions, bool& isReturn)
{

    isReturn = false;

    switch (statement->type())
    {
        case Syntax::ElementType::ReturnStatement:
        {
            isReturn = true;

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
        case Syntax::ElementType::IfStatement:
        {
            Syntax::IfStatement* i = dynamic_cast<Syntax::IfStatement*>(statement);

            if (i == nullptr)
                throw std::runtime_error("Could not cast to an if statement!");

            generateIfStatement(i, ss, callsFunctions);

            break;
        }
        case Syntax::ElementType::IfElseStatement:
        {
            Syntax::IfElseStatement* i = dynamic_cast<Syntax::IfElseStatement*>(statement);

            if (i == nullptr)
                throw std::runtime_error("Could not cast to an if else statement!");

            generateIfElseStatement(i, ss, callsFunctions);

            break;
        }
        case Syntax::ElementType::WhileStatement:
        {
            Syntax::WhileStatement* w = dynamic_cast<Syntax::WhileStatement*>(statement);

            if (w == nullptr)
                throw std::runtime_error("Could not cast to an if else statement!");

            generateWhileStatement(w, ss, callsFunctions);

            break;
        }
        case Syntax::ElementType::DoWhileStatement:
        {
            Syntax::DoWhileStatement* w = dynamic_cast<Syntax::DoWhileStatement*>(statement);

            if (w == nullptr)
                throw std::runtime_error("Could not cast to a DoWhile statement!");

            generateDoWhileStatement(w, ss, callsFunctions);

            break;
        }
        case Syntax::ElementType::ForLoop:
        {
            Syntax::ForLoop* l = dynamic_cast<Syntax::ForLoop*>(statement);

            if (l == nullptr)
                throw std::runtime_error("Could not cast to a ForLoop statement!");

            generateForLoopStatement(l, ss, callsFunctions);

            break;
        }
        case Syntax::ElementType::UnaryExpression:
        case Syntax::ElementType::BinaryExpression:
        {

            buildAndCheckStandaloneExpression(statement, ss, callsFunctions);

            break;
        }
        case Syntax::ElementType::Switch:
        {
            Syntax::Switch* s = dynamic_cast<Syntax::Switch*>(statement);

            if (s == nullptr)
                throw std::runtime_error("Could not cast to a Switch statement!");

            generateSwitchStatement(s, ss, callsFunctions);

            break;
        }
        case Syntax::ElementType::Default:
        case Syntax::ElementType::Case:
        {
            throw std::runtime_error("Default / Case outside of a switch statement");
        }
        case Syntax::ElementType::Break:
        {
            if (m_breakStack.size() == 0)
                throw std::runtime_error("Break statement outside of for / while / switch");

            LabelStore::Label l = m_breakStack.back();

            ss << SPACES "jump " << l->asmName << std::endl;

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

void CodeGenerator::generateIfStatement(const Syntax::IfStatement* i, std::stringstream& ss,
                             bool& callsFunctions)
{
    IntRep::IntRep ir;

    Expr::Expression e;

    e.fromSyntaxTree(i->getExpression());

    ir = e.generateIntRep();

    // Get "out" of int rep

    VariableStore::Var v = ir.getOutputVar();

    if (v == VariableStore::Var(0))
        throw std::runtime_error("Could not get 'out' of int rep");

    // Declare label to jump over block

    LabelStore::Label l = LabelStore::getInstance()->declareTempLab();

    // Generate test and jump to label

    IntRep::IntRep ir2;

    ir2.assimilate(ir);

    ir2.test(v, 0xffff);

    ir2.jumpZ(l);

    ss << IntRepCompiler::GenerateAssembly(ir2, m_registers16, false);

    // Generate block

    bool dummy;

    generateBlock(i->getBlock(), ss, callsFunctions, dummy);

    // Generate label at end of block

    ss << l->asmName << ":" << std::endl;

}

void CodeGenerator::generateIfElseStatement(const Syntax::IfElseStatement* i, std::stringstream& ss,
                                            bool& callsFunctions)
{

    IntRep::IntRep ir;

    Expr::Expression e;

    e.fromSyntaxTree(i->getExpression());

    ir = e.generateIntRep();

    // Get "out" of int rep

    VariableStore::Var v = ir.getOutputVar();

    if (v == VariableStore::Var(0))
        throw std::runtime_error("Could not get 'out' of int rep");

    // Declare label to jump over block and jump when done the if part

    LabelStore::Label l = LabelStore::getInstance()->declareTempLab();
    LabelStore::Label l2 = LabelStore::getInstance()->declareTempLab();

    // Generate test and jump to label

    IntRep::IntRep ir2;

    ir2.assimilate(ir);

    ir2.test(v, 0xffff);

    ir2.jumpZ(l);

    ss << IntRepCompiler::GenerateAssembly(ir2, m_registers16, false);

    // Generate block

    bool dummy;

    generateBlock(i->getIfBlock(), ss, callsFunctions, dummy);

    ss << SPACES << "jump " << l2->asmName << std::endl;

    // Generate label at end of block

    ss << l->asmName << ":" << std::endl;

    generateBlock(i->getElseBlock(), ss, callsFunctions, dummy);

    ss << l2->asmName << ":" << std::endl;

}

void CodeGenerator::pushScope(std::string scope)
{
    VariableStore::getInstance()->pushScope(scope);
    LabelStore::getInstance()->pushScope(scope);
}

void CodeGenerator::popScope()
{
    VariableStore::getInstance()->popScope();
    LabelStore::getInstance()->popScope();
}

void CodeGenerator::generateWhileStatement(const Syntax::WhileStatement* w, std::stringstream& ss,
                             bool& callsFunctions)
{
    IntRep::IntRep ir;

    Expr::Expression e;

    e.fromSyntaxTree(w->getExpression());

    ir = e.generateIntRep();

    // Get "out" of int rep

    VariableStore::Var v = ir.getOutputVar();

    if (v == VariableStore::Var(0))
        throw std::runtime_error("Could not get 'out' of int rep");

    // Declare label to jump over block

    LabelStore::Label l = LabelStore::getInstance()->declareTempLab();
    LabelStore::Label l2 = LabelStore::getInstance()->declareTempLab();

    // Generate test and jump to label

    IntRep::IntRep ir2;

    ir2.assimilate(ir);

    ir2.test(v, 0xffff);

    ir2.jumpZ(l);

    ss << l2->asmName << ":" << std::endl;

    ss << IntRepCompiler::GenerateAssembly(ir2, m_registers16, false);

    // Generate block

    bool dummy;

    generateBlock(w->getBlock(), ss, callsFunctions, dummy);

    ss << SPACES "jump " << l2->asmName << std::endl;

    // Generate label at end of block

    ss << l->asmName << ":" << std::endl;

}

void CodeGenerator::generateDoWhileStatement(const Syntax::DoWhileStatement* w, std::stringstream& ss,
                             bool& callsFunctions)
{
    IntRep::IntRep ir;

    Expr::Expression e;

    e.fromSyntaxTree(w->getExpression());

    ir = e.generateIntRep();

    // Get "out" of int rep

    VariableStore::Var v = ir.getOutputVar();

    if (v == VariableStore::Var(0))
        throw std::runtime_error("Could not get 'out' of int rep");

    // Declare label to jump over block

    LabelStore::Label l = LabelStore::getInstance()->declareTempLab();

    // Generate test and jump to label

    IntRep::IntRep ir2;

    ir2.assimilate(ir);

    ir2.test(v, 0xffff);

    ir2.jumpNZ(l);

    ss << l->asmName << ":" << std::endl;

    // Generate block

    bool dummy;

    generateBlock(w->getBlock(), ss, callsFunctions, dummy);

    ss << IntRepCompiler::GenerateAssembly(ir2, m_registers16, false);

}



void CodeGenerator::buildAndCheckStandaloneExpression(const Syntax::Syntagma* statement,
                                                      std::stringstream& ss,
                                                      bool& callFunctions)
{

        Expr::Expression e;

        e.fromSyntaxTree(statement);

        IntRep::IntRep i;

        i = e.generateIntRep();

        // Todo: check if the expression has any effect?

        ss << IntRepCompiler::GenerateAssembly(i, m_registers16, false);


}

void CodeGenerator::generateForLoopStatement(const Syntax::ForLoop* l, std::stringstream& ss,
                             bool& callsFunctions)
{

    // Generate initialiser

    buildAndCheckStandaloneExpression(l->getInitialiser(), ss, callsFunctions);

    // Generate label l1

    LabelStore::Label l1 = LabelStore::getInstance()->declareTempLab();
    LabelStore::Label l2 = LabelStore::getInstance()->declareTempLab();

    ss << l1->asmName << ":" << std::endl;

    // Generate condition

    Expr::Expression e;

    e.fromSyntaxTree(l->getCondition());

    IntRep::IntRep ir;

    ir = e.generateIntRep();

    VariableStore::Var v = ir.getOutputVar();

    if (v == VariableStore::Var(0))
        throw std::runtime_error("Could not get 'out' of int rep");

    IntRep::IntRep ir2;

    ir2.assimilate(ir);

    ir2.test(v, 0xffff);

    ir2.jumpZ(l2);

    ss << IntRepCompiler::GenerateAssembly(ir2, m_registers16, false);

    // Generate block

    bool dummy;

    generateBlock(l->getBlock(), ss, callsFunctions, dummy);

    // Generate incrementor

    Expr::Expression e2;

    e2.fromSyntaxTree(l->getIncrementor());

    IntRep::IntRep ir3;

    ir3 = e2.generateIntRep();

    ss << IntRepCompiler::GenerateAssembly(ir3, m_registers16, false);

    // Jump to l1

    ss << SPACES "jump " << l1->asmName << std::endl;

    // Generate label l2

    ss << l2->asmName << ":" << std::endl;
}

void CodeGenerator::generateSwitchStatement(const Syntax::Switch* s,
                                            std::stringstream&  ss,
                                            bool& callsFunctions)
{

    std::stringstream genCode;

    // Push final label (to break to) to "break stack"

    LabelStore::Label endSwitch = LabelStore::getInstance()->declareTempLab();

    m_breakStack.push_back(endSwitch);

    // Parse all statements in the switch block and deduce jump table.

    Syntax::Block* b = s->getBlock();

    std::vector<std::pair<LabelStore::Label, Syntax::Syntagma*>> labels;

    LabelStore::Label defaultLabel{0};

    bool dummy;

    for (Syntax::Syntagma* syn : b->statements())
    {
        switch (syn->type())
        {
            case Syntax::ElementType::Case:
            {
                Syntax::Case* cs = dynamic_cast<Syntax::Case*>(syn);

                if (cs == nullptr)
                    throw std::runtime_error("Could not cast to case!");

                LabelStore::Label l = LabelStore::getInstance()->declareTempLab();

                genCode << l->asmName << ":" << std::endl;

                labels.emplace_back(std::make_pair(l, cs->getExpression()));

                break;
            }
            case Syntax::ElementType::Default:
            {
                defaultLabel = LabelStore::getInstance()->declareTempLab();

                genCode << defaultLabel->asmName << ":" << std::endl;

                break;
            }
            default:
                generateStatement(syn, genCode, callsFunctions, dummy);

                break;
        }
    }

    // Generate jump table

    IntRep::IntRep ir;

    Expr::Expression e;

    e.fromSyntaxTree(s->getExpression());

    ir = e.generateIntRep();

    VariableStore::Var outv = ir.getOutputVar();

    IntRep::IntRep ir2;

    ir2.assimilate(ir);

        // for each case statement we found, evaluate the expression (todo: check that these reduce to
        // an integer constant

    for (auto p : labels)
    {
        Expr::Expression ee;
        ee.fromSyntaxTree(p.second);

        IntRep::IntRep ir3 = ee.generateIntRep();

        VariableStore::Var evalv = ir3.getOutputVar();

        ir2.assimilate(ir3);

        ir2.compare(outv, evalv);
        ir2.jumpZ(p.first);

        ir2.deleteTemporary(evalv);

    }

    if (defaultLabel != LabelStore::Label{0})
    {
        ir2.jump(defaultLabel);
    }
    else
    {
        ir2.jump(endSwitch);
    }

    ir2.deleteTemporary(outv);

    ss << IntRepCompiler::GenerateAssembly(ir2, m_registers16, false);

    ss << genCode.str();

    ss << endSwitch->asmName << ":" << std::endl;

    m_breakStack.pop_back();

}
