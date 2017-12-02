%{
/*
 *                                                   _
         .-==-.                   ..--/\/\--..      (_)
    __      ___    __      ____  ___  ____  ___     __ __
   /  \  /\/  _\/\/  \  /\/ _  \/ _ \/ _  \/ _ \/\ / //  \  /\
  / /\ \/ /  (_) / /\ \/ / (_) / (_)/ (_) / (_)  // // /\ \/ /
  \/  \__/\__/\_/\/  \__/\____/ (_) \ __ (\__/\_//_/ \/  \__/
                              \_____//  \_\
   .-==-.           --===/\/\/\===---             ..--/\/\--..

    NanoBrain C Compiler (c) 2017 James Ross Sharp

                                                                            */

#include <cstdio>
#include <iostream>
#include <unistd.h>
#include <string.h>

extern "C" int yylex();
extern "C" int yyparse();
extern "C" FILE *yyin;
extern int line_num;

void yyerror(const char *s);
  %}

%union {
  int integerLiteral;
  unsigned int hexLiteral;
  char *stringLiteral;
}

%token STR_LITERAL
%token ENDL

%token <integerLiteral> INT
%token <hexLiteral>     HEXVAL
%token <stringLiteral>  STRING


%%

statement: STRING ENDL ;

%%

void printUsage(char *arg0)
{
    std::cout << "Usage: " << arg0 << " [-o <output.o>] <file.c>" << std::endl;
    std::cout << std::endl;
}

int main(int argc, char** argv) {

    if (argc != 2)
    {
        printUsage(argv[0]);
        exit(EXIT_FAILURE);
    }

    char *outputFile = nullptr;
    char *type = nullptr;
    char c;

    while ((c = getopt (argc, argv, "o:")) != -1)
    switch (c)
    {
        case 'o':
            outputFile = strdup(optarg);
            break;
        default:
            printUsage(argv[0]);
            exit(EXIT_FAILURE);
    }

    if (optind == argc)
    {
        std::cout << "ERROR: no input files" << std::endl;
    }

    // open a file handle to a particular file:
    FILE *myfile = fopen(argv[optind], "r");
    // make sure it's valid:
    if (!myfile) {
    std::cout << "I can't open Test.asm file!" << std::endl;
    return -1;
    }
    // set lex to read from it instead of defaulting to STDIN:
    yyin = myfile;

    // parse through the input until there is no more:

    do {
    yyparse();
    } while (!feof(yyin));


}

int yywrap()
{
    return 1;
}

void yyerror(const char *s)
{
    std::cout << "Parse error on line " << line_num << ": " << s << std::endl;
    // might as well halt now:
    exit(-1);
}

