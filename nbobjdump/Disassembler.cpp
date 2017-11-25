#include "Disassembler.h"
#include "types.h"
#include "nbInstructionDecodeTable.h"

#include <sstream>
#include <iomanip>
#include <iostream>

void Disassembler::disassembleWord(std::uint32_t curAddress, std::uint16_t word)
{

    std::stringstream ins;

    UniqueOpCode opcode = UniqueOpCode::None;

    ins << std::hex << std::setw(6) << std::setfill('0') << curAddress << " : " << std::setw(4) << word << " :: ";

    for (const nbInstructionDecodeInfo& info : instructionInfo)
    {

        if ((word & info.mask) == info.instruction)
        {
            ins << info.string << " ";
            opcode = info.opcode;
            break;
        }

    }

    bool immSet = false;

    switch (opcode)
    {
        case UniqueOpCode::IMM:

            ins << "0x" << std::hex << std::setw(4) << std::setfill('0') << (word & 0x3fff);

            m_imm = word & 0x3fff;

            immSet = true;

            break;
        case UniqueOpCode::ADD_IMM:
        case UniqueOpCode::ADC_IMM:
        case UniqueOpCode::SUB_IMM:
        case UniqueOpCode::SBB_IMM:
        case UniqueOpCode::AND_IMM:
        case UniqueOpCode::OR_IMM:
        case UniqueOpCode::XOR_IMM:
        case UniqueOpCode::CMP_IMM:
        case UniqueOpCode::TEST_IMM:
        case UniqueOpCode::LOAD_IMM:
        case UniqueOpCode::MUL_IMM:
        case UniqueOpCode::MULS_IMM:
        case UniqueOpCode::DIV_IMM:
        case UniqueOpCode::DIVS_IMM:

            ins << (Register)((word & 0xf0) >> 4) << ", 0x" << std::hex << std::setw(1) <<
                   (word & 0x0f) << " // " << std::hex << std::setw(4) << (((m_imm & 0xfff) << 4) | word & 0x0f);

            break;
        case UniqueOpCode::ADD_REG:
        case UniqueOpCode::ADC_REG:
        case UniqueOpCode::SUB_REG:
        case UniqueOpCode::SBB_REG:
        case UniqueOpCode::AND_REG:
        case UniqueOpCode::OR_REG:
        case UniqueOpCode::XOR_REG:
        case UniqueOpCode::CMP_REG:
        case UniqueOpCode::TEST_REG:
        case UniqueOpCode::LOAD_REG:
        case UniqueOpCode::MUL_REG:
        case UniqueOpCode::MULS_REG:
        case UniqueOpCode::DIV_REG:
        case UniqueOpCode::DIVS_REG:
        case UniqueOpCode::OUT:
        case UniqueOpCode::IN:

            ins <<  (Register)((word & 0xf0) >> 4) << "," <<
                    (Register)(word & 0x0f);

            break;
        case UniqueOpCode::SLA:
        case UniqueOpCode::SLX:
        case UniqueOpCode::SL0:
        case UniqueOpCode::SL1:
        case UniqueOpCode::RL:
        case UniqueOpCode::SRA:
        case UniqueOpCode::SRX:
        case UniqueOpCode::SR0:
        case UniqueOpCode::SR1:
        case UniqueOpCode::RR:

            ins <<  (Register)((word & 0xf0) >> 4);
            break;
        case UniqueOpCode::BSL:
        case UniqueOpCode::BSR:

            ins << (Register)((word & 0xf0) >> 4);
            break;
        case UniqueOpCode::FMUL:
        case UniqueOpCode::FDIV:
        case UniqueOpCode::FADD:
        case UniqueOpCode::FSUB:
        case UniqueOpCode::FCMP:
        case UniqueOpCode::FINT:
        case UniqueOpCode::FFLT:


            break;
        case UniqueOpCode::NOP:
        case UniqueOpCode::SLEEP:

            break;
        case UniqueOpCode::JUMP:
        case UniqueOpCode::JUMPZ:
        case UniqueOpCode::JUMPC:
        case UniqueOpCode::JUMPNZ:
        case UniqueOpCode::JUMPNC:
        case UniqueOpCode::CALL:
        case UniqueOpCode::CALLZ:
        case UniqueOpCode::CALLC:
        case UniqueOpCode::CALLNZ:
        case UniqueOpCode::CALLNC:

            ins << "0x" << std::hex << std::setw(6) << ((word & 0x1ff) + m_imm);
            break;
        case UniqueOpCode::JUMP_REL:
        case UniqueOpCode::JUMPZ_REL:
        case UniqueOpCode::JUMPC_REL:
        case UniqueOpCode::JUMPNZ_REL:
        case UniqueOpCode::JUMPNC_REL:
        case UniqueOpCode::CALL_REL:
        case UniqueOpCode::CALLZ_REL:
        case UniqueOpCode::CALLC_REL:
        case UniqueOpCode::CALLNZ_REL:
        case UniqueOpCode::CALLNC_REL:

            ins << "0x" << std::hex << std::setw(6) <<
                   (((word & 0x100) ? (int16_t)((word & 0x1ff) | 0xfe00) : (int16_t)(word & 0x1ff)) + curAddress);
            break;
        case UniqueOpCode::SVC:
            break;
        case UniqueOpCode::RET:
        case UniqueOpCode::RETI:
        case UniqueOpCode::RETE:
            break;
        case UniqueOpCode::LDW_REG:
        case UniqueOpCode::STW_REG:

            ins << (Register)((word & 0xf0) >> 4) << ", [" << (Register)(((word & 0x300) >> 8) + 24) << " , " <<
                   (Register)(word & 0x0f) << "]";

            break;
        case UniqueOpCode::STW_IMM:
        case UniqueOpCode::LDW_IMM:

            ins << (Register)((word & 0xf0) >> 4) << ", [" << (Register)(((word & 0x300) >> 8) + 24) << " , " <<
                   (word & 0x0f) << "]" << " // 0x" << (m_imm + word & 0x0f);

            break;
        case UniqueOpCode::LDSPR:

            ins <<  (Register)(((word & 0xf0) >> 4) + 16) << "," <<
                    (Register)(word & 0x0f);

            break;
        case UniqueOpCode::STSPR:
            ins <<  (Register)((word & 0xf0) >> 4) << "," <<
                    (Register)((word & 0x0f) + 16);

            break;
        case UniqueOpCode::INCW:
        case UniqueOpCode::DECW:

            ins <<  (Register)(((word & 0xf0) >> 4) + 16);

            break;
    }

    if (! immSet)
        m_imm = 0;

    std::cout << ins.str() << std::endl;


}
