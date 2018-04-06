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
        kTemporary
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
        bool        _reduceToConstant();
        Expression  _generateIntRep();

        Expression  doOp(ElementType op,
                         std::function<ExpressionElement (ExpressionElement&, ExpressionElement&)> func);

        ExpressionElement& getFinalElement();

        std::vector<ExpressionElement> m_elements;

    };


}

std::ostream& operator << (std::ostream& os, const Expr::ExpressionElement& elem);
std::ostream& operator << (std::ostream& os, const Expr::Expression& e);

