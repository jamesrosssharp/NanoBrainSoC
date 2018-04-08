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

using namespace CodeGen;

CodeGenerator::CodeGenerator(Syntax::Syntagma* toplevel)
{

    m_toplevel = dynamic_cast<Syntax::TopLevel*>(toplevel);

    if (m_toplevel == nullptr)
        throw std::runtime_error("toplevel is not a Top Level!\n");

    m_inGlobalScope = true;

}


void CodeGenerator::generate()
{

    std::ostream& of = std::cout;

    // start generation

    for (Syntax::Syntagma* s : m_toplevel->declarations())
    {
        generateCode(s);
    }

    // Emit prologue

    of << "\t.org 0h"               << std::endl;
    of                              << std::endl;
    of << "_reset:      jump    _start" << std::endl;
    of << "             .align 4"   << std::endl;
    of << "_interrupt:  nop"        << std::endl;
    of << "             nop"        << std::endl;
    of << "_exception:  nop"        << std::endl;
    of << "             nop"        << std::endl;
    of << "_svc:        nop"        << std::endl;
    of << "             nop"        << std::endl;

    of << std::endl << std::endl;

    of << "_start:"                 << std::endl;

    of << "             load r0, (_stack_end & 0x0000ffff)" << std::endl;
    of << "             load r1, (_stack_end & 0xffff0000) >> 16" << std::endl;
    of << "             ldspr s8, r0" << std::endl;

    of << "             call f_main" << std::endl;

    of << "_die:        sleep"      << std::endl;
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

        // Todo: set up frame pointer

    if (functionCallsFunctions)
    {
        // TODO: Preserve link register on stack
    }

    // examine registers and preserve all registers above r8 that have been used

    m_text << functionBody.str();

    // Generate function epilogue. If this function calls any other function we have to
    // restore the link register.

    if (functionCallsFunctions)
    {
        // TODO: restore link register
    }

    // final return

    if (! lastStatementIsReturn)
    {
        // Check return type. If it not "void", generate an error / warning

        m_text << "             ret" << std::endl;
    }

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


    // emit ret instruction

    ss << "             ret" << std::endl;

}

