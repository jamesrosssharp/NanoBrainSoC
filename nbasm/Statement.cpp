#include "Statement.h"

#include "Expression.h"
#include "Assembly.h"

#include <string.h>
#include <sstream>

void Statement::reset()
{
    lineNum  = 0;
    expression.reset();
    pseudoOp = PseudoOp::None;
    opcode	 = OpCode::None;
    type	 = StatementType::None;
    label	 = nullptr;
    regSrc	 = Register::None;
    regDest	 = Register::None;
    timesStatement = nullptr;
    repetitionCount = 1;
    assembledWords.clear();
    address = 0;
}

void Statement::firstPassAssemble(uint32_t& curAddress, SymbolTable& syms)
{

    address = curAddress;

    // Create dummy bytes for statement

    if (timesStatement != nullptr)
    {
        repetitionCount = timesStatement->expression.value;
    }

    switch (type)
    {
        case StatementType::STANDALONE_OPCODE:
        case StatementType::TWO_REGISTER_OPCODE:
        {
            // shouldn't get here - this should be assembled directly

            std::stringstream ss;
            ss << "Error: not directly assembling line " << lineNum << std::endl;
            throw std::runtime_error(ss.str());

            break;
        }
        case StatementType::ONE_REGISTER_OPCODE_AND_EXPRESSION:
        case StatementType::OPCODE_WITH_EXPRESSION:
        {

            // Assembly might have 1 or two bytes, depending if an imm instruction is needed


            // can expression be evaluated? If so, see if it's possible to fit any immediate value into
            // a 4 bit nibble, in which case we don't need an imm

            bool expressionCanFitIn4Bits = false;

            int32_t exprValue = 0;

            if (expression.evaluate(exprValue, syms))
            {
                if (exprValue >= 0 && exprValue < 16)
                    expressionCanFitIn4Bits = true;
            }

            switch (opcode)
            {
                case OpCode::IMM:
                case OpCode::NOP:
                case OpCode::SLEEP:
                    // these instructions are all single word
                    assembledWords.resize(1 * repetitionCount);
                    break;
                case OpCode::ADD:
                case OpCode::ADC:
                case OpCode::SUB:
                case OpCode::SBB:
                case OpCode::AND:
                case OpCode::OR:
                case OpCode::XOR:
                case OpCode::CMP:
                case OpCode::TEST:
                case OpCode::LOAD:
                case OpCode::MUL:
                case OpCode::MULS:
                case OpCode::DIV:
                case OpCode::DIVS:
                    if (expressionCanFitIn4Bits)
                        assembledWords.resize(1 * repetitionCount);
                    else
                        assembledWords.resize(2 * repetitionCount);
                    break;
                case OpCode::BSL:
                case OpCode::BSR:
                    // barrel shift instructions are single word
                    assembledWords.resize(1 * repetitionCount);
                    break;
                case OpCode::JUMP:
                case OpCode::JUMPZ:
                case OpCode::JUMPC:
                case OpCode::JUMPNZ:
                case OpCode::JUMPNC:
                case OpCode::CALL:
                case OpCode::CALLZ:
                case OpCode::CALLC:
                case OpCode::CALLNZ:
                case OpCode::CALLNC:
                    // jumps and calls can be made relative. For now, assign 2 words to
                    // instruction as if it were absolute. We will optimise it to a single
                    // byte later on.
                    assembledWords.resize(2 * repetitionCount);
                    break;
                /*
                SVC:
                RET:
                RETI:
                RETE:
                LDW:
                STW:
                LDSPR:
                STSPR:
                case OpCode::SLA:
                case OpCode::SLX:
                case OpCode::SL0:
                case OpCode::SL1:
                case OpCode::RL:
                case OpCode::SRA:
                case OpCode::SRX:
                case OpCode::SR0:
                case OpCode::SR1:
                case OpCode::RR:
                    break;
                */
                default:
                {
                    std::stringstream ss;
                    ss << "Error: opcode cannot take expression on line " << lineNum << std::endl;
                    throw std::runtime_error(ss.str());
                }
            }

            break;
        }
        case StatementType::PSEUDO_OP_WITH_EXPRESSION:

            switch (pseudoOp)
            {
                case PseudoOp::ALIGN:
                {
                    // No times used with align

                    if (repetitionCount != 1)
                    {
                        std::stringstream ss;
                        ss << "Error: times used with align on line " << lineNum << std::endl;
                        throw std::runtime_error(ss.str());
                    }

                    // check alignment expression is evaluable
                    // and that it is a multiple of sixteen bit
                    // words


                    int32_t exprValue = 0;

                    if (! expression.evaluate(exprValue, syms))
                    {
                        std::stringstream ss;
                        ss << "Error: could not evaluate align expression on line " << lineNum << std::endl;
                        throw std::runtime_error(ss.str());
                    }

                    if ((exprValue < 0) || ((exprValue & 1) != 0))
                    {
                        std::stringstream ss;
                        ss << "Error: invalid alignment expression on line " << lineNum << std::endl;
                        throw std::runtime_error(ss.str());
                    }

                    assembledWords.resize((exprValue - curAddress%exprValue) >> 1);

                    break;
                }
                case PseudoOp::DW:

                    // If DW string literal, then set the word count to the length of the string

                    char* stringLit;

                    if (expression.isStringLiteral(stringLit))
                    {
                        assembledWords.resize(repetitionCount * strlen(stringLit));
                    }
                    else
                    {
                        assembledWords.resize(1 * repetitionCount);
                    }
                    break;
                case PseudoOp::DD:
                    if (expression.isStringLiteral(stringLit))
                    {
                        assembledWords.resize(2 * repetitionCount * strlen(stringLit));
                    }
                    else
                    {
                        assembledWords.resize(2 * repetitionCount);
                    }
                    break;
            }

            break;
        default:
            break;
    }

    curAddress += assembledWords.size() * 2;

}

void Statement::assemble(uint32_t &curAddress)
{
    address = curAddress;

    // Create dummy bytes for statement

    if (timesStatement != nullptr)
    {
        repetitionCount = timesStatement->expression.value;
    }

    switch (type)
    {
        case StatementType::STANDALONE_OPCODE:
        {
            switch (opcode)
            {
                case OpCode::RET:
                {
                    assembledWords.resize(1);
                    assembledWords[0] =  0b1011111000000000;
                    break;
                }
                case OpCode::RETI:
                {
                    assembledWords.resize(1);
                    assembledWords[0] =  0b1011111000000001;
                    break;
                }
                case OpCode::NOP:
                {
                    assembledWords.resize(1);
                    assembledWords[0] =  0b0100011100000000;
                    break;
                }
                case OpCode::SLEEP:
                {
                    assembledWords.resize(1);
                    assembledWords[0] =  0b0100111100000000;
                    break;
                }
                default:
                {
                    std::stringstream ss;
                    ss << "Error: opcode is not stand-alone on line " << lineNum << std::endl;
                    throw std::runtime_error(ss.str());

                    break;
                }
            }

            break;
        }
        case StatementType::TWO_REGISTER_OPCODE:
        {

            switch (opcode)
            {
                case OpCode::ADD:
                    break;
                case OpCode::ADC:
                    break;
                case OpCode::SUB:
                    break;
                case OpCode::SBB:
                    break;
                case OpCode::AND:
                    break;
                case OpCode::OR:
                    break;
                case OpCode::XOR:
                    break;
                case OpCode::CMP:
                    break;
                case OpCode::TEST:
                    break;
                case OpCode::LOAD:
                {
                    uint32_t op = 0b0100101000000000;

                    op |= ((uint32_t)regDest) << 4;
                    op |= ((uint32_t)regSrc);

                    assembledWords.resize(1);
                    assembledWords[0] = op;

                    break;
                }
                case OpCode::MUL:
                    break;
                case OpCode::MULS:
                    break;
                case OpCode::DIV:
                    break;
                case OpCode::DIVS:
                    break;
                case OpCode::BSL:
                    break;
                case OpCode::BSR:
                    break;
                case OpCode::FMUL:
                    break;
                case OpCode::FDIV:
                    break;
                case OpCode::FADD:
                    break;
                case OpCode::FSUB:
                    break;
                case OpCode::FCMP:
                    break;
                case OpCode::FINT:
                    break;
                case OpCode::FFLT:
                    break;
                case OpCode::LDSPR:
                {
                    uint16_t op = Assembly::LDSPR_INSTRUCTION;

                    if ((uint32_t)regDest < 16)
                    {
                        std::stringstream ss;
                        ss << "Error: dest register is not an SPR on line " << lineNum << std::endl;
                        throw std::runtime_error(ss.str());
                    }

                    if ((uint32_t)regSrc > 15)
                    {
                        std::stringstream ss;
                        ss << "Error: source register is not a GPR on line " << lineNum << std::endl;
                        throw std::runtime_error(ss.str());
                    }

                    if ((uint32_t)regSrc & 1)
                    {
                        std::stringstream ss;
                        ss << "Error: source register must be r0, r2, r4, r6, r8, r10, r12, or r14 on line " << lineNum << std::endl;
                        throw std::runtime_error(ss.str());
                    }

                    op |= ((uint16_t)regDest - 16) << 4;
                    op |= (uint16_t)regSrc & 0xe;

                    assembledWords.resize(1);
                    assembledWords[0] = op;

                    break;
                }
                case OpCode::STSPR:
                    break;
                case OpCode::OUT:
                {
                    uint16_t op = 0b1111110000000000;

                    op |= ((uint16_t)regDest) << 4;
                    op |= ((uint16_t)regSrc);

                    assembledWords.resize(1);
                    assembledWords[0] = op;

                    break;
                }
                case OpCode::IN:
                    break;
            }

            break;
        }
        case StatementType::ONE_REGISTER_OPCODE_AND_EXPRESSION:
        {
            switch (opcode)
            {
                case OpCode::ADD:
                    Assembly::makeArithmeticInstructionWithImmediate(0b0110000000000000, regDest, expression.value, assembledWords, lineNum);
                    break;
                case OpCode::ADC:
                    Assembly::makeArithmeticInstructionWithImmediate(0b0111000000000000, regDest, expression.value, assembledWords, lineNum);
                    break;
                case OpCode::SUB:
                    Assembly::makeArithmeticInstructionWithImmediate(0b0110100000000000, regDest, expression.value, assembledWords, lineNum);
                    break;
                case OpCode::SBB:
                    Assembly::makeArithmeticInstructionWithImmediate(0b0111100000000000, regDest, expression.value, assembledWords, lineNum);
                    break;
                case OpCode::AND:
                    Assembly::makeArithmeticInstructionWithImmediate(0b0110010000000000, regDest, expression.value, assembledWords, lineNum);
                    break;
                case OpCode::OR:
                    Assembly::makeArithmeticInstructionWithImmediate(0b0111010000000000, regDest, expression.value, assembledWords, lineNum);
                    break;
                case OpCode::XOR:
                    Assembly::makeArithmeticInstructionWithImmediate(0b0110110000000000, regDest, expression.value, assembledWords, lineNum);
                    break;
                case OpCode::CMP:
                    Assembly::makeArithmeticInstructionWithImmediate(0b0110001000000000, regDest, expression.value, assembledWords, lineNum);
                    break;
                case OpCode::TEST:
                    Assembly::makeArithmeticInstructionWithImmediate(0b0111001000000000, regDest, expression.value, assembledWords, lineNum);
                    break;
                case OpCode::LOAD:
                    Assembly::makeArithmeticInstructionWithImmediate(0b0110101000000000, regDest, expression.value, assembledWords, lineNum);
                    break;
                case OpCode::MUL:
                    Assembly::makeArithmeticInstructionWithImmediate(0b0110011000000000, regDest, expression.value, assembledWords, lineNum);
                    break;
                case OpCode::MULS:
                    Assembly::makeArithmeticInstructionWithImmediate(0b0111011000000000, regDest, expression.value, assembledWords, lineNum);
                    break;
                case OpCode::DIV:
                    Assembly::makeArithmeticInstructionWithImmediate(0b0110111000000000, regDest, expression.value, assembledWords, lineNum);
                    break;
                case OpCode::DIVS:
                    Assembly::makeArithmeticInstructionWithImmediate(0b0111111000000000, regDest, expression.value, assembledWords, lineNum);
                    break;
                case OpCode::BSL:
                    Assembly::makeArithmeticInstructionWithImmediate(0b0100000100000000, regDest, expression.value, assembledWords, lineNum);
                    break;
                case OpCode::BSR:
                    Assembly::makeArithmeticInstructionWithImmediate(0b0110000100000000, regDest, expression.value, assembledWords, lineNum);
                    break;
                default:
                {
                    std::stringstream ss;
                    ss << "Error: opcode is not single register with expression on line " << lineNum << std::endl;
                    throw std::runtime_error(ss.str());

                    break;
                }
            }

            break;
        }
        case StatementType::OPCODE_WITH_EXPRESSION:
        {

            switch (opcode)
            {
                case OpCode::JUMP:
                case OpCode::JUMPZ:
                case OpCode::JUMPC:
                case OpCode::JUMPNZ:
                case OpCode::JUMPNC:
                case OpCode::CALL:
                case OpCode::CALLZ:
                case OpCode::CALLC:
                case OpCode::CALLNZ:
                case OpCode::CALLNC:
                    Assembly::makeFlowControlInstruction(opcode, address, expression.value, lineNum, assembledWords);
                    break;
                default:
                {
                    std::stringstream ss;
                    ss << "Error: opcode is not standalone with expression on line " << lineNum << std::endl;
                    throw std::runtime_error(ss.str());

                    break;
                }
            }

            break;
        }
        case StatementType::PSEUDO_OP_WITH_EXPRESSION:
        {


            break;
        }
        default:
            break;
    }

    curAddress += assembledWords.size() * 2;



}

