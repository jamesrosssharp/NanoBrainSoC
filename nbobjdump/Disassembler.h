#pragma once

#include <cstdint>

class Disassembler
{
public:

    virtual ~Disassembler() {}

    virtual void read() = 0;
    virtual void disassemble() = 0;

    virtual void disassembleWord(std::uint32_t curAddress, std::uint16_t word);

private:

    uint16_t m_imm = 0;

};

