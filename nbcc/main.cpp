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

#include "syntaxstack.h"
#include "syntax.h"
#include "codegenerator.h"

extern "C" int yylex();
extern "C" int yyparse();
extern "C" FILE *yyin;
extern int line_num;

SyntaxStack g_syntaxStack;


void printUsage(char *arg0)
{
    std::cout << "Usage: " << arg0 << " [-o <output.o>] <file.c>" << std::endl;
    std::cout << std::endl;
}

int main(int argc, char** argv) {

    if (argc == 1)
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
    if ( ! myfile)
    {
        std::cout << "I can't open file " << argv[optind] << std::endl;
        return -1;
    }
    // set lex to read from it instead of defaulting to STDIN:
    yyin = myfile;

    // parse through the input until there is no more:

    do {
    yyparse();
    } while (!feof(yyin));

    fclose(myfile);

    // Pop top level from syntax stack

    Syntax::Syntagma* s = g_syntaxStack.pop();

    if (! g_syntaxStack.empty())
    {
        throw std::runtime_error("After popping top level, syntax stack was not empty. Weird!");
    }

    if (s == nullptr)
        throw std::runtime_error("AST was null!");

    std::cout << "Abstract syntax tree: " << std::endl;
    std::cout << *s << std::endl;

    std::cout << "Generating code: " << std::endl;
    std::cout << "==================================================" << std::endl;

    CodeGen::CodeGenerator g(s);

    g.generate();

    std::cout << "==================================================" << std::endl;

    std::cout << "Cleaning up " << std::endl;
    delete s;

}

