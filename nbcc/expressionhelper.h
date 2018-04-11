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
#include "intrep.h"
#include "expression.h"

#include <functional>

namespace ExpressionHelper
{

    // Generate a new expression element, allocating a temporary variable to hold the sum
    // of the two expression elements
    Expr::ExpressionElement AddVar(Expr::ExpressionElement& left, Expr::ExpressionElement& right);
    Expr::ExpressionElement SubVar(Expr::ExpressionElement& left, Expr::ExpressionElement& right);

    Expr::ExpressionElement DoFunc(Expr::ExpressionElement& expr);
    Expr::ExpressionElement DoImm(Expr::ExpressionElement& expr);

    Expr::ExpressionElement GenericBinaryOp(Expr::ExpressionElement& left, Expr::ExpressionElement& right,
                                            std::function<void (IntRep::IntRep& intrep, VariableStore::Var& left,
                                                                VariableStore::Var& right,
                                                                VariableStore::Var& out)> genFunc);

}
