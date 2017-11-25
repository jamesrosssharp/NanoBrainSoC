#pragma once

#include "Disassembler.h"

#include <cstdint>
#include <vector>

class BinFileDisassembler : public Disassembler
{

public:
    BinFileDisassembler(char* fname, std::uint32_t origin);
    void read();
    void disassemble();

private:
    std::vector<uint16_t> m_words;
    char* m_fname;
    std::uint32_t m_origin;

};
