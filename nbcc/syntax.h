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

#include "types.h"

#include <string>
#include <list>
#include <ostream>

extern int line_num;

namespace Syntax
{

    enum class ElementType
    {
        Immediate               = 0,
        Type                    = 1,
        Attribute               = 2,
        Variable                = 3,
        Symbol                  = 4,
        UnaryExpression         = 5,
        BinaryExpression        = 6,
        Assignment              = 7,
        ReturnStatement         = 8,
        FunctionArguments       = 9,
        FunctionDeclaration     = 10,
        FunctionCall            = 11,
        IfStatement             = 12,
        VariableDeclaration     = 13,
        WhileStatement          = 14,
        Block                   = 15,
        Function                = 16,
        TopLevel                = 17,
        Decorator               = 18,
        DecoratorList           = 19,
        RegisterDescription     = 20,
        RegisterList            = 21,
        AsmStatement            = 22,
        GroupedExpression       = 23,
    };

    enum class UnaryExpressionType
    {
        LogicalNot,
        BitwiseNot,
        Negation
    };

    enum class BinaryExpressionType
    {
        LogicalOr,
        LogicalAnd,
        BitwiseOr,
        BitwiseAnd,
        BitwiseXor,
        Addition,
        Subtraction,
        Multiply,
        Divide,
        Equal,
        LessThan,
        LessThanOrEqual,
        GreaterThan,
        GreaterThanOrEqual,
        ShiftLeft,
        ShiftRight,
    };

    enum class DecoratorType
    {
        Const,
        Volatile,
        Static,
        Auto
    };

    // Base class for all syntax units
    class Syntagma
    {
    public:
        virtual ~Syntagma() {}
        Syntagma(ElementType type) : m_type(type), m_linenum(line_num) {}

        virtual ElementType type() const { return m_type; }

        int linenum() const { return m_linenum; }

        virtual void print(std::ostream& os, int indent) const = 0;

    protected:
        ElementType m_type;
        int m_linenum;

    };

    class Immediate : public Syntagma
    {
    public:
        Immediate(int value) :
            Syntagma(ElementType::Immediate),
            m_immType(ImmediateType::kInt)
            { m_value.i = value; }

        Immediate(char *string, bool isCharLiteral) :
            Syntagma(ElementType::Immediate)
            {
                m_string = string;
                if (isCharLiteral)
                {
                    m_immType = ImmediateType::kChar;
                }
                else
                {
                    m_immType = ImmediateType::kStringLiteral;
                }
            }

        void append(char* string)
        {
            m_string += string;
        }

        void print(std::ostream& os, int indent) const;

        ImmediateType immType() const { return m_immType; }

        int intValue() const { return m_value.i; }
        unsigned int uintValue() const { return m_value.u; }

        std::string stringValue() const { return m_string; }

    private:
        ImmediateType m_immType;
        union
        {
            int i;
            unsigned int u;
            float f;
        } m_value;
        std::string m_string;

    };

    class Type : public Syntagma
    {
    public:
        Type(const char* type) :
            Syntagma(ElementType::Type),
            m_typeName(type)
        {

        }

        void print(std::ostream &os, int indent) const;

        const std::string& typeName() const { return m_typeName; }

    private:
        std::string m_typeName;
    };

    class Attribute : public Syntagma
    {
    public:
        Attribute(char* attrib) :
            Syntagma(ElementType::Attribute),
            m_attrib(attrib)
        {

        }

        void print(std::ostream &os, int indent) const;
    private:
        std::string m_attrib;

    };

    class Variable : public Syntagma
    {
    public:

        //
        //  Variable types are passed as strings since we need to allow for user defined
        //  type defs.
        //
        Variable(Type* variableType, char* symbol) :
            Syntagma(ElementType::Variable),
            m_varType(variableType),
            m_symbol(symbol) {}

        ~Variable() {}

        void print(std::ostream& os, int indent) const;

        const Syntax::Type* varType() const { return m_varType; }

        const std::string& name() const { return m_symbol; }

    private:
        Type* m_varType;
        std::string m_symbol;
    };

    class Symbol : public Syntagma
    {
    public:

        Symbol(char* symbol)    :
            Syntagma(ElementType::Symbol),
            m_symbol(symbol)
        {

        }

        void print(std::ostream &os, int indent) const;

        const std::string& symbol() const { return m_symbol; }

    private:
        std::string m_symbol;
    };

    class UnaryExpression : public Syntagma
    {
    public:
        UnaryExpression(UnaryExpressionType type, Syntagma* expression) :
            Syntagma(ElementType::UnaryExpression),
            m_expressionType(type),
            m_expression(expression) {}

        ~UnaryExpression() { delete m_expression; }

        void print(std::ostream& os, int indent) const;

    private:
        UnaryExpressionType m_expressionType;
        Syntagma* m_expression;
    };

    class BinaryExpression : public Syntagma
    {
    public:
        BinaryExpression(BinaryExpressionType type, Syntagma* left, Syntagma* right)    :
            Syntagma(ElementType::BinaryExpression),
            m_expressionType(type),
            m_left(left),
            m_right(right)
        {
        }

        ~BinaryExpression()
        {
            delete m_left;
            delete m_right;
        }

        const Syntagma* right() const { return m_right; }
        const Syntagma* left() const { return m_left; }
        BinaryExpressionType binaryExpressionType() const  { return m_expressionType; }

        void print(std::ostream& os, int indent) const;

    private:
        BinaryExpressionType m_expressionType;
        Syntagma* m_left;
        Syntagma* m_right;
    };

    class Assignment : public Syntagma
    {
    public:
        Assignment(char* var, Syntagma* expression) :
            Syntagma(ElementType::Assignment),
            m_var(var),
            m_expression(expression)
        {}

        ~Assignment()
        {
            delete m_expression;
        }

        void print(std::ostream& os, int indent) const;

    private:
        std::string m_var;
        Syntagma* m_expression;
    };

    class ReturnStatement : public Syntagma
    {
    public:
        ReturnStatement(Syntagma* expression)   :
            Syntagma(ElementType::ReturnStatement),
            m_expression(expression)
        {}

        ~ReturnStatement()
        {
            delete m_expression;
        }

        const Syntax::Syntagma* expression() const { return m_expression; }

        void print(std::ostream& os, int indent) const;

    private:
        Syntagma* m_expression;
    };

    class FunctionArguments : public Syntagma
    {
    public:
        FunctionArguments() :
            Syntagma(ElementType::FunctionArguments)
        {

        }

        ~FunctionArguments()
        {
            for (Syntagma* s : m_arguments)
                delete s;
        }

        void append(Syntagma* argument)
        {
            m_arguments.push_back(argument);
        }

        void print(std::ostream& os, int indent) const;
        const std::list<Syntagma*>& arguments() const { return m_arguments; }

    private:
        std::list<Syntagma*> m_arguments;
    };

    class Block : public Syntagma
    {
    public:
        Block() :
            Syntagma(ElementType::Block)
        {

        }

        void append(Syntagma* statement)
        {
            m_statements.push_back(statement);
        }

        ~Block()
        {
            for (Syntagma* s : m_statements)
                delete s;
        }

        void print(std::ostream& os, int indent) const;

        const std::list<Syntagma*>& statements() const { return m_statements; }

    private:
        std::list<Syntagma*> m_statements;

    };

    class FunctionDeclaration : public Syntagma
    {
    public:
        FunctionDeclaration(char* function, Syntagma* attribute, Syntagma* returnType, FunctionArguments* arguments, Block* block)   :
            Syntagma(ElementType::FunctionDeclaration),
            m_functionName(function),
            m_attribute(attribute),
            m_returnType(returnType),
            m_arguments(arguments),
            m_body(block)
        {

        }

        virtual ~FunctionDeclaration()
        {
            delete m_arguments;
            delete m_attribute;
            delete m_returnType;
            delete m_body;
        }

        void print(std::ostream& os, int indent) const;

        const std::string& functionName() const { return m_functionName; }
        const FunctionArguments* arguments() const { return m_arguments; }
        const Block* body() const { return m_body; }

    protected:
        std::string m_functionName;
        Syntagma* m_attribute;
        Syntagma* m_returnType;
        FunctionArguments* m_arguments;
        Block* m_body;
    };

    class FunctionCall : public Syntagma
    {
    public:
        FunctionCall(char* function, FunctionArguments* arguments)  :
            Syntagma(ElementType::FunctionCall),
            m_functionName(function),
            m_arguments(arguments)
        {

        }

        virtual ~FunctionCall()
        {
            delete m_arguments;
        }

        void print(std::ostream& os, int indent) const;

        const std::string& functionName() const { return m_functionName; }
        const FunctionArguments* arguments() const { return m_arguments; }

    protected:
        std::string m_functionName;
        FunctionArguments* m_arguments;
    };


    class IfStatement : public Syntagma
    {
    public:
        IfStatement(Syntagma* condition, Syntagma* block)   :
            Syntagma(ElementType::IfStatement),
            m_condition(condition),
            m_block(block)
        {

        }

        ~IfStatement()
        {
            delete m_condition;
            delete m_block;
        }

        void print(std::ostream& os, int indent) const;

    private:
        Syntagma* m_condition;
        Syntagma* m_block;
    };

    class Decorator : public Syntagma
    {
    public:
        Decorator(DecoratorType type) :
            Syntagma(ElementType::Decorator),
            m_decoratorType(type)
        {

        }

        void print(std::ostream& os, int indent) const;

        DecoratorType decoratorType() const { return m_decoratorType; }

    private:

        DecoratorType m_decoratorType;
    };

    class DecoratorList : public Syntagma
    {
    public:
        DecoratorList() :
            Syntagma(ElementType::DecoratorList)
        {

        }

        ~DecoratorList()
        {
            for (Decorator* d : m_decorators)
                delete d;
        }

        void print(std::ostream &os, int indent) const;

        void append(Decorator* dec)
        {
            m_decorators.push_back(dec);
        }

        const std::list<Decorator*>& decorators() const
        {
            return m_decorators;
        }

    private:
        std::list<Decorator*> m_decorators;

    };

    class VariableDeclaration : public Syntagma
    {
    public:
        VariableDeclaration(char* varname, Type* type, DecoratorList* decorator, Syntagma* expression)    :
            Syntagma(ElementType::VariableDeclaration),
            m_varname(varname),
            m_vartype(type),
            m_decorator(decorator),
            m_expression(expression)
        {

        }

        ~VariableDeclaration()
        {
            delete m_expression;
        }

        void print(std::ostream& os, int indent) const;

        const Type* vartype() const { return m_vartype; }
        const std::string& name() const { return m_varname; }
        const DecoratorList* decorator() const { return m_decorator; }
        const Syntagma* expression() const { return m_expression; }

    private:

        std::string m_varname;
        Type* m_vartype;
        DecoratorList* m_decorator;    // e.g. auto, static, const
        Syntagma* m_expression;

    };

    class WhileStatement : public Syntagma
    {
    public:
        WhileStatement(Syntagma* condition, Syntagma* block) :
            Syntagma(ElementType::WhileStatement),
            m_condition(condition),
            m_block(block)
        {

        }

        ~WhileStatement()
        {
            delete m_condition;
            delete m_block;
        }

        void print(std::ostream& os, int indent) const;

    private:

        Syntagma* m_condition;
        Syntagma* m_block;
    };

    class TopLevel : public Syntagma
    {
    public:
        TopLevel()  :
            Syntagma(ElementType::TopLevel)
        {

        }

        ~TopLevel()
        {
            for (Syntagma* s : m_declarations)
                delete s;
        }

        void append(Syntagma* s)
        {
            m_declarations.push_front(s);
        }

        const std::list<Syntagma*>& declarations() const { return m_declarations; }

        void print(std::ostream& os, int indent) const;

    private:

        std::list<Syntagma*> m_declarations;

    };



    class RegisterDescription : public Syntagma
    {
    public:
        RegisterDescription(char* reg, char* sym)   :
            Syntagma(ElementType::RegisterDescription),
            m_reg(reg),
            m_symbol(sym)
        {

        }

        void print(std::ostream &os, int indent) const;

        std::string symbol() { return m_symbol; }
        std::string reg() { return m_reg; }

    private:

        std::string m_reg;
        std::string m_symbol;

    };

    class RegisterList : public Syntagma
    {
    public:
        RegisterList() :
            Syntagma(ElementType::RegisterList)
        {

        }

        ~RegisterList()
        {
            for (RegisterDescription* r : m_registers)
                if (r != nullptr)
                    delete r;
        }

        void print(std::ostream &os, int indent) const;

        void append(RegisterDescription* desc)
        {
            m_registers.push_back(desc);
        }

        const std::list<RegisterDescription*>& getRegisterDescriptions() const { return m_registers; }

    private:

        std::list<RegisterDescription*> m_registers;

    };

    class AsmStatement : public Syntagma
    {
    public:

        AsmStatement(DecoratorList* dl, Immediate* sl, RegisterList* out, RegisterList* in, RegisterList* clobbers) :
            Syntagma(ElementType::AsmStatement),
            m_dl(dl),
            m_stringLiteral(sl),
            m_out(out),
            m_in(in),
            m_clobbers(clobbers)
        {

        }

        ~AsmStatement()
        {
            delete m_dl;
            delete m_stringLiteral;
            delete m_out;
            delete m_in;
            delete m_clobbers;
        }

        void print(std::ostream &os, int indent) const;

        std::string getAsm() const
        {
            return m_stringLiteral->stringValue();
        }

        const RegisterList* getInRegisters() const
        {
            return m_in;
        }

        const RegisterList* getClobberRegisters() const
        {
            return m_clobbers;
        }

    private:

        DecoratorList* m_dl;
        Immediate*     m_stringLiteral;
        RegisterList*  m_out;
        RegisterList*  m_in;
        RegisterList*  m_clobbers;

    };

    class GroupedExpression : public Syntagma
    {
    public:

        GroupedExpression(Syntax::Syntagma* expression) :
            Syntagma(ElementType::GroupedExpression),
            m_expression(expression)
        { }

        ~GroupedExpression()
        {
            delete m_expression;
        }

        void print(std::ostream &os, int indent) const;

        const Syntax::Syntagma* expression() const { return m_expression; }

    private:
        Syntax::Syntagma* m_expression;
    };

    void PrintIndent(std::ostream& os, int indent);

}

std::ostream& operator << (std::ostream& os, const Syntax::Syntagma& s);
