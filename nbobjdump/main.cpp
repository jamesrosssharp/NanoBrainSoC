#include "BinFileDisassembler.h"
#include "Disassembler.h"

#include <fstream>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <sstream>
#include <cstdint>

void printUsage(char* exe)
{
    std::cout << "Usage: " << exe << " -t <bin|elf> -o <origin (for bin files)> <infile> " << std::endl;

}

int main(int argc, char** argv)
{
    if (argc == 1)
    {
        printUsage(argv[0]);
        exit(EXIT_FAILURE);
    }

    char *type  = nullptr;
    std::uint32_t origin = 0;
    char c;

    while ((c = getopt(argc, argv, "t:o:")) != -1)
    switch (c)
    {
        case 't':
            type = strdup(optarg);
            break;
        case 'o':
            origin = atoi(optarg);
        default:
            printUsage(argv[0]);
            exit(EXIT_FAILURE);
    }

    if (optind == argc)
    {
        std::cout << "ERROR: no input file" << std::endl;
        exit(EXIT_FAILURE);
    }

    // open input file and disassemble

    if (type == nullptr)
        type = strdup("bin");

    std::string typestr(type);

    Disassembler* disas;

    if (typestr == "bin")
    {
        disas = new BinFileDisassembler (argv[optind], origin);
    }
    else
    {
        std::cout << "elf files not yet supported!" << std::endl;
    }

    disas->read();
    disas->disassemble();

    delete disas;

    free(type);

}
