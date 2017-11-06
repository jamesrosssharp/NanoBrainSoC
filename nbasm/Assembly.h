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

    static void makeArithmeticInstruction(uint16_t opcode,
                                                       Register regDest,
                                                       Register regSrc, std::vector<uint16_t>& assembledWords,
                                                       uint32_t lineNum);

    static void makeFlowControlInstruction(OpCode opcode, uint32_t address, uint32_t target, uint32_t lineNum,
                                           std::vector<uint16_t>& assembledWords);

    static void makeLoadStoreWithExpression(OpCode opcode, uint32_t lineNum, std::vector<uint16_t>& words, int32_t value,
                                            Register regInd, Register regDest);


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
    static const uint16_t LOAD_REG_INSTRUCTION   = 0b0100101000000000;

    static const uint16_t ADD_REG_INSTRUCTION    = 0b0100000000000000;
    static const uint16_t ADC_REG_INSTRUCTION    = 0b0101000000000000;
    static const uint16_t SUB_REG_INSTRUCTION    = 0b0100100000000000;
    static const uint16_t SBB_REG_INSTRUCTION    = 0b0101100000000000;
    static const uint16_t AND_REG_INSTRUCTION    = 0b0100010000000000;
    static const uint16_t OR_REG_INSTRUCTION     = 0b0101010000000000;
    static const uint16_t XOR_REG_INSTRUCTION    = 0b0100110000000000;
    static const uint16_t CMP_REG_INSTRUCTION    = 0b0100001000000000;
    static const uint16_t TEST_REG_INSTRUCTION   = 0b0101001000000000;
    static const uint16_t MUL_REG_INSTRUCTION    = 0b0100011000000000;
    static const uint16_t MULS_REG_INSTRUCTION   = 0b0101011000000000;
    static const uint16_t DIV_REG_INSTRUCTION    = 0b0100111000000000;
    static const uint16_t DIVS_REG_INSTRUCTION   = 0b0101111000000000;

    static const uint16_t ADD_IMM_INSTRUCTION    = 0b0110000000000000;
    static const uint16_t ADC_IMM_INSTRUCTION    = 0b0111000000000000;
    static const uint16_t SUB_IMM_INSTRUCTION    = 0b0110100000000000;
    static const uint16_t SBB_IMM_INSTRUCTION    = 0b0111100000000000;
    static const uint16_t AND_IMM_INSTRUCTION    = 0b0110010000000000;
    static const uint16_t OR_IMM_INSTRUCTION     = 0b0111010000000000;
    static const uint16_t XOR_IMM_INSTRUCTION    = 0b0110110000000000;
    static const uint16_t CMP_IMM_INSTRUCTION    = 0b0110001000000000;
    static const uint16_t TEST_IMM_INSTRUCTION   = 0b0111001000000000;
    static const uint16_t LOAD_IMM_INSTRUCTION   = 0b0110101000000000;
    static const uint16_t MUL_IMM_INSTRUCTION    = 0b0110011000000000;
    static const uint16_t MULS_IMM_INSTRUCTION   = 0b0111011000000000;
    static const uint16_t DIV_IMM_INSTRUCTION    = 0b0110111000000000;
    static const uint16_t DIVS_IMM_INSTRUCTION   = 0b0111111000000000;

    static const uint16_t BSL_INSTRUCTION = 0b0100000100000000;
    static const uint16_t BSR_INSTRUCTION = 0b0110000100000000;

    static const uint16_t INCW_INSTRUCTION      = 0b1110001000000010;
    static const uint16_t DECW_INSTRUCTION      = 0b1110001000000011;

    static const uint16_t LDW_REG_INSTRUCTION   = 0b1100000000000000;
    static const uint16_t STW_REG_INSTRUCTION   = 0b1100010000000000;
    static const uint16_t LDW_IMM_INSTRUCTION   = 0b1101000000000000;
    static const uint16_t STW_IMM_INSTRUCTION   = 0b1101010000000000;

    static const uint16_t SL0_INSTRUCTION = 0b0101110000000010;
    static const uint16_t SL1_INSTRUCTION = 0b0101110000000011;
    static const uint16_t SLA_INSTRUCTION = 0b0101110000000000;
    static const uint16_t SLX_INSTRUCTION = 0b0101110000000001;
    static const uint16_t RL_INSTRUCTION  = 0b0101110000000100;

    static const uint16_t SR0_INSTRUCTION = 0b0111110000000010;
    static const uint16_t SR1_INSTRUCTION = 0b0111110000000011;
    static const uint16_t SRA_INSTRUCTION = 0b0111110000000000;
    static const uint16_t SRX_INSTRUCTION = 0b0111110000000001;
    static const uint16_t RR_INSTRUCTION  = 0b0111110000000100;

};

