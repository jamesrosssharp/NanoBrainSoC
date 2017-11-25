#include "BinFileDisassembler.h"

#include <fstream>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <sstream>
#include <limits>

BinFileDisassembler::BinFileDisassembler(char* fname, std::uint32_t origin)
{
    m_fname = fname;
    m_origin = origin;
}

void BinFileDisassembler::read()
{

    std::ifstream from;
    from.open(m_fname);

    from.ignore( std::numeric_limits<std::streamsize>::max() );
    std::streamsize length = from.gcount();
    from.clear();   //  Since ignore will have set eof.
    from.seekg( 0, std::ios_base::beg );

    std::cout << " File size in bytes: " << length << std::endl;

    m_words.resize(length / 2);

    from.read((char*)m_words.data(), m_words.size() * 2);

    from.close();

}

void BinFileDisassembler::disassemble()
{

    uint32_t curAddress = m_origin;

    for (uint16_t word : m_words)
    {
        Disassembler::disassembleWord(curAddress, word);

        curAddress += 2;
    }

}
