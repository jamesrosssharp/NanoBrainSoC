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

#include <string>
#include <list>
#include <ostream>

namespace Syntax
{

    enum class ElementType
    {
        Immediate,
        Type,
        Attribute,
        Variable,
        UnaryExpression,
        BinaryExpression,
        Assignment,
        ReturnStatement,
        FunctionArguments,
        FunctionDeclaration,
        FunctionCall,
        IfStatement,
        VariableDeclaration,
        WhileStatement,
        Block,
        Function,
        TopLevel
    };

    enum class UnaryExpressionType
    {
        LogicalNot,
        BitwiseNot,
        Negation
    };

    enum class ImmediateType
    {
        Int,
        Uint,
        Float
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
    };

    // Base class for all syntax units
    class Syntagma
    {
    public:
        virtual ~Syntagma() {}
        Syntagma(ElementType type) : m_type(type) {}

        virtual ElementType type() const { return m_type; }

        virtual void print(std::ostream& os, int indent) const = 0;

    protected:
        ElementType m_type;

    };

    class Immediate : public Syntagma
    {
    public:
        Immediate(int value) :
            Syntagma(ElementType::Assignment),
            m_immType(ImmediateType::Int)
            { m_value.i = value; }

        void print(std::ostream& os, int indent) const;

    private:
        ImmediateType m_immType;
        union
        {
            int i;
            unsigned int u;
            float f;
        } m_value;
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
        Variable(Syntagma* variableType, char* symbol) :
            Syntagma(ElementType::Variable),
            m_varType(variableType),
            m_symbol(symbol) {}

        ~Variable() {}

        void print(std::ostream& os, int indent) const;

    private:
        Syntagma* m_varType;
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

    private:
        std::list<Syntagma*> m_arguments;
    };

    class FunctionDeclaration : public Syntagma
    {
    public:
        FunctionDeclaration(char* function, Syntagma* attribute, Syntagma* returnType, FunctionArguments* arguments, Syntagma* block)   :
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
        }

        void print(std::ostream& os, int indent) const;

    protected:
        std::string m_functionName;
        Syntagma* m_attribute;
        Syntagma* m_returnType;
        FunctionArguments* m_arguments;
        Syntagma* m_body;
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

    protected:
        std::string m_functionName;
        FunctionArguments* m_arguments;
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

    private:
        std::list<Syntagma*> m_statements;

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

    class VariableDeclaration : public Syntagma
    {
    public:
        VariableDeclaration(char* varname, char* type, char* decorator, Syntagma* expression)    :
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

    private:

        std::string m_varname;
        std::string m_vartype;
        std::string m_decorator;    // e.g. auto, static, const
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

        void print(std::ostream& os, int indent) const;

    private:

        std::list<Syntagma*> m_declarations;

    };

    void PrintIndent(std::ostream& os, int indent);

}

std::ostream& operator << (std::ostream& os, const Syntax::Syntagma& s);