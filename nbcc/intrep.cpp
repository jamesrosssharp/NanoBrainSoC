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

#include "intrep.h"
#include "codegendefs.h"
#include "variablestore.h"
#include "functionstore.h"

std::ostream& operator << (std::ostream& os, const IntRep::Element& e)
{
    switch (e.element)
    {
        case IntRep::ElementType::kAdd:
            os << "ADD " << *e.v1 << " " << *e.v2 << " " << *e.v3;
            break;
        case IntRep::ElementType::kDeclareTemporary:
            os << "DECL " << *e.v1;
            break;
        case IntRep::ElementType::kDeleteTemporary:
            os << "DEL " << *e.v1;
            break;
        case IntRep::ElementType::kLoadImm:
            os << "LOADIMM " << *e.v1 << " " << e.immval;
            break;
        case IntRep::ElementType::kOutput:
            os << "OUT " << *e.v1;
            break;
        case IntRep::ElementType::kPushFunctionArg:
            os << "PUSH_ARG " << *e.v1;
            break;
        case IntRep::ElementType::kCallFunction:
            os << "CALL " << *e.f1;
            break;
    }

    return os;
}

std::ostream& operator << (std::ostream& os, const IntRep::IntRep& i)
{
    for (const IntRep::Element& e: i.m_elements)
    {
        os << e << std::endl;

    }
    return os;
}


void IntRep::IntRep::addVar(const VariableStore::Var& left, const VariableStore::Var& right, const VariableStore::Var& out)
{

    Element e;

    e.element = ElementType::kAdd;
    e.v1 = left;
    e.v2 = right;
    e.v3 = out;

    m_elements.push_back(e);

}

VariableStore::Var IntRep::IntRep::declareTemporary()
{

    Element e;
    e.element = ElementType::kDeclareTemporary;

    CodeGen::Type t;

    t.type = BuiltInType::kInt; // Todo: this depends on what we're declaring...

    VariableStore* v = VariableStore::getInstance();

    e.v1 = VariableStore::Var(v->declareTemporary(t));

    m_elements.push_back(e);

    return e.v1;
}

void IntRep::IntRep::deleteTemporary(const VariableStore::Var& temp)
{

    Element e;
    e.element = ElementType::kDeleteTemporary;

    e.v1 = temp;

    m_elements.push_back(e);

}

void IntRep::IntRep::loadImm(const VariableStore::Var& var, int32_t intval)
{
    // For now, make all immediates that come here signed shorts
    // TODO: parser must determine type associated with an immediate

    ImmediateValue v;
    v.type = ImmediateType::kShort;
    v.v.sval = intval;

    loadImm(var, v);
}

void IntRep::IntRep::loadImm(const VariableStore::Var& var, uint32_t intval)
{
    // For now, make all immediates that come here unsigned shorts
    ImmediateValue v;
    v.type = ImmediateType::kUShort;
    v.v.uval = intval;

    loadImm(var, v);
}

void IntRep::IntRep::loadImm(const VariableStore::Var& var, ImmediateValue& v)
{
    Element e;
    e.element = ElementType::kLoadImm;
    e.immval = v;
    e.v1 = var;
    m_elements.push_back(e);
}


void IntRep::IntRep::output(const VariableStore::Var& out)
{
    Element e;
    e.element = ElementType::kOutput;
    e.v1 = out;
    m_elements.push_back(e);
}


void IntRep::IntRep::genFunctionCall(const VariableStore::Var& out, const FunctionStore::FunctionHandle f, const std::vector<VariableStore::Var>& args)
{

    // Push all arguments

    for (const VariableStore::Var& v : args)
    {
        Element e;
        e.element = ElementType::kPushFunctionArg;
        e.v1 = v;
        m_elements.push_back(e);
    }

    Element e;
    e.v1 = out;
    e.f1 = FunctionStore::Func(f);
    e.element = ElementType::kCallFunction;
    m_elements.push_back(e);

}
