#include <fstream>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <sstream>

void printUsage(char* exe)
{
    std::cout << "Usage: " << exe << " -o <outfile> -t <template.vhd> <infile> " << std::endl;

}

void printBitVector(std::ostream& outf, uint16_t word)
{
    outf << "\"";
    for (int i = 0; i < 16; i++)
    {
        if (word & (1 << (16 - i)))
            outf << "1";
        else
            outf << "0";
    }
    outf << "\"";

}

int main(int argc, char** argv)
{
    if (argc == 1)
    {
        printUsage(argv[0]);
        exit(EXIT_FAILURE);
    }

    char *outputFile    = nullptr;
    char *templateFile  = nullptr;
    char c;

    while ((c = getopt (argc, argv, "t:o:")) != -1)
    switch (c)
    {
        case 't':
            if (optarg != nullptr)
                templateFile = strdup(optarg);
            else
            {
                std::cout << "optarg was null!" << std::endl;
            }
            break;
        case 'o':
            outputFile   = strdup(optarg);
            break;
        default:
            printUsage(argv[0]);
            exit(EXIT_FAILURE);
    }

    if (optind == argc)
    {
        std::cout << "ERROR: no input files" << std::endl;
        exit(EXIT_FAILURE);
    }

    if (templateFile == nullptr)
    {
        std::cout << "ERROR: no template file" << std::endl;
        exit(EXIT_FAILURE);
    }

    // open a file handle to a particular file:

    std::ifstream inf;
    std::ofstream outf;

    std::ifstream binf;

    char* binFile = argv[optind];

    inf.open(templateFile);
    outf.open(outputFile == nullptr ? "out.vhd" : outputFile);
    binf.open(binFile);

    std::string s;

    while (std::getline(inf, s))
    {
        // Read line from inf
        // Tokenise

        std::string token = "%ROM_DATA%";

        int pos = s.find(token);

        if (pos > 0)
        {
            std::string beg = s.substr(0, pos);
            std::string end = s.substr(pos + token.size(), s.size());

            outf << beg << std::endl;

            int idx = 0;

            while (! binf.eof())
            {
                unsigned short s;
                binf.read((char*)&s, sizeof(unsigned short));
                outf << "\t\t\t" << idx++ << " => ";
                printBitVector(outf, s);
                outf << "\n";
            }

            outf << end;
        }
        else
        {
            outf << s;
        }

        outf << "\n";

    }

}
