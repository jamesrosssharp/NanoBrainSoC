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
#include "expressionhelper.h"

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
            e.line_num = expression->linenum();

            m_elements.push_back(e);

            fromSyntaxTree(g->expression());

            e.elem = ElementType::kRightParenthesis;
            e.line_num = expression->linenum();

            m_elements.push_back(e);

            break;

        }
        case Syntax::ElementType::FunctionCall:
        {
            const Syntax::FunctionCall* f = dynamic_cast<const Syntax::FunctionCall*>(expression);

            ExpressionElement e;
            e.elem = ElementType::kFunctionCall;
            e.line_num = expression->linenum();

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
            e.line_num = expression->linenum();

            switch(imm->immType())
            {
                case ImmediateType::kInt:
                    e.type.type = BuiltInType::kInt;
                    e.v.sval = imm->intValue();
                    break;
                default:
                    std::cout << (int)imm->immType() << std::endl;
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
            e.line_num = expression->linenum();

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
            e.line_num = expression->linenum();

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
        case Syntax::BinaryExpressionType::BitwiseAnd:
            e.elem = ElementType::kAnd;
            break;
        case Syntax::BinaryExpressionType::BitwiseOr:
            e.elem = ElementType::kOr;
            break;
        case Syntax::BinaryExpressionType::BitwiseXor:
            e.elem = ElementType::kXor;
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
                case BuiltInType::kInt:
                case BuiltInType::kShort:
                case BuiltInType::kLong:
                case BuiltInType::kChar:
                    os << elem.v.sval;
                    break;
                case BuiltInType::kUInt:
                case BuiltInType::kUShort:
                case BuiltInType::kULong:
                case BuiltInType::kUChar:
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
        case BuiltInType::kInt:
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
    // call the recursive parser

    Expression e = _generateIntRep();

    // The returned expression *should* consist of a single element with all the generated
    // int rep stitched together

    ExpressionElement elem = e.getFinalElement();

    return elem.intRep;
}


Expression Expression::_generateIntRep()
{

    // Search for sub expressions and evaluate them

    Expression  e, sube;
    uint32_t     stackDepth = 0;
    Expression* curExp = &e;

    for (ExpressionElement& elem : m_elements)
    {
        switch (elem.elem)
        {
            case ElementType::kLeftParenthesis:
                if (stackDepth == 0)
                {
                    curExp = &sube;
                    sube.reset();
                }
                else
                {
                    curExp->addElement(elem);
                }
                stackDepth++;
                break;
            case ElementType::kRightParenthesis:
                if (stackDepth == 0)
                {
                    std::stringstream ss;
                    ss << "unbalanced parenthesis, on line " << elem.line_num << std::endl;
                    throw std::runtime_error(ss.str());
                }

                stackDepth --;

                if (stackDepth == 0)
                {
                    curExp = &e;

                    // Recursively evaluate the sube

                    Expression subeRes = sube._generateIntRep();

                    std::cout << subeRes << std::endl;

                    ExpressionElement& subeResElem = subeRes.getFinalElement();

                    curExp->addElement(subeResElem);

                }
                else
                {
                    curExp->addElement(elem);
                }

                break;
             default:
                curExp->addElement(elem);
        }
    }

    // Replace constants with intrep

    e = e.doImmediates();

    // Reduce function calls
    //
    //  TODO:
    //  If function calls appear on the other side of a || or && operator, we only evaluate them
    //  if the needed. Need to fix this. For the time being, evaluate all function calls
    //

    e = e.doFunctionCalls();

    // Evaluate unary operators

    // Evaluate binary operators

    e = e.doOp(ElementType::kPlus, ExpressionHelper::AddVar);
    e = e.doOp(ElementType::kMinus, ExpressionHelper::SubVar);
    e = e.doOp(ElementType::kShiftLeft, ExpressionHelper::ShlVar);
    e = e.doOp(ElementType::kShiftRight, ExpressionHelper::ShrVar);

    //< <= 	For relational operators < and ≤ respectively
    //> >= 	For relational operators > and ≥ respectively
    //== != 	For relational = and ≠ respectively

    //  & 	Bitwise AND
    e = e.doOp(ElementType::kAnd, ExpressionHelper::AndVar);
    // 	^ 	Bitwise XOR (exclusive or)
    e = e.doOp(ElementType::kXor, ExpressionHelper::XorVar);
    // 	|
    e = e.doOp(ElementType::kOr, ExpressionHelper::OrVar);


    // Return reduced expression


    return e;

}

ExpressionElement& Expression::getFinalElement()
{
    if (m_elements.size() != 1)
        throw std::runtime_error("Expression is not fully reduced!");

    return m_elements[0];
}

Expression  Expression::doOp(ElementType type,
                 std::function<ExpressionElement (ExpressionElement&, ExpressionElement&)> func)
{

    Expression e;
    bool foundOp = false;

    ElementType op = ElementType::kNone;

    ExpressionElement* left = nullptr;

    int idx = 1;

    for (ExpressionElement& mid : m_elements)
    {

        if (mid.elem == type)
        {
            foundOp = true;

            ExpressionElement* right;

            try {
                right = &m_elements.at(idx);
            }
            catch (std::out_of_range)
            {
                std::stringstream ss;
                ss << "Binary operator without RHS on line" << mid.line_num;
                throw std::runtime_error(ss.str());
            }

            if (left == nullptr)
            {
                std::stringstream ss;
                ss << "Binary operator without LHS on line" << mid.line_num;
                throw std::runtime_error(ss.str());
            }

            ExpressionElement out = func(*left, *right);

            e.addElement(out);

            for (int i = idx + 1; i < m_elements.size(); i ++)
                e.addElement(m_elements[i]);

            goto done;

        }
        else
        {
            if (left != nullptr)
                e.addElement(*left);
        }

        left = &mid;
        idx++;
    }

    if (left != nullptr)
        e.addElement(*left);

done:
    if (foundOp)
        return e.doOp(type, func);
    else
        return e;
}

Expression Expression::doFunctionCalls()
{
    Expression e;

    for (ExpressionElement& elem : m_elements)
    {
        if (elem.elem == ElementType::kFunctionCall)
        {
            ExpressionElement funcElem = ExpressionHelper::DoFunc(elem);

            e.addElement(funcElem);
        }
        else
        {
            e.addElement(elem);
        }
    }

    return e;

}

Expression Expression::doImmediates()
{
    Expression e;

    for (ExpressionElement& elem : m_elements)
    {
        if (elem.elem == ElementType::kLiteral)
        {
            ExpressionElement immElem = ExpressionHelper::DoImm(elem);
            e.addElement(immElem);
        }
        else
        {
            e.addElement(elem);
        }
    }

    return e;
}
