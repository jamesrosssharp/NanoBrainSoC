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

#include "expressionhelper.h"

Expr::ExpressionElement ExpressionHelper::AddVar(Expr::ExpressionElement& left, Expr::ExpressionElement& right)
{
    return GenericBinaryOp(left, right,
                        [] (IntRep::IntRep& intRep, CodeGen::Variable* left, CodeGen::Variable* right, CodeGen::Variable* out)
                        {
                            intRep.addVar(left, right, out);
                        });

}

Expr::ExpressionElement ExpressionHelper::GenericBinaryOp(Expr::ExpressionElement& left,
                                                          Expr::ExpressionElement& right,
                                        std::function<void (IntRep::IntRep& intrep,
                                                            CodeGen::Variable* left,
                                                            CodeGen::Variable* right, CodeGen::Variable* out)> genFunc)
{

    // Check type of left and right... can only add immediate numeric values with each other, with other variables,
    // or with temporaries.
    // Function calls should have previously been evaluated.

    Expr::ExpressionElement e;

    switch (left.elem)
    {
        case Expr::ElementType::kLiteral:

            switch (right.elem)
            {
                case Expr::ElementType::kLiteral:
                {
                    // Deduce type of literal - we can only operate on integral types and char literals
                    // otherwise error.

                    CodeGen::Variable* temp1 = e.intRep.declareTemporary();
                    CodeGen::Variable* temp2 = e.intRep.declareTemporary();
                    CodeGen::Variable* temp3 = e.intRep.declareTemporary();

                    e.intRep.loadImm(temp1, left.v.sval);   // TODO: These depend on type
                    e.intRep.loadImm(temp2, right.v.sval);

                    genFunc(e.intRep, temp1, temp2, temp3);

                    e.intRep.deleteTemporary(temp1);
                    e.intRep.deleteTemporary(temp2);

                    e.intRep.output(temp3);

                    break;
                }
                default:
                    throw std::runtime_error("Cannot add literal (left) to element (right)");
            }

            break;
        case Expr::ElementType::kSymbol:

            // TODO: we need to know the types of all the symbols. Possibly all variables need to be looked up
            // (using a callback mechanism) when the expression is created from the syntax tree.

            throw std::runtime_error("TODO: Add support for adding symbols");

            break;
        case Expr::ElementType::kTemporary:

            throw std::runtime_error("TODO: Add support for adding temporaries");

            break;
        default:
            throw std::runtime_error("Cannot add elements");
    }

    return e;


}


