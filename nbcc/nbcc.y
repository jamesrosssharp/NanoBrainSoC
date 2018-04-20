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
#include "parsehandlers.h"

extern "C" int yylex();
extern "C" int yyparse();
extern "C" FILE *yyin;
extern int line_num;

extern SyntaxStack g_syntaxStack;

void yyerror(const char *s);

%}

%union {
  int           integerLiteral;
  unsigned int  hexLiteral;
  char*         stringLiteral;
  char*         symbol;
}
%error-verbose

%token TYPE_INT TYPE_CHAR TYPE_SHORT TYPE_LONG
%token TYPE_UNSIGNED_INT TYPE_UNSIGNED_CHAR TYPE_UNSIGNED_SHORT TYPE_UNSIGNED_LONG
%token TYPE_VOID

%token ASM VOLATILE CONST STATIC AUTO

%token RETURN WHILE IF SWITCH CASE ELSE DO

%token LEFT_BRACE RIGHT_BRACE

%token SHIFT_LEFT SHIFT_RIGHT

%token DECREMENT INCREMENT

%token <integerLiteral> INT
%token <hexLiteral>     HEXVAL
%token <stringLiteral>  STRING
%token <stringLiteral>  STR_LITERAL
%token <stringLiteral>  CH_LITERAL
%token <symbol>         SYMBOL

%left '+'
%left '-'
%left '*'
%left '/'
%left SHIFT_RIGHT
%left SHIFT_LEFT
%left '&'
%left '|'
%left '^'

%right DECREMENT
%right INCREMENT

%%

program: function program    { handleNewTopLevel(); } |
         declaration program { handleNewTopLevel(); } |
                             ;

/* function declaration */

function: type SYMBOL argument_list block { handleFunctionDeclaration($2); } ;

argument_list: '(' arguments ')'

arguments: arguments ',' variable { handleArguments1(); } |
           variable               { handleArguments2(); } |
                                  { handleArguments3(); }
            ;

variable: type SYMBOL { handleVariable($2); } ;

/* block */

block: LEFT_BRACE block_contents RIGHT_BRACE ;

block_contents: block_contents statement { handleBlock1(); } |
                block_contents block     { handleBlock2(); } |
                                         { handleBlock3(); }
                ;

/* statements */

statement: return_statement     |
           while_statement      |
           ifelse_statement     |
           if_statement         |
           assigment            |
           declaration          |
           asm_block            |
           do_while_statement   |
           expression ';' ;

/* asm block */

asm_block: ASM decorators '(' string_literal ':' reglist ':' reglist ':' reglist ')' ';' { handleAsm1(); } |
           ASM decorators '(' string_literal ':' reglist ':' reglist ')'             ';' { handleAsm2(); }

reglist:  reglist ',' reg { handleRegList1(); } | reg { handleRegList2(); } | { handleRegList3(); }

reg:    STR_LITERAL { handleReg1($1); } | STR_LITERAL '(' SYMBOL ')' { handleReg2($1,$3); }

decorators: decorators decorator { handleDecoratorList1(); } | decorator { handleDecoratorList2(); } | { handleDecoratorList3(); } ;

decorator: CONST    { handleDecorator(Syntax::DecoratorType::Const);     }    |
           VOLATILE { handleDecorator(Syntax::DecoratorType::Volatile);  }    |
           STATIC   { handleDecorator(Syntax::DecoratorType::Static);    }    |
           AUTO     { handleDecorator(Syntax::DecoratorType::Auto);      }

/* return */

return_statement: RETURN expression ';' { handleReturnStatement(); }

/* while */

while_statement: WHILE '(' expression ')' block { handleWhile1(); } |
                 WHILE '(' expression ')' ';'       { handleWhile2(); }

/* do ... while */

do_while_statement: DO block WHILE '(' expression ')' ';' { handleDoWhile(); }

/* if */

if_statement: IF '(' expression ')' block { handleIf1(); } |
              IF '(' expression ')' statement ';' { handleIf2(); }

/* if ... else */

ifelse_statement: ifelse_statement ifelse_statement_basic |
                  ifelse_statement_basic ;

ifelse_statement_basic: IF '(' expression ')' block ELSE block { handleIfElse1(); } |
                  IF '(' expression ')' block ELSE statement  {handleIfElse2(); } |
                  IF '(' expression ')' statement ELSE block {handleIfElse3(); } |
                  IF '(' expression ')' statement ELSE statement  {handleIfElse4(); }

/* assignment */

assigment:      SYMBOL '=' expression ';'           { handleAssignment($1); }

/* variable declaration */

declaration:    type SYMBOL '=' expression ';'      { handleDeclaration($2); }

/* expression */

expression:
        SYMBOL          { handleExpressionSymbol($1); }             |
        INT             { handleExpressionIntImmediate($1); }       |
        CH_LITERAL      { handleExpressionCharLiteral($1); }        |
        expression '+' expression { handleExpressionAddition(); }   |
        expression '-' expression { handleExpressionSubtraction(); }   |
        expression '*' expression { handleExpressionMultiplication(); }   |
        expression '/' expression { handleExpressionDivision(); }   |
        expression '&' expression { handleExpressionBitwiseAnd(); }   |
        expression '|' expression { handleExpressionBitwiseOr(); }   |
        expression '^' expression { handleExpressionBitwiseXor(); } |
        expression SHIFT_LEFT expression    { handleExpressionShiftLeft(); } |
        expression SHIFT_RIGHT expression   { handleExpressionShiftRight(); } |
        '(' expression ')'                  { handleGroupedExpression(); } |
        SYMBOL parameter_list               { handleExpressionFunctionCall($1); } |
        SYMBOL INCREMENT                    { handleExpressionPostIncrement($1); } |
        INCREMENT SYMBOL                     { handleExpressionPreIncrement($2); } |
        SYMBOL DECREMENT                     { handleExpressionPostDecrement($1); } |
        DECREMENT SYMBOL                     { handleExpressionPreDecrement($2); }

        ;

/* string literal - strings can be concatenated */

string_literal: string_literal STR_LITERAL { handleStringLiteral1($2); } | STR_LITERAL { handleStringLiteral2($1); }

/* parameters for function call */

parameter_list: '(' parameters ')' ;

parameters: parameters ',' parameter { handleParameter1(); }    |
            parameter                { handleParameter2(); }    |
                                     { handleParameter3(); }

parameter: expression ;

/* types */

type: TYPE_INT   { pushType("int"); }  |
      TYPE_CHAR  { pushType("char"); } |
      TYPE_SHORT { pushType("short");} |
      TYPE_LONG  { pushType("long"); } |
      TYPE_UNSIGNED_INT   { pushType("uint");  } |
      TYPE_UNSIGNED_CHAR  { pushType("uchar"); } |
      TYPE_UNSIGNED_SHORT { pushType("ushort");} |
      TYPE_UNSIGNED_LONG  { pushType("ulong"); } |
      TYPE_VOID           { pushType("void");  }

%%

void yyerror(const char *s)
{
    std::cout << "Parse error on line " << line_num << ": " << s << std::endl;
    // might as well halt now:
    exit(-1);
}

