#pragma once

#include <vector>
#include <cstdint>

#include "types.h"



class Assembly
{
public:

    static void makeArithmeticInstructionWithImmediate(uint16_t opcode,
                                                       Register reg,
                                                       int32_t immediate, std::vector<uint16_t>& assembledWords,
                                                       uint32_t lineNum);

    static void makeFlowControlInstruction(OpCode opcode, uint32_t address, uint32_t target, uint32_t lineNum,
                                           std::vector<uint16_t>& assembledWords);

    static const uint16_t NOP_INSTRUCTION        = 0b0100011100000000;
    static const uint16_t IMM_INSTRUCTION        = 0b0000000000000000;

    static const uint16_t JUMP_INSTRUCTION       = 0b1000010000000000;
    static const uint16_t JUMPNZ_INSTRUCTION     = 0b1000000000000000;
    static const uint16_t JUMPZ_INSTRUCTION      = 0b1001000000000000;
    static const uint16_t JUMPNC_INSTRUCTION     = 0b1000100000000000;
    static const uint16_t JUMPC_INSTRUCTION      = 0b1001100000000000;
    static const uint16_t JUMP_REL_INSTRUCTION   = 0b1000011000000000;
    static const uint16_t JUMPNZ_REL_INSTRUCTION = 0b1000001000000000;
    static const uint16_t JUMPZ_REL_INSTRUCTION  = 0b1001001000000000;
    static const uint16_t JUMPNC_REL_INSTRUCTION = 0b1000101000000000;
    static const uint16_t JUMPC_REL_INSTRUCTION  = 0b1001101000000000;

    static const uint16_t CALL_INSTRUCTION       = 0b1010010000000000;
    static const uint16_t CALLNZ_INSTRUCTION     = 0b1010000000000000;
    static const uint16_t CALLZ_INSTRUCTION      = 0b1011000000000000;
    static const uint16_t CALLNC_INSTRUCTION     = 0b1010100000000000;
    static const uint16_t CALLC_INSTRUCTION      = 0b1011100000000000;
    static const uint16_t CALL_REL_INSTRUCTION   = 0b1010011000000000;
    static const uint16_t CALLNZ_REL_INSTRUCTION = 0b1010001000000000;
    static const uint16_t CALLZ_REL_INSTRUCTION  = 0b1011001000000000;
    static const uint16_t CALLNC_REL_INSTRUCTION = 0b1010101000000000;
    static const uint16_t CALLC_REL_INSTRUCTION  = 0b1011101000000000;

    static const uint16_t LDSPR_INSTRUCTION      = 0b1110000000000000;


};

