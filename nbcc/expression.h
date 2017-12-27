#pragma once

#include "codegendefs.h"
#include "syntax.h"
#include "intrep.h"

#include <memory>

namespace Expr
{

    enum class ElementType : uint32_t
    {
        kNone = 0,
        kLeftParenthesis,
        kRightParenthesis,
        kPlus,
        kMinus,
        kDiv,
        kMult,
        kShiftLeft,
        kShiftRight,
        kAnd,
        kOr,
        kXor,
        kUnaryNegation,
        kUnaryLogicalNot,
        kUnaryBitwiseNot,
        kFunctionCall,
        kLiteral,
        kSymbol
    };

    class Expression;

    struct ExpressionElement
    {

        ElementType          elem;
        CodeGen::Type        type;
        union
        {
            char* string;
            unsigned int uval;
            int     sval;
            float   fval;
        } v;

        std::string symbol;

        std::string functionName;
        std::vector<std::shared_ptr<Expression>> functionArguments;

    };

    class Expression
    {
    public:

        void fromSyntaxTree(const Syntax::Syntagma* expression);

        bool reduceToConstant(CodeGen::Variable& vout);

        IntRep::IntRep generateIntRep();

        const std::vector<ExpressionElement>& elements() const { return m_elements; }

    private:

        void convertExpressionType(ExpressionElement& e, Syntax::BinaryExpressionType type);
        bool _reduceToConstant();

        std::vector<ExpressionElement> m_elements;

    };


}

std::ostream& operator << (std::ostream& os, const Expr::ExpressionElement& elem);
std::ostream& operator << (std::ostream& os, const Expr::Expression& e);

