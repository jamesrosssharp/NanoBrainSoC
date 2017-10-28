%{
#include <cstdio>
#include <iostream>
#include "AST.h"


  using namespace std;

  // stuff from flex that bison needs to know about:
  extern "C" int yylex();
  extern "C" int yyparse();
  extern "C" FILE *yyin;
  extern int line_num;
 
  AST g_ast;

  void yyerror(const char *s);
  %}

// Bison fundamentally works by asking flex to get the next token, which it
// returns as an object of type "yystype".  But tokens could be of any
// arbitrary data type!  So we deal with that in Bison by defining a C union
// holding each of the types of tokens that Flex could return, and have Bison
// use that union instead of "int" for the definition of "yystype":
%union {
  int ival;
  unsigned int hval;
  char *sval;
  char *lsval;
  char *cval;
  char *regval;
  char *opcval;
  char *pseudoopval;
}

%token COMMA HEX
%token END ENDL
%token TIMES OPEN_PARENTHESIS
%token CLOSE_PARENTHESIS PLUS MINUS MULT SHL SHR AND OR NOT XOR
%token DIV LABEL
%token EQU

 // define the "terminal symbol" token types I'm going to use (in CAPS
 // by convention), and associate each with a field of the union:
%token <ival> INT
%token <hval> HEXVAL
%token <sval> STRING
%token <lsval> STR_LITERAL
%token <cval> CH_LITERAL
%token <regval> REG
%token <opcval> OPCODE
%token <pseudoopval> PSEUDOOP

%%

asm: body_section footer { cout << "Parsed asm file" << endl; } ;

op_code : op_code_1 | op_code_2 | op_code_3 | op_code_4;

op_code_1: OPCODE REG COMMA REG ENDLS { g_ast.addTwoRegisterOpcode(line_num - 1, $1,$2,$4); } ;
op_code_2 : OPCODE REG COMMA expressions ENDLS { g_ast.addOneRegisterAndExpressionOpcode(line_num - 1, $1, $2); } ;
op_code_3 : OPCODE expressions ENDLS { g_ast.addExpressionOpcode(line_num - 1, $1); } ;
op_code_4 : OPCODE ENDLS { g_ast.addStandaloneOpcode(line_num - 1, $1); } ;

expressions: expressions expression | expression ;
expression: hexval | integer | open_parenthesis | close_parenthesis | plus | minus|
            mult | div | and | or | shl | shr | not | xor | string | str_literal | ch_literal;


str_literal:            STR_LITERAL         { g_ast.addStringLiteralToCurrentStatementExpression($1); }
ch_literal:             CH_LITERAL          { g_ast.addCharLiteralToCurrentStatementExpression($1); }
string:                 STRING              { g_ast.addStringToCurrentStatementExpression($1); }
open_parenthesis:       OPEN_PARENTHESIS    { g_ast.addLeftParenthesisToCurrentStatementExpression(); }
close_parenthesis:      CLOSE_PARENTHESIS   { g_ast.addRightParenthesisToCurrentStatementExpression(); }
plus :                  PLUS                { g_ast.addPlusToCurrentStatementExpression(); }
minus :                 MINUS               { g_ast.addMinusToCurrentStatementExpression(); }
mult:                   MULT                { g_ast.addMultToCurrentStatementExpression(); }
div:                    DIV                 { g_ast.addDivToCurrentStatementExpression(); }
and :                   AND                 { g_ast.addAndToCurrentStatementExpression(); }
or :                    OR                  { g_ast.addOrToCurrentStatementExpression(); }
not :                   NOT                 { g_ast.addNotToCurrentStatementExpression(); }
xor :                   XOR                 { g_ast.addXorToCurrentStatementExpression(); }
shl :                   SHL                 { g_ast.addShiftLeftToCurrentStatementExpression();}
shr :                   SHR                 { g_ast.addShiftRightToCurrentStatementExpression();}
hexval:                 HEXVAL              { g_ast.addUIntToCurrentStatementExpresion($1);}
integer:                INT                 { g_ast.addIntToCurrentStatementExpression($1); }

equ: STRING EQU expressions ENDLS {g_ast.addEqu(line_num - 1, $1); } ;

times_line: times ;

times: TIMES expressions { g_ast.addTimes(line_num); } ;

pseudoop_line: pseudoop ;

pseudoop: PSEUDOOP expressions ENDLS { g_ast.addExpressionPseudoOp(line_num - 1, $1); } ;

label: STRING LABEL ENDLS { g_ast.addLabel(line_num - 1, $1); } ;

body_section: body_lines ;
body_lines: body_lines body_line | body_line ;

body_line: op_code | times | pseudoop | label | equ ;

footer: END ENDLS;
ENDLS: ENDLS ENDL | ENDL ;
%%

int main(int, char**) {

  // open a file handle to a particular file:
  FILE *myfile = fopen("Test.asm", "r");
  // make sure it's valid:
  if (!myfile) {
    cout << "I can't open Test.asm file!" << endl;
    return -1;
  }
  // set lex to read from it instead of defaulting to STDIN:
  yyin = myfile;

  // parse through the input until there is no more:
	
  do {
    yyparse();
  } while (!feof(yyin));
	
  //cout << g_ast << endl;

  // build symbol table

  g_ast.buildSymbolTable();

  // first pass assemble

  g_ast.firstPassAssemble();

  g_ast.printAssembly();

  // resolve symbols

  g_ast.resolveSymbols();

  g_ast.printSymbolTable();

  // evaluate expressions

  g_ast.evaluateExpressions();

  // generate assembly

  g_ast.assemble();

  g_ast.printAssembly();


  // output binary file

  g_ast.writeBinOutput("Test.bin");

}

void yyerror(const char *s) {
  cout << "Parse error on line " << line_num << ": " << s << endl;
  // might as well halt now:
  exit(-1);
}
