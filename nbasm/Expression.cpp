#include "Expression.h"
#include "AST.h"
#include <sstream>
#include <iostream>

bool Expression::evaluate(int32_t &value, SymbolTable &syms)
{

    //
    //  Expressions are like the following:
    //
    // ((1 + 2) * 7 + 3 ) / 8
    //
    // They have been tokenised by the lexer and so processing them
    // is as follows:
    //
    // - scan for sub-expressions and recursively evaluate

    Expression e, sube;
    uint32_t            stackDepth = 0;
    Expression* curExp = &e;

    for (ExpressionElement& elem : elements)
    {

        switch (elem.elem)
        {
            case ExpressionElementType::kLeftParenthesis:
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
            case ExpressionElementType::kRightParenthesis:
                if (stackDepth == 0)
                {
                    std::cout << elem << std::endl;

                    std::stringstream ss;
                    ss << "unbalanced parenthesis, on line " << lineNum << std::endl;
                    throw std::runtime_error(ss.str());
                }

                stackDepth --;

                if (stackDepth == 0)
                {
                    curExp = &e;
                    int32_t subValue = 0;
                    if (! sube.evaluate(subValue, syms))
                        return false;
                    else
                    {
                        ExpressionElement subElem;
                        subElem.elem = ExpressionElementType::kInt;
                        subElem.v.sval = subValue;
                        e.addElement(subElem);
                    }
                }
                else
                {
                    curExp->addElement(elem);
                }

                break;
            case ExpressionElementType::kString:
            {
                // substitute symbols

                Symbol* sym = nullptr;

                try {
                    sym = &syms.at(elem.v.string);
                }
                catch (std::out_of_range)
                {
                    std::stringstream ss;
                    ss << "undefined symbol" << elem.v.string << ", on line " << lineNum << std::endl;
                    throw std::runtime_error(ss.str());
                }

                if (! sym->evaluted)
                    return false;

                ExpressionElement symElem;
                symElem.elem = ExpressionElementType::kInt;
                symElem.v.sval = sym->value;
                e.addElement(symElem);

                break;
            }
            default:
                curExp->addElement(elem);
                break;
        }

    }


    // - recursively reduce expression (TODO: operator precedence?)

        // mult, div

    e = e.doMultDiv();


        // shl, shr


        // and, or

        // not

        // unary plus / minus

    // Expression now contains integers, plusses and minuses. Simply accumulate from left to right


    int32_t accu = 0;

    ExpressionElementType op = ExpressionElementType::kPlus;

    for (ExpressionElement& elem : e.elements)
    {

        switch (elem.elem)
        {
            case ExpressionElementType::kPlus:
            case ExpressionElementType::kMinus:
                op = elem.elem;
                break;
            case ExpressionElementType::kInt:
                accu = doPlusMinus(accu, op, elem.v.sval);
                break;
            case ExpressionElementType::kUInt:
                accu = doPlusMinus(accu, op, elem.v.uval);
                break;
            case ExpressionElementType::kCharLiteral:
                accu = doPlusMinus(accu, op, (int32_t)elem.v.string[0]);
                break;
            default:
                std::stringstream ss;
                ss << "unexpected element, on line " << lineNum << std::endl;
                throw std::runtime_error(ss.str());
                break;
        }


    }


    value = accu;

    return true;
}

int32_t Expression::doPlusMinus(int32_t accu, ExpressionElementType& op, int32_t value)
{
    switch(op)
    {
        case ExpressionElementType::kNone:
        {
            std::stringstream ss;
            ss << "operator expected on line " << lineNum << std::endl;
            throw std::runtime_error(ss.str());
            break;
        }
        case ExpressionElementType::kPlus:
            accu = accu + value;
            break;
        case ExpressionElementType::kMinus:
            accu = accu - value;
            break;
    }
    op = ExpressionElementType::kNone;
    return accu;
}


Expression Expression::doMultDiv()
{
    // Recursively replace multiplications and divisions, left to right, with their evaluated values

    Expression e;
    bool foundMultDiv = false;

    ExpressionElementType op = ExpressionElementType::kNone;

    ExpressionElement* left = nullptr;

    int idx = 1;

    for (ExpressionElement& mid : elements)
    {

        switch (mid.elem)
        {
            case ExpressionElementType::kMult:
            case ExpressionElementType::kDiv:
            {
                foundMultDiv = true;

                ExpressionElement* right;

                try {
                    right = &elements.at(idx);
                }
                catch (std::out_of_range)
                {
                    std::stringstream ss;
                    ss << "multiplication / division without right operand on line " << lineNum << std::endl;
                    throw std::runtime_error(ss.str());
                }

                int32_t lval, rval;

                switch (left->elem)
                {
                    case ExpressionElementType::kInt:
                        lval = left->v.sval;
                        break;
                    case ExpressionElementType::kUInt:
                        lval = left->v.uval;
                        break;
                    case ExpressionElementType::kCharLiteral:
                        lval = (int32_t)left->v.string[0];
                        break;
                    default:
                        std::stringstream ss;
                        ss << "unexpected element, on line " << lineNum << std::endl;
                        throw std::runtime_error(ss.str());
                        break;
                }

                switch (right->elem)
                {
                    case ExpressionElementType::kInt:
                        rval = right->v.sval;
                        break;
                    case ExpressionElementType::kUInt:
                        rval = right->v.uval;
                        break;
                    case ExpressionElementType::kCharLiteral:
                        rval = (int32_t)right->v.string[0];
                        break;
                    default:
                        std::stringstream ss;
                        ss << "unexpected element, on line " << lineNum << std::endl;
                        throw std::runtime_error(ss.str());
                        break;
                }

                int32_t value = mid.elem == ExpressionElementType::kMult ? lval * rval : lval / rval;

                ExpressionElement result;

                result.v.sval = value;
                result.elem = ExpressionElementType::kInt;

                e.addElement(result);

                // Add in rest of elements to expression

                for (int i = idx + 1; i < elements.size(); i++)
                    e.addElement(elements[i]);

                goto done;
            }
            default:
                if (left != nullptr)
                    e.addElement(*left);
                break;
        }

        left = &mid;
        idx ++;
    }

    if (left != nullptr)
        e.addElement(*left);

done:
    if (foundMultDiv)
        return e.doMultDiv();
    else
        return e;

}
