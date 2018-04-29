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


Expr::ExpressionElement ExpressionHelper::LessThanVar(Expr::ExpressionElement& left, Expr::ExpressionElement& right)
{
    return GenericBinaryOp(left, right,
                        [] (IntRep::IntRep& intRep, VariableStore::Var& left, VariableStore::Var& right, VariableStore::Var& out)
                        {
                            intRep.lessThanVar(left, right, out);
                        });

}

Expr::ExpressionElement ExpressionHelper::LessThanEqualVar(Expr::ExpressionElement& left, Expr::ExpressionElement& right)
{
    return GenericBinaryOp(left, right,
                        [] (IntRep::IntRep& intRep, VariableStore::Var& left, VariableStore::Var& right, VariableStore::Var& out)
                        {
                            intRep.lessThanEqualVar(left, right, out);
                        });

}

Expr::ExpressionElement ExpressionHelper::GreaterThanVar(Expr::ExpressionElement& left, Expr::ExpressionElement& right)
{
    return GenericBinaryOp(left, right,
                        [] (IntRep::IntRep& intRep, VariableStore::Var& left, VariableStore::Var& right, VariableStore::Var& out)
                        {
                            intRep.greaterThanVar(left, right, out);
                        });

}

Expr::ExpressionElement ExpressionHelper::GreaterThanEqualVar(Expr::ExpressionElement& left, Expr::ExpressionElement& right)
{
    return GenericBinaryOp(left, right,
                        [] (IntRep::IntRep& intRep, VariableStore::Var& left, VariableStore::Var& right, VariableStore::Var& out)
                        {
                            intRep.greaterThanEqualVar(left, right, out);
                        });

}

Expr::ExpressionElement ExpressionHelper::EqualVar(Expr::ExpressionElement& left, Expr::ExpressionElement& right)
{
    return GenericBinaryOp(left, right,
                        [] (IntRep::IntRep& intRep, VariableStore::Var& left, VariableStore::Var& right, VariableStore::Var& out)
                        {
                            intRep.equalVar(left, right, out);
                        });

}

Expr::ExpressionElement ExpressionHelper::NotEqualVar(Expr::ExpressionElement& left, Expr::ExpressionElement& right)
{
    return GenericBinaryOp(left, right,
                        [] (IntRep::IntRep& intRep, VariableStore::Var& left, VariableStore::Var& right, VariableStore::Var& out)
                        {
                            intRep.notEqualVar(left, right, out);
                        });
}

Expr::ExpressionElement ExpressionHelper::SimpleAssignment(Expr::ExpressionElement& left, Expr::ExpressionElement& right)
{
    return GenericBinaryOp(left, right,
                        [] (IntRep::IntRep& intRep, VariableStore::Var& left, VariableStore::Var& right, VariableStore::Var& out)
                        {
                            intRep.assignSymbol(left, right, out);
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
            throw std::runtime_error("Cannot combine elements");
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

    for (auto a : args)
        e.intRep.deleteTemporary(a);

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

Expr::ExpressionElement ExpressionHelper::DoSym(Expr::ExpressionElement& elem)
{
    Expr::ExpressionElement e;

    VariableStore::Var temp1 = e.intRep.declareTemporary(); // TODO: Declare with type from function

    VariableStore::Var sym = VariableStore::getInstance()->findVar(elem.symbol);

    if (sym == VariableStore::Var{0})
        throw std::runtime_error("Could not find variable!");

    e.intRep.loadSym(temp1, sym);
    e.intRep.output(temp1);

    e.elem            = Expr::ElementType::kIntRepPlaceHolder;
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

Expr::ExpressionElement ExpressionHelper::PostDecVar(Expr::ExpressionElement& expr)
{
    return GenericUnaryOp(expr, []
                          (IntRep::IntRep& intRep, VariableStore::Var& v, VariableStore::Var& out)
                            {
                                intRep.postDecVar(v, out);

                            });
}

Expr::ExpressionElement ExpressionHelper::PostIncVar(Expr::ExpressionElement& expr)
{
    return GenericUnaryOp(expr, [] (IntRep::IntRep& intRep, VariableStore::Var& v, VariableStore::Var& out)
                            { intRep.postIncVar(v, out); });
}

Expr::ExpressionElement ExpressionHelper::PreDecVar(Expr::ExpressionElement& expr)
{
    return GenericUnaryOp(expr, [] (IntRep::IntRep& intRep, VariableStore::Var& v, VariableStore::Var& out)
                            { intRep.preDecVar(v, out); });
}

Expr::ExpressionElement ExpressionHelper::PreIncVar(Expr::ExpressionElement& expr)
{
    return GenericUnaryOp(expr, [] (IntRep::IntRep& intRep, VariableStore::Var& v, VariableStore::Var& out)
                            { intRep.preIncVar(v, out); });
}

Expr::ExpressionElement ExpressionHelper::GenericUnaryOp(Expr::ExpressionElement& left,
                                        std::function<void (IntRep::IntRep& intrep, VariableStore::Var& left,
                                                            VariableStore::Var& out)> genFunc)
{

    Expr::ExpressionElement e;

    e.elem = Expr::ElementType::kIntRepPlaceHolder;

    e.intRep.assimilate(left.intRep);

    VariableStore::Var v = left.intRep.getOutputVar();

    VariableStore::Var out = e.intRep.declareTemporary();

    genFunc(e.intRep, v, out);

    return e;

}

Expr::ExpressionElement ExpressionHelper::LogicalAndVar(Expr::ExpressionElement& left, Expr::ExpressionElement& right)
{
    Expr::ExpressionElement e;

    e.elem = Expr::ElementType::kIntRepPlaceHolder;

    e.intRep.assimilate(left.intRep);

    VariableStore::Var v1 = left.intRep.getOutputVar();
    VariableStore::Var v2 = right.intRep.getOutputVar();

    LabelStore::Label l1 = LabelStore::getInstance()->declareTempLab();
    LabelStore::Label l2 = LabelStore::getInstance()->declareTempLab();

    VariableStore::Var out = e.intRep.declareTemporary();

    e.intRep.test(v1, 0xffff);
    e.intRep.jumpZ(l1);

    e.intRep.assimilate(right.intRep);

    e.intRep.test(v2, 0xffff);
    e.intRep.jumpZ(l1);

    e.intRep.loadImm(out, 1);
    e.intRep.jump(l2);

    e.intRep.labelDec(l1);
    e.intRep.loadImm(out, 0);

    e.intRep.labelDec(l2);

    e.intRep.deleteTemporary(v1);
    e.intRep.deleteTemporary(v2);

    e.intRep.output(out);

    return e;

}

Expr::ExpressionElement ExpressionHelper::LogicalOrVar(Expr::ExpressionElement& left, Expr::ExpressionElement& right)
{
    Expr::ExpressionElement e;

    e.elem = Expr::ElementType::kIntRepPlaceHolder;

    e.intRep.assimilate(left.intRep);

    VariableStore::Var v1 = left.intRep.getOutputVar();
    VariableStore::Var v2 = right.intRep.getOutputVar();

    LabelStore::Label l1 = LabelStore::getInstance()->declareTempLab();
    LabelStore::Label l2 = LabelStore::getInstance()->declareTempLab();

    VariableStore::Var out = e.intRep.declareTemporary();

    e.intRep.test(v1, 0xffff);
    e.intRep.jumpNZ(l1);

    e.intRep.assimilate(right.intRep);

    e.intRep.test(v2, 0xffff);
    e.intRep.jumpNZ(l1);

    e.intRep.loadImm(out, 0);
    e.intRep.jump(l2);

    e.intRep.labelDec(l1);
    e.intRep.loadImm(out, 1);

    e.intRep.labelDec(l2);

    e.intRep.deleteTemporary(v1);
    e.intRep.deleteTemporary(v2);

    e.intRep.output(out);

    return e;
}

Expr::ExpressionElement ExpressionHelper::LogicalNotVar(Expr::ExpressionElement& expr)
{
    return GenericUnaryOp(expr, [] (IntRep::IntRep& intRep, VariableStore::Var& v, VariableStore::Var& out)
                            { intRep.logicalNotVar(v, out); });
}

Expr::ExpressionElement ExpressionHelper::BitwiseNotVar(Expr::ExpressionElement& expr)
{
    return GenericUnaryOp(expr, [] (IntRep::IntRep& intRep, VariableStore::Var& v, VariableStore::Var& out)
                            { intRep.bitwiseNotVar(v, out); });
}

Expr::ExpressionElement ExpressionHelper::NegVar(Expr::ExpressionElement& expr)
{
    return GenericUnaryOp(expr, [] (IntRep::IntRep& intRep, VariableStore::Var& v, VariableStore::Var& out)
                            { intRep.negVar(v, out); });
}
