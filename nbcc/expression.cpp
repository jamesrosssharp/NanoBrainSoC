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

    - Expression is responsible for reconstructing evaluable expressions from
      AST expressions

    - It can emit an intermediate representation of an expression, reduced as
      much as possible, which can be compiled to assembly language in the
      CodeGenerator.


=============================================================================*/

#include "expression.h"

using namespace Expr;

void Expression::fromSyntaxTree(const Syntax::Syntagma* expression)
{
    // Convert from Tree representation of an expression to a flattened, linear
    // representation, which we can reduce using left to right evaluation.

    switch (expression->type())
    {
        case Syntax::ElementType::GroupedExpression:
        {
            const Syntax::GroupedExpression* g = dynamic_cast<const Syntax::GroupedExpression*>(expression);

            ExpressionElement e;
            e.elem = ElementType::kLeftParenthesis;

            m_elements.push_back(e);

            fromSyntaxTree(g->expression());

            e.elem = ElementType::kRightParenthesis;

            m_elements.push_back(e);

            break;

        }
        case Syntax::ElementType::FunctionCall:
        {
            const Syntax::FunctionCall* f = dynamic_cast<const Syntax::FunctionCall*>(expression);

            ExpressionElement e;
            e.elem = ElementType::kFunctionCall;

            e.functionName = f->functionName();

            const Syntax::FunctionArguments* a = f->arguments();

            for (const Syntax::Syntagma* s : a->arguments())
            {
                std::shared_ptr<Expression> exp = std::make_shared<Expression>();
                exp->fromSyntaxTree(s);
                e.functionArguments.push_back(exp);
            }

            m_elements.push_back(e);

            break;
        }
        case Syntax::ElementType::Immediate:
        {
            const Syntax::Immediate* imm = dynamic_cast<const Syntax::Immediate*>(expression);

            ExpressionElement e;
            e.elem = ElementType::kLiteral;

            switch(imm->immType())
            {
                case Syntax::ImmediateType::Int:
                    e.type.type = CodeGen::BuiltInType::kInt;
                    e.v.sval = imm->intValue();
                    break;
                default:
                    throw std::runtime_error("Unknown immediate type!");
            }

            m_elements.push_back(e);

            break;
        }
        case Syntax::ElementType::Symbol:
        {
            const Syntax::Symbol* sym = dynamic_cast<const Syntax::Symbol*>(expression);

            ExpressionElement e;

            e.elem = ElementType::kSymbol;

            e.symbol = sym->symbol();

            m_elements.push_back(e);

            break;
        }
        case Syntax::ElementType::BinaryExpression:
        {
            // With a binary expression, recurse down the left side, add the operator,
            // then recurse down the right side

            const Syntax::BinaryExpression* be = dynamic_cast<const Syntax::BinaryExpression*>(expression);

            if (be == nullptr)
                throw std::runtime_error("Cannot cast binary expression!");

            fromSyntaxTree(be->left());

            // create the element corresponding to this node

            ExpressionElement e;

            convertExpressionType(e, be->binaryExpressionType());

            m_elements.push_back(e);

            // recurse right

            fromSyntaxTree(be->right());

            break;
        }
        //case Syntax::ElementType::UnaryExpression:
            // with a unary expression

        //    break;
        //case Syntax::ElementType::FunctionCall:

        //    break;
        default:
        {
            std::stringstream s;
            s << "Unknown item in expression! " << (uint32_t)expression->type();
            throw std::runtime_error(s.str());
        }
    }
}

void Expression::convertExpressionType(ExpressionElement& e, Syntax::BinaryExpressionType type)
{
    switch(type)
    {
        case Syntax::BinaryExpressionType::Addition:
            e.elem = ElementType::kPlus;
            break;
        case Syntax::BinaryExpressionType::Subtraction:
            e.elem = ElementType::kMinus;
            break;
        case Syntax::BinaryExpressionType::ShiftLeft:
            e.elem = ElementType::kShiftLeft;
            break;
    }
}


std::ostream& operator << (std::ostream& os, const ExpressionElement& elem)
{

    switch(elem.elem)
    {
        case ElementType::kLeftParenthesis:
            os << " ( ";
            break;
        case ElementType::kRightParenthesis:
            os << " ) ";
            break;
        case ElementType::kPlus:
            os << " + ";
            break;
        case ElementType::kMinus:
            os << " - ";
            break;
        case ElementType::kDiv:
            os << " / ";
            break;
        case ElementType::kMult:
            os << " * ";
            break;
        case ElementType::kShiftLeft:
            os << " << ";
            break;
        case ElementType::kShiftRight:
            os << " >> ";
            break;
        case ElementType::kAnd:
            os << " & ";
            break;
        case ElementType::kOr:
            os << " | ";
            break;
        case ElementType::kXor:
            os << " ^ ";
            break;
        case ElementType::kUnaryLogicalNot:
            os << " ! ";
            break;
        case ElementType::kUnaryBitwiseNot:
            os << " ~ ";
            break;
        case ElementType::kLiteral:
        {
            switch (elem.type.type)
            {
                case CodeGen::BuiltInType::kInt:
                case CodeGen::BuiltInType::kShort:
                case CodeGen::BuiltInType::kLong:
                case CodeGen::BuiltInType::kChar:
                    os << elem.v.sval;
                    break;
                case CodeGen::BuiltInType::kUInt:
                case CodeGen::BuiltInType::kUShort:
                case CodeGen::BuiltInType::kULong:
                case CodeGen::BuiltInType::kUChar:
                    os << elem.v.uval;
                    break;
            }
        }
        case ElementType::kSymbol:
        {
            os << elem.symbol;
            break;
        }
        case ElementType::kFunctionCall:
        {
            os << elem.functionName;

            os << '(';
            for (const std::shared_ptr<Expression>& e : elem.functionArguments)
                os << *e << ',';
            os << ')';
            break;
        }
        default:
            break;
    }

    return os;
}

std::ostream& operator << (std::ostream& os, const Expression& e)
{
    for (ExpressionElement elem : e.elements())
    {
        os << elem;
    }
    return os;
}

bool Expression::reduceToConstant(CodeGen::Variable& vout)
{
    // Reduce expression. If we hit any function calls or variables, return false, since the expression
    // is not a constant in that case. Note: in C99 or C11 even const variables cannot be used to declare
    // other variables in global scope.

    Expression e = *this; // make a copy.

    if (! e._reduceToConstant()) return false;

    if (e.elements().size() != 1) return false;

    if (e.elements()[0].elem != ElementType::kLiteral) return false;

    vout.type = e.m_elements[0].type;

    switch (e.m_elements[0].type.type)
    {
        case CodeGen::BuiltInType::kInt:
            vout.initialValue.i = e.m_elements[0].v.sval;
            break;
        default:
            throw std::runtime_error("Unsupported type!");
    }

    return true;
}

bool Expression::_reduceToConstant()
{


    return true;
}


IntRep::IntRep Expression::generateIntRep()
{
    return {};
}
