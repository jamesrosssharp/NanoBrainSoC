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

    Intermediate representation, used for compiling expressions into
    a form which can be optimised and then compiled.

    Each block of intrep must have a single output at the end, usually
    the last element. This allows segments of intrep to be chained together,
    forming larger programs. These will eventually be omptimised and then compiled.


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

void IntRep::IntRep::subVar(const VariableStore::Var& left, const VariableStore::Var& right, const VariableStore::Var& out)
{

    Element e;

    e.element = ElementType::kSub;
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

    Element e;
    e.v1 = out;
    e.f1 = FunctionStore::Func(f);
    e.args = args;
    e.element = ElementType::kCallFunction;
    m_elements.push_back(e);

}

VariableStore::Var IntRep::IntRep::getOutputVar()
{
    // Walk backward and find first output and return the var.

    for (auto e = m_elements.rbegin(); e != m_elements.rend(); e++)
    {
        auto elem = *e;

        if (elem.element == ElementType::kOutput)
        {
            return elem.v1;
        }

    }

    return {0};
}

void IntRep::IntRep::assimilate(const IntRep& i)
{
    for (auto e : i.m_elements)
    {
        if (e.element == ElementType::kOutput)
            continue;

        m_elements.push_back(e);
    }
}

