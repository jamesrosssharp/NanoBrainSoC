#include "parsehandlers.h"

#include "syntax.h"
#include "syntaxstack.h"

extern SyntaxStack g_syntaxStack;

void handleNewTopLevel()
{
    Syntax::TopLevel* tl;
    if (g_syntaxStack.empty())
    {
        tl = new Syntax::TopLevel();
    }
    else if (g_syntaxStack.peek()->type() != Syntax::ElementType::TopLevel)
    {
        tl = new Syntax::TopLevel();
    }
    else
    {
        tl = dynamic_cast<Syntax::TopLevel*>(g_syntaxStack.pop());
        if (tl == nullptr)
            throw std::runtime_error("Could not cast top level!");
    }

    tl->append(g_syntaxStack.pop());
    g_syntaxStack.push(tl);
}

void handleFunctionDeclaration(char* name)
{
    if (g_syntaxStack.empty())
        throw std::runtime_error("Syntax stack empty trying to create a block");

    // block
    Syntax::Block* s = dynamic_cast<Syntax::Block*>(g_syntaxStack.pop());

    if (s == nullptr)
        throw std::runtime_error("Function without a block!");

    // arguments
    Syntax::FunctionArguments* arg = dynamic_cast<Syntax::FunctionArguments*>(g_syntaxStack.pop());

    if (arg == nullptr)
        throw std::runtime_error("Could not retrieve arguments from stack!");
    // return type
    Syntax::Type* type = dynamic_cast<Syntax::Type*>(g_syntaxStack.pop());

    if (type == nullptr)
        throw std::runtime_error("Could not retrieve type from stack!");

    g_syntaxStack.push(new Syntax::FunctionDeclaration(name, nullptr, type, arg, s));

}

void handleArguments1()
{
    if (g_syntaxStack.empty())
        throw std::runtime_error("Stack empty!");

    Syntax::Syntagma* s = g_syntaxStack.pop();

    if (s->type() != Syntax::ElementType::Variable)
        throw std::runtime_error("Expected variable!");

    Syntax::FunctionArguments* a = dynamic_cast<Syntax::FunctionArguments*>(g_syntaxStack.pop());

    if (a == nullptr)
        throw std::runtime_error("No argument list on stack!");

    a->append(s);

    g_syntaxStack.push(a);

}

void handleArguments2()
{
    if (g_syntaxStack.empty())
        throw std::runtime_error("Stack empty!");

    Syntax::Syntagma* s = g_syntaxStack.pop();

    if (s->type() != Syntax::ElementType::Variable)
        throw std::runtime_error("Expected variable!");

    Syntax::FunctionArguments* a = new Syntax::FunctionArguments();
    a->append(s);

    g_syntaxStack.push(a);
}

void handleArguments3()
{
    Syntax::FunctionArguments* a = new Syntax::FunctionArguments();

    g_syntaxStack.push(a);
}

void handleVariable(char* name)
{
    Syntax::Type* type = dynamic_cast<Syntax::Type*>(g_syntaxStack.pop());

    if (type == nullptr)
        throw std::runtime_error("Expected type");

    Syntax::Variable* v = new Syntax::Variable(type, name);

    g_syntaxStack.push(v);

}

void handleBlock1()
{
    Syntax::Syntagma* statement = g_syntaxStack.pop();

    Syntax::Block* b;

    if (g_syntaxStack.peek()->type() != Syntax::ElementType::Block)
    {
        b = new (std::nothrow) Syntax::Block();
    }
    else
    {
        b = dynamic_cast<Syntax::Block*>(g_syntaxStack.pop());
    }

    if (b == nullptr)
        throw std::runtime_error("Could not get a block!");

    b->append(statement);

    g_syntaxStack.push(b);

}

void handleBlock2()
{
    Syntax::Block* subblock = dynamic_cast<Syntax::Block*>(g_syntaxStack.pop());

    if (subblock == nullptr)
        throw std::runtime_error("Could not cast subblock");

    Syntax::Block* b;

    if (g_syntaxStack.peek()->type() != Syntax::ElementType::Block)
    {
        b = new (std::nothrow) Syntax::Block();
    }
    else
    {
        b = dynamic_cast<Syntax::Block*>(g_syntaxStack.pop());
    }

    if (b == nullptr)
        throw std::runtime_error("Could not get a block!");

    b->append(subblock);

    g_syntaxStack.push(b);
}

void handleBlock3()
{
    Syntax::Block* b;

    b = new (std::nothrow) Syntax::Block();

    g_syntaxStack.push(b);
}

void handleReturnStatement()
{

    Syntax::Syntagma* expression = g_syntaxStack.pop();

    Syntax::ReturnStatement* ret = new Syntax::ReturnStatement(expression);

    g_syntaxStack.push(ret);

}

void handleAssignment(char* symbol)
{
    Syntax::Syntagma* expression = g_syntaxStack.pop();

    // to do
    Syntax::Assignment* assy = new Syntax::Assignment(symbol, expression);

    g_syntaxStack.push(assy);

}

void handleDeclaration(char* symbol)
{
    Syntax::Syntagma* expression = g_syntaxStack.pop();

    Syntax::Type* type = dynamic_cast<Syntax::Type*>(g_syntaxStack.pop());
    if (type == nullptr)
        throw std::runtime_error("Could not pop type from syntax stack!");

    // TODO: Add additional declarations which specify the decorator
    Syntax::DecoratorList* deco = new Syntax::DecoratorList;
    Syntax::Decorator*     d    = new Syntax::Decorator(Syntax::DecoratorType::Auto);
    deco->append(d);

    Syntax::VariableDeclaration* varDec = new Syntax::VariableDeclaration(symbol, type, deco, expression);

    g_syntaxStack.push(varDec);

}

void handleWhile1()
{
    Syntax::Syntagma* block = g_syntaxStack.pop();

    Syntax::Syntagma* expr = g_syntaxStack.pop();

    Syntax::WhileStatement* w = new Syntax::WhileStatement(expr, block);

    g_syntaxStack.push(w);
}

void handleWhile2()
{
    Syntax::Syntagma* block = new Syntax::Block();

    Syntax::Syntagma* expr = g_syntaxStack.pop();

    Syntax::WhileStatement* w = new Syntax::WhileStatement(expr, block);

    g_syntaxStack.push(w);
}

void handleExpressionSymbol(char* symbol)
{
   // variable

   Syntax::Symbol* sym = new Syntax::Symbol(symbol);

   g_syntaxStack.push(sym);
}

void handleExpressionIntImmediate(int immVal)
{
    // integer literal

    Syntax::Immediate* imm = new Syntax::Immediate(immVal);

    g_syntaxStack.push(imm);
}

void pushType(const char *type)
{
    Syntax::Type* t = new Syntax::Type(type);
    g_syntaxStack.push(t);
}

void handleExpressionCharLiteral(char* charLit)
{
    // char literal

    Syntax::Immediate* imm = new Syntax::Immediate(charLit, true);

    g_syntaxStack.push(imm);
}

void handleExpressionAddition()
{
    // addition
    handleExpressionArith(Syntax::BinaryExpressionType::Addition);
}

void handleExpressionArith(Syntax::BinaryExpressionType type)
{
    Syntax::Syntagma* right = g_syntaxStack.pop();
    Syntax::Syntagma* left = g_syntaxStack.pop();

    Syntax::BinaryExpression* b = new Syntax::BinaryExpression(type, left, right);

    g_syntaxStack.push(b);

}

void handleExpressionFunctionCall(char* symbol)
{
    // Function call

    Syntax::FunctionArguments* args = dynamic_cast<Syntax::FunctionArguments*> (g_syntaxStack.pop());

    if (args == nullptr)
        throw std::runtime_error("Could not pop arguments!");

    Syntax::FunctionCall* f = new Syntax::FunctionCall(symbol, args);

    g_syntaxStack.push(f);

}

void handleParameter1()
{
    if (g_syntaxStack.empty())
        throw std::runtime_error("Stack empty!");

    Syntax::Syntagma* s = g_syntaxStack.pop();

    Syntax::FunctionArguments* a = dynamic_cast<Syntax::FunctionArguments*>(g_syntaxStack.pop());

    if (a == nullptr)
        throw std::runtime_error("No argument list on stack!");

    a->append(s);

    g_syntaxStack.push(a);

}

void handleParameter2()
{
    if (g_syntaxStack.empty())
        throw std::runtime_error("Stack empty!");

    Syntax::Syntagma* s = g_syntaxStack.pop();

    Syntax::FunctionArguments* a = new Syntax::FunctionArguments();
    a->append(s);

    g_syntaxStack.push(a);
}

void handleParameter3()
{
    Syntax::FunctionArguments* a = new Syntax::FunctionArguments();

    g_syntaxStack.push(a);
}

void handleAsm1()
{
    // ASM decorators '(' string_literal ':' reglist ':' reglist ':' reglist ')' ';'

    Syntax::RegisterList* rl3 = dynamic_cast<Syntax::RegisterList*>(g_syntaxStack.pop());
    Syntax::RegisterList* rl2 = dynamic_cast<Syntax::RegisterList*>(g_syntaxStack.pop());
    Syntax::RegisterList* rl1 = dynamic_cast<Syntax::RegisterList*>(g_syntaxStack.pop());

    if (rl1 == nullptr || rl2 == nullptr || rl3 == nullptr)
        throw std::runtime_error("Could not get a register list!");

    Syntax::Immediate* sl = dynamic_cast<Syntax::Immediate*>(g_syntaxStack.pop());

    if (sl == nullptr)
        throw std::runtime_error("Could not get a string literal!");

    Syntax::DecoratorList* dl = dynamic_cast<Syntax::DecoratorList*>(g_syntaxStack.pop());

    if (dl == nullptr)
        throw std::runtime_error("could not get a decorator list!");

    Syntax::AsmStatement* a = new Syntax::AsmStatement(dl, sl, rl1, rl2, rl3);

    g_syntaxStack.push(a);

}

void handleAsm2()
{
    // ASM decorators '(' string_literal ':' reglist ':' reglist ')'             ';'

    Syntax::RegisterList* rl2 = dynamic_cast<Syntax::RegisterList*>(g_syntaxStack.pop());
    Syntax::RegisterList* rl1 = dynamic_cast<Syntax::RegisterList*>(g_syntaxStack.pop());

    if (rl1 == nullptr || rl2 == nullptr)
        throw std::runtime_error("Could not get a register list!");

    Syntax::Immediate* sl = dynamic_cast<Syntax::Immediate*>(g_syntaxStack.pop());

    if (sl == nullptr)
        throw std::runtime_error("Could not get a string literal!");

    Syntax::DecoratorList* dl = dynamic_cast<Syntax::DecoratorList*>(g_syntaxStack.pop());

    if (dl == nullptr)
        throw std::runtime_error("could not get a decorator list!");

    Syntax::AsmStatement* a = new Syntax::AsmStatement(dl, sl, rl1, rl2, nullptr);

    g_syntaxStack.push(a);
}

void handleStringLiteral1(char* string)
{
    // string_literal STR_LITERAL

    Syntax::Immediate* i = dynamic_cast<Syntax::Immediate*>(g_syntaxStack.pop());

    if (i == nullptr)
        throw std::runtime_error("Could not pop a string literal!");

    i->append(string);

    g_syntaxStack.push(i);

}

void handleStringLiteral2(char* string)
{
    // STR_LITERAL

    Syntax::Immediate* i = new Syntax::Immediate(string, false);

    g_syntaxStack.push(i);
}

void handleExpressionSubtraction()
{
    handleExpressionArith(Syntax::BinaryExpressionType::Subtraction);
}

void handleExpressionMultiplication()
{
    handleExpressionArith(Syntax::BinaryExpressionType::Multiply);
}

void handleExpressionDivision()
{
    handleExpressionArith(Syntax::BinaryExpressionType::Divide);
}

void handleExpressionShiftLeft()
{
    handleExpressionArith(Syntax::BinaryExpressionType::ShiftLeft);
}

void handleExpressionShiftRight()
{
    handleExpressionArith(Syntax::BinaryExpressionType::ShiftRight);
}

void handleDecorator(Syntax::DecoratorType type)
{
    Syntax::Decorator* d = new Syntax::Decorator(type);

    g_syntaxStack.push(d);
}

void handleDecoratorList1()
{
    Syntax::Decorator* d = dynamic_cast<Syntax::Decorator*>(g_syntaxStack.pop());

    if (d == nullptr)
        throw std::runtime_error("Could not pop a decorator!");

    Syntax::DecoratorList* dl = dynamic_cast<Syntax::DecoratorList*>(g_syntaxStack.pop());

    if (dl == nullptr)
        throw std::runtime_error("Could not pop a decorator list!");

    dl->append(d);

    g_syntaxStack.push(dl);
}

void handleDecoratorList2()
{
    Syntax::Decorator* d = dynamic_cast<Syntax::Decorator*>(g_syntaxStack.pop());

    if (d == nullptr)
        throw std::runtime_error("Could not pop a decorator!");

    Syntax::DecoratorList* dl = new Syntax::DecoratorList();

    dl->append(d);

    g_syntaxStack.push(dl);
}

void handleDecoratorList3()
{
    Syntax::DecoratorList* dl = new Syntax::DecoratorList();

    g_syntaxStack.push(dl);
}

void handleReg1(char* reg)
{
    Syntax::RegisterDescription* r = new Syntax::RegisterDescription(reg, (char*)"");

    g_syntaxStack.push(r);
}

void handleReg2(char* reg, char* sym)
{
    Syntax::RegisterDescription* r = new Syntax::RegisterDescription(reg, sym);

    g_syntaxStack.push(r);
}

void handleRegList1()
{
    Syntax::RegisterDescription* r = dynamic_cast<Syntax::RegisterDescription*>(g_syntaxStack.pop());

    if (r == nullptr)
        throw std::runtime_error("Could not pop a register description!");

    Syntax::RegisterList* rl = dynamic_cast<Syntax::RegisterList*>(g_syntaxStack.pop());

    if (rl == nullptr)
        throw std::runtime_error("Could not pop a register list!");

    rl->append(r);

    g_syntaxStack.push(rl);

}

void handleRegList2()
{
    Syntax::RegisterDescription* r = dynamic_cast<Syntax::RegisterDescription*>(g_syntaxStack.pop());

    if (r == nullptr)
        throw std::runtime_error("Could not pop a register description!");

    Syntax::RegisterList* rl = new Syntax::RegisterList;

    rl->append(r);

    g_syntaxStack.push(rl);

}

void handleRegList3()
{

    Syntax::RegisterList* rl = new Syntax::RegisterList;

    g_syntaxStack.push(rl);

}

void handleGroupedExpression()
{
    Syntax::Syntagma* expression = g_syntaxStack.pop();

    Syntax::GroupedExpression* g = new Syntax::GroupedExpression(expression);

    g_syntaxStack.push(g);
}
