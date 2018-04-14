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

#include "variablestore.h"
#include "functionstore.h"

Expr::ExpressionElement ExpressionHelper::AddVar(Expr::ExpressionElement& left, Expr::ExpressionElement& right)
{
    return GenericBinaryOp(left, right,
                        [] (IntRep::IntRep& intRep, VariableStore::Var& left, VariableStore::Var& right, VariableStore::Var& out)
                        {
                            intRep.addVar(left, right, out);
                        });

}

Expr::ExpressionElement ExpressionHelper::SubVar(Expr::ExpressionElement& left, Expr::ExpressionElement& right)
{
    return GenericBinaryOp(left, right,
                        [] (IntRep::IntRep& intRep, VariableStore::Var& left, VariableStore::Var& right, VariableStore::Var& out)
                        {
                            intRep.subVar(left, right, out);
                        });

}

Expr::ExpressionElement ExpressionHelper::ShlVar(Expr::ExpressionElement& left, Expr::ExpressionElement& right)
{
    return ShiftOp(left, right, true);
}

Expr::ExpressionElement ExpressionHelper::ShrVar(Expr::ExpressionElement& left, Expr::ExpressionElement& right)
{
    return ShiftOp(left, right, false);
}

Expr::ExpressionElement ExpressionHelper::AndVar(Expr::ExpressionElement& left, Expr::ExpressionElement& right)
{
    return GenericBinaryOp(left, right,
                        [] (IntRep::IntRep& intRep, VariableStore::Var& left, VariableStore::Var& right, VariableStore::Var& out)
                        {
                            intRep.andVar(left, right, out);
                        });

}

Expr::ExpressionElement ExpressionHelper::OrVar(Expr::ExpressionElement& left, Expr::ExpressionElement& right)
{
    return GenericBinaryOp(left, right,
                        [] (IntRep::IntRep& intRep, VariableStore::Var& left, VariableStore::Var& right, VariableStore::Var& out)
                        {
                            intRep.orVar(left, right, out);
                        });

}

Expr::ExpressionElement ExpressionHelper::XorVar(Expr::ExpressionElement& left, Expr::ExpressionElement& right)
{
    return GenericBinaryOp(left, right,
                        [] (IntRep::IntRep& intRep, VariableStore::Var& left, VariableStore::Var& right, VariableStore::Var& out)
                        {
                            intRep.xorVar(left, right, out);
                        });

}

Expr::ExpressionElement ExpressionHelper::GenericBinaryOp(Expr::ExpressionElement& left,
                                                          Expr::ExpressionElement& right,
                                        std::function<void (IntRep::IntRep& intrep,
                                                            VariableStore::Var& left,
                                                            VariableStore::Var& right, VariableStore::Var& out)> genFunc)
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

                    VariableStore::Var temp1 = e.intRep.declareTemporary();
                    VariableStore::Var temp2 = e.intRep.declareTemporary();
                    VariableStore::Var temp3 = e.intRep.declareTemporary();

                    e.intRep.loadImm(temp1, left.v.sval);   // TODO: These depend on type
                    e.intRep.loadImm(temp2, right.v.sval);

                    genFunc(e.intRep, temp1, temp2, temp3);

                    e.intRep.deleteTemporary(temp1);
                    e.intRep.deleteTemporary(temp2);

                    e.intRep.output(temp3);

                    e.elem = Expr::ElementType::kIntRepPlaceHolder;

                    break;
                }
                case Expr::ElementType::kIntRepPlaceHolder:
                {

                    e.intRep.assimilate(right.intRep);

                    VariableStore::Var temp2 = right.intRep.getOutputVar();

                    if (temp2 == VariableStore::Var(0))
                        throw std::runtime_error("Could not link int rep!");

                    VariableStore::Var temp1 = e.intRep.declareTemporary();
                    VariableStore::Var temp3 = e.intRep.declareTemporary();

                    e.intRep.loadImm(temp1, left.v.sval);

                    genFunc(e.intRep, temp1, temp2, temp3);

                    e.intRep.deleteTemporary(temp1);
                    e.intRep.deleteTemporary(temp2);

                    e.intRep.output(temp3);

                    e.elem = Expr::ElementType::kIntRepPlaceHolder;

                    break;
                }
                default:
                    printf("Expr::ElementType: %d\n", (int)right.elem);
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
        case Expr::ElementType::kIntRepPlaceHolder:
        case Expr::ElementType::kLiteralIntRepPlaceHolder:
        {
            switch (right.elem)
            {
                case Expr::ElementType::kLiteral:
                {
                    e.intRep.assimilate(left.intRep);

                    VariableStore::Var temp1 = left.intRep.getOutputVar();

                    if (temp1 == VariableStore::Var(0))
                        throw std::runtime_error("Could not link int rep!");

                    VariableStore::Var temp2 = e.intRep.declareTemporary();
                    VariableStore::Var temp3 = e.intRep.declareTemporary();

                    e.intRep.loadImm(temp2, right.v.sval);

                    genFunc(e.intRep, temp1, temp2, temp3);

                    e.intRep.deleteTemporary(temp1);
                    e.intRep.deleteTemporary(temp2);

                    e.intRep.output(temp3);

                    e.elem = Expr::ElementType::kIntRepPlaceHolder;

                    break;
                }
                case Expr::ElementType::kIntRepPlaceHolder:
                case Expr::ElementType::kLiteralIntRepPlaceHolder:
                {
                    e.intRep.assimilate(left.intRep);
                    e.intRep.assimilate(right.intRep);

                    VariableStore::Var temp1 = left.intRep.getOutputVar();
                    VariableStore::Var temp2 = right.intRep.getOutputVar();

                    if (temp1 == VariableStore::Var(0))
                        throw std::runtime_error("Could not link int rep!");

                    if (temp2 == VariableStore::Var(0))
                        throw std::runtime_error("Could not link int rep!");

                    VariableStore::Var temp3 = e.intRep.declareTemporary();

                    genFunc(e.intRep, temp1, temp2, temp3);

                    e.intRep.deleteTemporary(temp1);
                    e.intRep.deleteTemporary(temp2);

                    e.intRep.output(temp3);

                    e.elem = Expr::ElementType::kIntRepPlaceHolder;

                    break;
                }

            }

            break;
        }
        default:
            throw std::runtime_error("Cannot add elements");
    }

    return e;


}

Expr::ExpressionElement ExpressionHelper::DoFunc(Expr::ExpressionElement& expr)
{
    FunctionStore* fstore = FunctionStore::getInstance();

    FunctionStore::FunctionHandle fhandle = fstore->findFunctionByName(expr.functionName);

    if (fhandle == 0)   // TODO: Generate proper error messages
        throw std::runtime_error("Undefined function!");

    CodeGen::Function* f = fstore->getFunction(fhandle);

    Expr::ExpressionElement e;

    VariableStore::Var temp1 = e.intRep.declareTemporary(); // TODO: Declare with type from function

    // Create variables for functions arguments

    std::vector<VariableStore::Var> args; // TODO: Fill this in

    for (std::shared_ptr<Expr::Expression> argExp : expr.functionArguments)
    {
        IntRep::IntRep i = argExp->generateIntRep();

        VariableStore::Var v = i.getOutputVar();

        e.intRep.assimilate(i);

        args.push_back(v);

    }

    e.intRep.genFunctionCall(temp1, fhandle, args);

    e.intRep.output(temp1);
    e.elem = Expr::ElementType::kIntRepPlaceHolder;

    return e;

}

Expr::ExpressionElement ExpressionHelper::DoImm(Expr::ExpressionElement& elem)
{
    Expr::ExpressionElement e;

    VariableStore::Var temp1 = e.intRep.declareTemporary(); // TODO: Declare with type from function

    e.intRep.loadImm(temp1, elem.v.uval);
    e.intRep.output(temp1);

    e.elem            = Expr::ElementType::kLiteralIntRepPlaceHolder;
    e.v.uval          = elem.v.uval;

    return e;
}

Expr::ExpressionElement ExpressionHelper::ShiftOp(Expr::ExpressionElement& left, Expr::ExpressionElement& right, bool shiftLeft)
{
    Expr::ExpressionElement e;

    // For the moment, only allow a shift by a literal, which will translate directly to a barrel shift
    // instruction.

    switch (right.elem)
    {
        case Expr::ElementType::kLiteralIntRepPlaceHolder:

            switch (left.elem)
            {
                // TODO: literal case should be computed by the compiler
                case Expr::ElementType::kIntRepPlaceHolder:
                case Expr::ElementType::kLiteralIntRepPlaceHolder:
                {

                    e.intRep.assimilate(left.intRep);

                    VariableStore::Var temp1 = left.intRep.getOutputVar();

                    if (temp1 == VariableStore::Var(0))
                        throw std::runtime_error("Could not link int rep!");

                    VariableStore::Var temp2 = e.intRep.declareTemporary();

                    // TODO: check sign?
                    if (right.v.uval > 15)
                        throw std::runtime_error("Cannot shift more than 15 bits");

                    if (shiftLeft)
                        e.intRep.bslVar(temp1, right.v.uval, temp2);
                    else
                        e.intRep.bsrVar(temp1, right.v.uval, temp2);

                    e.intRep.deleteTemporary(temp1);

                    e.intRep.output(temp2);

                    e.elem = Expr::ElementType::kIntRepPlaceHolder;

                    break;
                }
            }

            break;
        default:
            throw std::runtime_error("Unsupported type!");
    }

    return e;
}
