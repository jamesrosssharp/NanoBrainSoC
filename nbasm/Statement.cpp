#include "Statement.h"

#include "Expression.h"

void Statement::reset()
{
    lineNum = 0;
    expression.reset();
    pseudoOp = PseudoOp::None;
    opcode = OpCode::None;
    type = StatementType::None;
    label = nullptr;
    regSrc = Register::None;
    regDest = Register::None;
    timeStatement = nullptr;
    assembledBytes.clear();
    address = 0;
}

void Statement::firstPassAssemble(uint32_t& curAddress)
{

}

void Statement::assemble(uint32_t &curAddress)
{

}

