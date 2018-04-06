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

std::ostream& operator << (std::ostream& os, const IntRep::Element& e)
{
    switch (e.element)
    {
        case IntRep::ElementType::kAdd:
            if (e.v1 != nullptr && e.v2 != nullptr && e.v3 != nullptr)
                os << "ADD " << *e.v1 << " " << *e.v2 << " " << *e.v3 << std::endl;
            break;
        case IntRep::ElementType::kDeclareTemporary:
            if (e.v1 != nullptr)
                os << "DECL " << *e.v1 << std::endl;
            break;
        case IntRep::ElementType::kDeleteTemporary:
            if (e.v1 != nullptr)
                os << "DEL " << *e.v1 << std::endl;
            break;
        case IntRep::ElementType::kLoadImm:
            if (e.v1 != nullptr)
                os << "LOADIMM " << *e.v1 << " " << e.immval << std::endl;
            break;
        case IntRep::ElementType::kOutput:
            if (e.v1 != nullptr)
                os << "OUT " << *e.v1 << std::endl;
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


void IntRep::IntRep::addVar(CodeGen::Variable* left, CodeGen::Variable* right, CodeGen::Variable* out)
{

}

CodeGen::Variable* IntRep::IntRep::declareTemporary()
{

    std::stringstream ss;
    ss << "temp" << m_tempvarIdx++;
    std::string tvarName = ss.str();

    CodeGen::Variable var;
    var.name = tvarName;
    var.asmName = tvarName;
    m_temporaries.push_back(var);

    Element e;
    e.element = ElementType::kDeclareTemporary;
    e.v1 = &m_temporaries.back();

    std::cout << *e.v1 << std::endl;

    m_elements.push_back(e);

    std::cout << m_elements.back() << std::endl;

    return &m_temporaries.back();

}

void IntRep::IntRep::deleteTemporary(CodeGen::Variable* temp)
{

}

void IntRep::IntRep::loadImm(CodeGen::Variable* var, int32_t intval)
{
    // For now, make all immediates that come here signed shorts
    // TODO: parser must determine type associated with an immediate

    ImmediateValue v;
    v.type = ImmediateType::kShort;
    v.v.sval = intval;

    loadImm(var, v);

}

void IntRep::IntRep::loadImm(CodeGen::Variable* var, uint32_t intval)
{
    // For now, make all immediates that come here unsigned shorts
    ImmediateValue v;
    v.type = ImmediateType::kUShort;
    v.v.uval = intval;

    loadImm(var, v);
}

void IntRep::IntRep::loadImm(CodeGen::Variable* var, ImmediateValue& v)
{
    Element e;
    e.element = ElementType::kLoadImm;
    e.immval = v;
    e.v1 = var;
    m_elements.push_back(e);
}


void IntRep::IntRep::output(CodeGen::Variable* out)
{

}



