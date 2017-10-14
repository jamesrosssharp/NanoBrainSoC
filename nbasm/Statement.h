#pragma once

#include <vector>
#include <cstdint>

#include "types.h"
#include "Expression.h"

enum class StatementType
{
    None,
    TWO_REGISTER_OPCODE,
    ONE_REGISTER_OPCODE_AND_EXPRESSION,
    OPCODE_WITH_EXPRESSION,
    PSEDUO_OP_WITH_EXPRESSION,
    STANDALONE_OPCODE,
    LABEL,
    TIMES,
    EQU,
};

struct Statement
{
    int          lineNum;

    PseudoOp    pseudoOp;
    OpCode      opcode;

    Register    regSrc;
    Register    regDest;
    Expression  expression;

    StatementType type;

    char*       label;

    std::vector<uint16_t> assembledBytes;
    uint32_t address;

    Statement* timeStatement = nullptr;

    void firstPassAssemble(uint32_t &curAddress);
    void assemble(uint32_t &curAddress);

    void reset();

};
