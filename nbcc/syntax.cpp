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


=============================================================================*/

#include "syntax.h"

using namespace Syntax;

std::ostream& operator << (std::ostream& os, const Syntax::Syntagma& s)
{
    s.print(os, 0);
    return os;
}

void Syntax::PrintIndent(std::ostream& os, int indent)
{
    for (int i = 0; i < indent; i++)
    {
        os << "\t";
    }
}

void Immediate::print(std::ostream &os, int indent) const
{
    PrintIndent(os, indent);

    os << "IMMEDIATE ";

    switch (m_immType)
    {
        case ImmediateType::Int:
            os << " INT " << m_value.i;
            break;
        case ImmediateType::Uint:
            os << " UINT " << m_value.u;
            break;
        case ImmediateType::Float:
            os << " FLOAT " << m_value.f;
            break;
        case ImmediateType::CharLiteral:
            os << " CHAR " << m_string;
            break;
        case ImmediateType::StringLiteral:
            os << " STRING " << m_string;
            break;
    }

    os << std::endl;
}

void Variable::print(std::ostream &os, int indent) const
{
    PrintIndent(os, indent);

    os << "VARIABLE ";
    os << m_symbol;
    os << std::endl;

    m_varType->print(os, indent + 1);
}

void Symbol::print(std::ostream &os, int indent) const
{
    PrintIndent(os, indent);

    os << "SYMBOL ";
    os << m_symbol;
    os << std::endl;
}

void UnaryExpression::print(std::ostream &os, int indent) const
{
    PrintIndent(os, indent);

    os << "UNARY_EXPRESSION ";
    switch (m_expressionType)
    {
        case UnaryExpressionType::BitwiseNot:
            os << " ~ ";
            break;
        case UnaryExpressionType::LogicalNot:
            os << " ! ";
            break;
        case UnaryExpressionType::Negation:
            os << " - ";
            break;
    }

    os << std::endl;

    m_expression->print(os, indent + 1);

}

void BinaryExpression::print(std::ostream &os, int indent) const
{
    PrintIndent(os, indent);

    os << "BINARY_EXPRESSION ";
    switch (m_expressionType)
    {
        case BinaryExpressionType::Addition:
            os << " + ";
            break;
        case BinaryExpressionType::BitwiseAnd:
            os << " & ";
            break;
        case BinaryExpressionType::BitwiseOr:
            os << " | ";
            break;
        case BinaryExpressionType::BitwiseXor:
            os << " ^ ";
            break;
        case BinaryExpressionType::Divide:
            os << " / ";
            break;
        case BinaryExpressionType::Equal:
            os << " == ";
            break;
        case BinaryExpressionType::GreaterThan:
            os << " > ";
            break;
        case BinaryExpressionType::GreaterThanOrEqual:
            os << " >= ";
            break;
        case BinaryExpressionType::LessThan:
            os << " < ";
            break;
        case BinaryExpressionType::LessThanOrEqual:
            os << " <= ";
            break;
        case BinaryExpressionType::LogicalAnd:
            os << " && ";
            break;
        case BinaryExpressionType::LogicalOr:
            os << " || ";
            break;
        case BinaryExpressionType::Multiply:
            os << " * ";
            break;
        case BinaryExpressionType::Subtraction:
            os << " - ";
            break;
        case BinaryExpressionType::ShiftLeft:
            os << " << ";
            break;
        case BinaryExpressionType::ShiftRight:
            os << " << ";
            break;
    }

    os << std::endl;

    m_left->print(os, indent + 1);
    m_right->print(os, indent + 1);

}

void Assignment::print(std::ostream &os, int indent) const
{
    PrintIndent(os, indent);

    os << "ASSIGNMENT ";
    os << m_var << " ";
    os << std::endl;

    m_expression->print(os, indent + 1);
}

void ReturnStatement::print(std::ostream &os, int indent) const
{
    PrintIndent(os, indent);

    os << "RETURN_STATEMENT ";
    os << std::endl;

    m_expression->print(os, indent + 1);
}

void FunctionArguments::print(std::ostream &os, int indent) const
{
    PrintIndent(os, indent);

    os << "FUNCTION_ARGUMENTS ";
    os << std::endl;

    for (Syntagma* s : m_arguments)
        s->print(os, indent + 1);
}

void FunctionDeclaration::print(std::ostream &os, int indent) const
{
    PrintIndent(os, indent);

    os << "FUNCTION_DECLARATION " << m_functionName;
    os << std::endl;

    m_returnType->print(os, indent + 1);

    if (m_attribute)
        m_attribute->print(os, indent + 1);

    m_arguments->print(os, indent + 1);

    m_body->print(os, indent + 1);

}

void FunctionCall::print(std::ostream &os, int indent) const
{
    PrintIndent(os, indent);

    os << "FUNCTION_CALL ";
    os << m_functionName;
    os << std::endl;

    m_arguments->print(os, indent + 1);
}

void Block::print(std::ostream &os, int indent) const
{
    PrintIndent(os, indent);

    os << "BLOCK" << std::endl;

    for (Syntagma* s : m_statements)
        s->print(os, indent + 1);
}

void IfStatement::print(std::ostream &os, int indent) const
{
    PrintIndent(os, indent);

    os << "IF_STATEMENT " << std::endl;

    m_condition->print(os, indent + 1);
    m_block->print(os, indent + 1);
}

void VariableDeclaration::print(std::ostream &os, int indent) const
{
    PrintIndent(os, indent);

    os << "VARIABLE_DECLARATION ";

    os << m_decorator << " ";
    os << m_varname   << " ";

    os << std::endl;

    if (m_vartype != nullptr)
        m_vartype->print(os, indent + 1);

    if (m_vartype != nullptr)
        m_expression->print(os, indent + 1);

}

void WhileStatement::print(std::ostream &os, int indent) const
{
    PrintIndent(os, indent);

    os << "WHILE_STATEMENT" << std::endl;

    m_condition->print(os, indent + 1);
    m_block->print(os, indent + 1);

}

void TopLevel::print(std::ostream &os, int indent) const
{
    PrintIndent(os, indent);

    os << "TOP_LEVEL" << std::endl;

    for (Syntagma* s : m_declarations)
        if (s) s->print(os, indent + 1);
}

void Type::print(std::ostream &os, int indent) const
{
    PrintIndent(os, indent);

    os << "TYPE " << m_typeName << std::endl;
}

void Attribute::print(std::ostream &os, int indent) const
{
    PrintIndent(os, indent);

    os << "ATTRIBUTE " << m_attrib << std::endl;
}

void Decorator::print(std::ostream& os, int indent) const
{
    PrintIndent(os, indent);

    switch (m_decoratorType)
    {
        case DecoratorType::Auto:
            os << "AUTO" << std::endl;
            break;
        case DecoratorType::Const:
            os << "CONST" << std::endl;
            break;
        case DecoratorType::Static:
            os << "STATIC" << std::endl;
            break;
        case DecoratorType::Volatile:
            os << "VOLATILE" << std::endl;
            break;
    }
}

void DecoratorList::print(std::ostream &os, int indent) const
{
     PrintIndent(os, indent);

     os << "DECORATORS" << std::endl;

     for (Decorator* d : m_decorators)
         if (d != nullptr)
             d->print(os, indent + 1);

}

void RegisterDescription::print(std::ostream &os, int indent) const
{
    PrintIndent(os, indent);

    os << "REGISTER " << m_reg << "(" << m_symbol << ")" << std::endl;
}

void RegisterList::print(std::ostream &os, int indent) const
{
    PrintIndent(os, indent);

    os << "REGISTER LIST" << std::endl;

    for (RegisterDescription* r : m_registers)
        if (r != nullptr)
            r->print(os, indent + 1);
}

void AsmStatement::print(std::ostream &os, int indent) const
{
    PrintIndent(os, indent);

    os << "ASM STATEMENT" << std::endl;

    if (m_dl != nullptr)
        m_dl->print(os, indent + 1);
    if (m_stringLiteral != nullptr)
        m_stringLiteral->print(os, indent + 1);
    if (m_out != nullptr)
        m_out->print(os, indent + 1);
    if (m_in != nullptr)
        m_in->print(os, indent + 1);
    if (m_clobbers != nullptr)
        m_clobbers->print(os, indent + 1);
}
