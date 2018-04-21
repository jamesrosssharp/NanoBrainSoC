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

#pragma once

#include "codegendefs.h"
#include "syntax.h"
#include "intrep.h"

#include "types.h"

#include <memory>
#include <functional>

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
        kSymbol,
        kTemporary,
        kIntRepPlaceHolder,
        kLiteralIntRepPlaceHolder,   // If it's a literal that's been compiled, we may need to coalesce as a
                                    // pre-optimisation step, so mark it as a separate entity
        kPostIncrement,
        kPostDecrement,
        kPreIncrement,
        kPreDecrement,

        kLessThan,
        kLessThanOrEqual,
        kGreaterThan,
        kGreaterThanOrEqual,
        kEqual,
        kNotEqual,

        kAssignment,

    };

    class Expression;

    struct ExpressionElement
    {

        ElementType          elem;
        CodeGen::Type        type;
        ValueUnion           v;

        std::string symbol;

        std::string functionName;
        std::vector<std::shared_ptr<Expression>> functionArguments;

        IntRep::IntRep intRep;

        int line_num;

        ExpressionElement()
        {
            elem = ElementType::kNone;
            v.sval = 0;
            line_num = 0;
        }

    };

    class Expression
    {
    public:

        void fromSyntaxTree(const Syntax::Syntagma* expression);

        bool reduceToConstant(CodeGen::Variable& vout);

        IntRep::IntRep generateIntRep();

        const std::vector<ExpressionElement>& elements() const { return m_elements; }

        void reset() { m_elements.clear(); }

        void addElement(ExpressionElement e) { m_elements.push_back(e); }

    private:

        void        convertExpressionType(ExpressionElement& e, Syntax::BinaryExpressionType type);
        void        convertUnaryExpressionType(ExpressionElement& e, Syntax::UnaryExpressionType type);

        bool        _reduceToConstant();
        Expression  _generateIntRep();

        Expression  doOp(ElementType op,
                         std::function<ExpressionElement (ExpressionElement&, ExpressionElement&)> func);

        Expression  doRightToLeftOp(ElementType op,
                         std::function<ExpressionElement (ExpressionElement&, ExpressionElement&)> func);

        Expression  doUnaryOp(ElementType op,
                         std::function<ExpressionElement (ExpressionElement&)> func);

        Expression doFunctionCalls();
        Expression doImmediates();
        Expression doSymbols();

        ExpressionElement& getFinalElement();

        uint32_t   parseCharLiteral(std::string literal);

        std::vector<ExpressionElement> m_elements;

    };


}

std::ostream& operator << (std::ostream& os, const Expr::ExpressionElement& elem);
std::ostream& operator << (std::ostream& os, const Expr::Expression& e);

