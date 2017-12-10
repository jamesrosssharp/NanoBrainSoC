%{
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

#include <cstdio>
#include <iostream>
#include <unistd.h>
#include <string.h>
#include <exception>

#include "syntaxstack.h"
#include "syntax.h"

extern "C" int yylex();
extern "C" int yyparse();
extern "C" FILE *yyin;
extern int line_num;

extern SyntaxStack g_syntaxStack;

void yyerror(const char *s);

void handleNewTopLevel();
void pushType(const char* type);

%}

%union {
  int           integerLiteral;
  unsigned int  hexLiteral;
  char*         stringLiteral;
  char*         symbol;
}
%error-verbose

%token STR_LITERAL

%token TYPE_INT TYPE_CHAR TYPE_SHORT TYPE_LONG
%token TYPE_UNSIGNED_INT TYPE_UNSIGNED_CHAR TYPE_UNSIGNED_SHORT TYPE_UNSIGNED_LONG

%token RETURN

%token LEFT_BRACE RIGHT_BRACE

%token <integerLiteral> INT
%token <hexLiteral>     HEXVAL
%token <stringLiteral>  STRING
%token <symbol>         SYMBOL

%left '+'

%%

program: function program
         {
            handleNewTopLevel();
         }
         |
         declaration program
         {
             handleNewTopLevel();
         }
         | ;

function: type SYMBOL argument_list block
        {
            if (g_syntaxStack.empty())
                throw std::runtime_error("Syntax stack empty trying to create a block");

            // block
            Syntax::Syntagma* s = g_syntaxStack.pop();

            if (s == nullptr || s->type() != Syntax::ElementType::Block)
                throw std::runtime_error("Function without a block!");

            // arguments
            Syntax::FunctionArguments* arg = dynamic_cast<Syntax::FunctionArguments*>(g_syntaxStack.pop());

            if (arg == nullptr)
                throw std::runtime_error("Could not retrieve arguments from stack!");
            // return type
            Syntax::Type* type = dynamic_cast<Syntax::Type*>(g_syntaxStack.pop());

            if (type == nullptr)
                throw std::runtime_error("Could not retrieve type from stack!");

            g_syntaxStack.push(new Syntax::FunctionDeclaration($2, nullptr, type, arg, s));

        } ;

argument_list: '(' arguments ')' {std::cout << "got argument list" << std::endl; } ;

arguments: arguments ',' variable
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
            |
            variable
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
            |
            {
                Syntax::FunctionArguments* a = new Syntax::FunctionArguments();

                g_syntaxStack.push(a);
            }
            ;

variable: type SYMBOL {
                        Syntax::Syntagma* type = g_syntaxStack.pop();

                        if (type->type() != Syntax::ElementType::Type)
                            throw std::runtime_error("Expected type");


                        Syntax::Variable* v = new Syntax::Variable(type, $2);

                        g_syntaxStack.push(v);

                      } ;


block: LEFT_BRACE block_contents RIGHT_BRACE
                {

                    Syntax::Block* b = new Syntax::Block();

                    g_syntaxStack.push(b);
                } ;

block_contents: block_contents statement | block_contents block | { std::cout << "got block contents" << std::endl; } ;

statement: return_statement | declaration { std::cout << "got statement" << std::endl; } ;

return_statement: RETURN expression ';' { std::cout << "return statement" << std::endl; };

declaration:
        type SYMBOL '=' expression ';' {std::cout << "got declaration" << std::endl; };

expression:
        SYMBOL |
        INT |
        expression '+' expression |
        SYMBOL parameter_list | /* function call */
        { std::cout << "got expression" << std::endl; }
        ;

parameter_list: '(' parameters ')' ;

parameters: parameters | parameter ',' parameter | parameter | ;

parameter: expression ;

type: TYPE_INT
      {
        pushType("int");
      }
      |
      TYPE_CHAR
      {
         pushType("char");
      }
      |
      TYPE_SHORT
      {
          pushType("short");
      }
      |
      TYPE_LONG
      {
          pushType("long");
      }
      |
      TYPE_UNSIGNED_INT
      {
          pushType("uint");
      }
      |
      TYPE_UNSIGNED_CHAR
      {
          pushType("uchar");
      }
      |
      TYPE_UNSIGNED_SHORT
      {
          pushType("ushort");
      }
      |
      TYPE_UNSIGNED_LONG
      {
          pushType("ulong");
      }


%%

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

void pushType(const char *type)
{
    Syntax::Type* t = new Syntax::Type(type);
    g_syntaxStack.push(t);
}

void yyerror(const char *s)
{
    std::cout << "Parse error on line " << line_num << ": " << s << std::endl;
    // might as well halt now:
    exit(-1);
}

