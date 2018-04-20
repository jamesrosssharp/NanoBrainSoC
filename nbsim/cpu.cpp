#include "cpu.h"

#include "nbInstructionSet.h"
#include "nbInstructionDecodeTable.h"
#include "types.h"

#include <time.h>
#include <unistd.h>

#include <sstream>
#include <iomanip>

#include <stdlib.h>
#include <string.h>

//#define DEBUG_INSTRUCTIONS

CPU::CPU(Memory &mem, IOPorts& ioports) :
    m_memory(mem),
    m_ioports(ioports),
    m_cpuThread([] (CPU* cpu) { cpu->runThread(); }, this),
    m_threadExit(false),
    m_threadPause(false),
    m_sleep(false),
    m_singleStep(false),
    m_paused(true)
{
        m_ioports.setCPUInterruptDelegate(this);
}

void CPU::hardReset()
{
    std::unique_lock<std::mutex> lock(m_mutex);

    m_pc = 0;
    m_imm = 0;
    m_C = false;
    m_Z = false;

    // Zero registers
    for (int i = 0; i < 16; i ++)
    {
        m_gprRegisters[i] = 0;
        m_sprRegisters[i] = 0;
    }

    m_interrupt = 0;
    m_exception = 0;
    m_svc = 0;

    m_ioports.hardReset();
}

void CPU::run()
{
    // Spawn a thread that will execute instructions at 50MHz

    m_cond.notify_all();

}

void CPU::pause()
{
    std::unique_lock<std::mutex> lock(m_mutex);

    if (m_paused) return;

    m_threadPause = true;
    m_cond.notify_all();
    m_pauseCond.wait(lock);
}

void CPU::singleStep()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    m_singleStep = true;
    m_cond.notify_all();
    m_singleStepCond.wait(lock);
}

void CPU::shutDown()
{
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_threadExit = true;
    }

    m_cond.notify_all();
    m_cpuThread.join();
}

std::string CPU::dumpRegisters()
{
    std::unique_lock<std::mutex> lock(m_mutex);

    std::stringstream ss;

    ss << "pc = " << std::hex << std::setw(4) << std::setfill('0') << (m_pc << 1) <<
          " C=" << m_C << " Z=" << m_Z << std::endl;

    for (int i = 0; i < 16; i ++)
    {
        if (i && (i%8 == 0))
            ss << std::endl;
        ss << "r" << std::dec << std::setw(2) << std::setfill('0') << i << "=" << std::hex << std::setw(4) << std::setfill('0') << m_gprRegisters[i] << " ";
    }

    ss << std::endl;

    for (int i = 0; i < 16; i ++)
    {
        if (i && (i%4 == 0))
            ss << std::endl;
        ss << "s" << std::dec << std::setw(2) << i << "=" << std::hex << std::setw(8) << std::setfill('0') << m_sprRegisters[i] << " ";
    }

    return ss.str();
}

std::string CPU::dumpDisas()
{
    std::stringstream ss;
    std::unique_lock<std::mutex> lock(m_mutex);

    uint32_t immediate = 0;

    for (int pc = std::max(0, (int)m_pc - 2*7); pc < m_pc + 2*7; pc ++)
    {
        uint16_t instruction = m_memory.readWord(pc);

        UniqueOpCode opcode = UniqueOpCode::None;

        if (pc == m_pc)
            ss << "==>";
        else
            ss << "   ";

        ss << std::hex << std::setw(6) << std::setfill('0') << (pc << 1) << ":\t";
        ss << std::hex << std::setw(4) << std::setfill('0') << instruction << "\t";

        for (const nbInstructionDecodeInfo& info : instructionInfo)
        {
            if ((instruction & info.mask) == info.instruction)
            {
                ss << info.string;

                for (int i = 0; i < 8 - strlen(info.string); i++)
                    ss << " ";

                opcode = info.opcode;
                break;
            }
        }

        // Determine how to format parameters

        bool immSet = false;

        switch (opcode)
        {
            case UniqueOpCode::IMM:
                immSet = true;
                immediate = (instruction & 0x3fff);

                ss << std::hex << immediate;
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
            {
                int regx = (instruction & 0xf0) >> 4;
                uint32_t immval = (instruction & 0x0f);

                ss << (Register)regx << ", " << std::hex << immval << "\t; " << ((immediate << 4) | immval);

                break;
            }
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
            {
                int regx = (instruction & 0xf0) >> 4;
                int regy = (instruction & 0x0f);

                ss << (Register)regx << ", " << (Register)regy;

                break;
            }
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
            {
                int regx = (instruction & 0xf0) >> 4;

                ss << (Register)regx;

                break;
            }
            case UniqueOpCode::BSL:
            case UniqueOpCode::BSR:
            {
                int regx = (instruction & 0xf0) >> 4;
                int immval = (instruction & 0x0f);

                ss << (Register)regx << ", " << (Register)immval;

                break;
            }
            case UniqueOpCode::FMUL:
            case UniqueOpCode::FDIV:
            case UniqueOpCode::FADD:
            case UniqueOpCode::FSUB:
            case UniqueOpCode::FCMP:
            case UniqueOpCode::FINT:
            case UniqueOpCode::FFLT:
            {
                break;
            }
            case UniqueOpCode::NOP:
            case UniqueOpCode::SLEEP:
            {
                break;
            }
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
            {
                uint32_t addr = (instruction & 0x1ff);

                ss << addr << "\t;" << (((immediate << 9) | addr) << 1);

                break;
            }
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
            {
                int32_t addr = (instruction & 0x1ff);

                if (addr & 0x100)
                    addr |= 0xfe00;

                ss << addr << "\t; " << ((pc + (int)addr) << 1);

                break;
            }
            case UniqueOpCode::SVC:
            case UniqueOpCode::RET:
            case UniqueOpCode::RETI:
            case UniqueOpCode::RETE:
            {
                break;
            }
            case UniqueOpCode::LDW_IMM:
            case UniqueOpCode::STW_IMM:
            {
                int regx = (instruction & 0xf0) >> 4;
                int immVal = (immediate << 4) | (instruction & 0x0f);
                int regi = (instruction & 0x300) >> 8;

                ss << (Register)regx << ", [" << (Register)(regi + 16 + 8) << ", " << immVal << "]";

                break;
            }
            case UniqueOpCode::LDW_REG:
            case UniqueOpCode::STW_REG:
            {
                int regx = (instruction & 0xf0) >> 4;
                int regy = (instruction & 0x0f);
                int regi = (instruction & 0x300) >> 8;

                ss << (Register)regx << ", [" << (Register)(regi + 16 + 8) << ", " << (Register)regy << "]";

                break;
            }
            case UniqueOpCode::LDSPR:
            {
                int regx = (instruction & 0xe0) >> 4;
                int regy = (instruction & 0x0f);

                ss << (Register)(regx + 16) << ", " << (Register)regy;

                break;
            }
            case UniqueOpCode::STSPR:
            {
                int regx = (instruction & 0xe0) >> 4;
                int regy = (instruction & 0x0f);

                ss << (Register)regx << ", " << (Register)(regy + 16);

                break;
            }
            case UniqueOpCode::INCW:
            case UniqueOpCode::DECW:
            {
                int regx = (instruction & 0xf0) >> 4;

                ss << (Register)(regx + 16);

                break;
            }
        }

        if (! immSet)
            immediate = 0;

        ss << std::endl;
    }

    return ss.str();

}

void CPU::setIRQ(bool level)
{
    if (level)
    {
        m_interrupt = true;
        // wake sleeping thread.
        if (! m_paused)
            m_cond.notify_all();
    }
    else
    {
        m_interrupt = false;
        // wake sleeping thread.
        if (! m_paused)
            m_cond.notify_all();
    }
}

void CPU::runThread()
{
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_cond.wait(lock);
    }

    while (! m_threadExit)
    {

        {
            std::unique_lock<std::mutex> lock(m_mutex);

            if (m_sleep)
            {
                if (! m_paused)
                    m_cond.wait(lock);
                m_sleep = false;
            }
        }

        {
            std::unique_lock<std::mutex> lock(m_mutex);

            if (m_threadPause)
            {
                m_paused = true;
                m_threadPause = false;
                m_pauseCond.notify_all();
                m_cond.wait(lock);
            }
        }

        {
            std::unique_lock<std::mutex> lock(m_mutex);

            m_paused = false;
            clockTick();
        }

        struct timespec tm1, tm2;

        tm1.tv_sec = 0;
        tm1.tv_nsec = 20;
        nanosleep(&tm1, &tm2);
        //usleep(1000);

        {
            std::unique_lock<std::mutex> lock(m_mutex);
            if (m_singleStep)
            {
                m_singleStep = false;
                m_paused = true;
                m_singleStepCond.notify_all();
                m_cond.wait(lock);
            }
        }
    }

}

void CPU::holdInReset(bool hold)
{
    if (hold)
    {
        m_threadPause = true;
        while (m_threadPause)
            usleep(100);
        hardReset();
    }
    else
    {
        m_cond.notify_all();
    }
}

void CPU::clockTick()
{

    if (m_interrupt && (m_sprRegisters[SPR_MSR] & MSR_IE))
    {

        std::uint32_t vbar = m_sprRegisters[SPR_VBAR];

        std::uint32_t addr = vbar + 2; // interrupt vector

        m_sprRegisters[SPR_ILR] = m_pc;
        m_sprRegisters[SPR_MSR] &= ~MSR_IE; // disable further interrupts.
        m_pc = addr;

    }
    else if (m_exception && (m_sprRegisters[SPR_MSR] & MSR_EE))
    {

    }
    else if (m_svc)
    {

    }
    else
    {
        fetchInstruction();
        executeInstruction();
    }

}

void CPU::fetchInstruction()
{
    m_instruction = m_memory.readWord(m_pc);

}

void CPU::executeInstruction()
{

    std::stringstream instructionText;

    #ifdef DEBUG_INSTRUCTIONS
        #define MAKE_DBG(x) instructionText << x
    #else
        #define MAKE_DBG(x)
    #endif

    MAKE_DBG (std::hex << std::setw(6) << std::setfill('0') << (m_pc << 1) << " ");
    MAKE_DBG (std::hex << std::setw(6) << std::setfill('0') << m_instruction << " ");

    UniqueOpCode opcode = UniqueOpCode::None;

    for (const nbInstructionDecodeInfo& info : instructionInfo)
    {
        if ((m_instruction & info.mask) == info.instruction)
        {
            MAKE_DBG (info.string << " ");
            opcode = info.opcode;
            break;
        }
    }

    std::uint32_t pcNext = m_pc + 1;

    std::uint16_t immNext = 0;

    // Calculate jump targets
    std::uint32_t jumpTarget = (m_imm << 9) | (m_instruction & 0x1ff);

    std::int16_t rel = m_instruction & 0x1ff;
    if (rel & 0x100)
    {
        rel |= 0xfe00;
    }

    std::uint32_t jumpTargetRel = (std::int32_t)m_pc + rel;

    switch (opcode)
    {
        case UniqueOpCode::IMM:
            immNext = m_instruction;

            MAKE_DBG(std::hex << std::setw(4) << std::setfill('0') << immNext);

            break;
        case UniqueOpCode::ADD_IMM:
        {
            int regx = (m_instruction & 0xf0) >> 4;
            int immval = ((m_imm & 0xfff) << 4) | m_instruction & 0x0f;

            std::uint32_t accu = m_gprRegisters[regx];
            accu += immval;

            m_C = accu & 0x10000;
            m_Z = ! (accu & 0xffff);

            m_gprRegisters[regx] = accu & 0xffff;

            MAKE_DBG((Register)regx << "," << immval);

            break;
        }
        case UniqueOpCode::ADD_REG:
        {
            int regx = (m_instruction & 0xf0) >> 4;
            int regy = m_instruction & 0x0f;

            std::uint32_t accu = m_gprRegisters[regx];
            accu              += m_gprRegisters[regy];

            m_C = accu & 0x10000;
            m_Z = ! (accu & 0xffff);

            m_gprRegisters[regx] = accu & 0xffff;

            MAKE_DBG((Register)regx << "," << (Register)regy);

            break;
        }
        case UniqueOpCode::ADC_IMM:
        {
            int regx = (m_instruction & 0xf0) >> 4;
            int immval = ((m_imm & 0xfff) << 4) | m_instruction & 0x0f;

            std::uint32_t accu = m_gprRegisters[regx];
            accu += immval;

            if (m_C)
                accu ++;

            m_C = accu & 0x10000;
            m_Z = ! (accu & 0xffff);

            m_gprRegisters[regx] = accu & 0xffff;

            MAKE_DBG((Register)regx << "," << immval);

            break;
        }
        case UniqueOpCode::ADC_REG:
        {
            int regx = (m_instruction & 0xf0) >> 4;
            int regy = m_instruction & 0x0f;

            std::uint32_t accu = m_gprRegisters[regx];
            accu              += m_gprRegisters[regy];

            if (m_C)
                accu ++;

            m_C = accu & 0x10000;
            m_Z = ! (accu & 0xffff);

            m_gprRegisters[regx] = accu & 0xffff;

            MAKE_DBG((Register)regx << "," << (Register)regy);

            break;
        }
        case UniqueOpCode::SUB_IMM:
        {
            int regx = (m_instruction & 0xf0) >> 4;
            int immval = ((m_imm & 0xfff) << 4) | m_instruction & 0x0f;

            std::uint32_t accu = m_gprRegisters[regx];
            accu -= immval;

            m_C = accu & 0x10000;
            m_Z = ! (accu & 0xffff);

            m_gprRegisters[regx] = accu & 0xffff;

            MAKE_DBG((Register)regx << "," << immval);

            break;
        }
        case UniqueOpCode::SUB_REG:
        {
            int regx = (m_instruction & 0xf0) >> 4;
            int regy = m_instruction & 0x0f;

            std::uint32_t accu = m_gprRegisters[regx];
            accu              -= m_gprRegisters[regy];

            m_C = accu & 0x10000;
            m_Z = ! (accu & 0xffff);

            m_gprRegisters[regx] = accu & 0xffff;

            MAKE_DBG((Register)regx << "," << (Register)regy);

            break;
        }
        case UniqueOpCode::SBB_IMM:
        {
            int regx = (m_instruction & 0xf0) >> 4;
            int immval = ((m_imm & 0xfff) << 4) | m_instruction & 0x0f;

            std::uint32_t accu = m_gprRegisters[regx];
            accu -= immval;

            if (m_C)
                accu --;

            m_C = accu & 0x10000;
            m_Z = ! (accu & 0xffff);

            m_gprRegisters[regx] = accu & 0xffff;

            MAKE_DBG((Register)regx << "," << immval);

            break;
        }
        case UniqueOpCode::SBB_REG:
        {
            int regx = (m_instruction & 0xf0) >> 4;
            int regy = m_instruction & 0x0f;

            std::uint32_t accu = m_gprRegisters[regx];
            accu              -= m_gprRegisters[regy];

            if (m_C)
                accu --;

            m_C = accu & 0x10000;
            m_Z = ! (accu & 0xffff);

            m_gprRegisters[regx] = accu & 0xffff;

            MAKE_DBG((Register)regx << "," << (Register)regy);


            break;
        }
        case UniqueOpCode::AND_IMM:
        {
            int regx = (m_instruction & 0xf0) >> 4;
            int immval = ((m_imm & 0xfff) << 4) | m_instruction & 0x0f;

            std::uint32_t accu = m_gprRegisters[regx];
            accu &= immval;

            m_C = accu & 0x10000;
            m_Z = ! (accu & 0xffff);

            m_gprRegisters[regx] = accu & 0xffff;

            MAKE_DBG((Register)regx << "," << immval);


            break;
        }
        case UniqueOpCode::AND_REG:
        {
            int regx = (m_instruction & 0xf0) >> 4;
            int regy = m_instruction & 0x0f;

            std::uint32_t accu = m_gprRegisters[regx];
            accu              &= m_gprRegisters[regy];

            m_C = accu & 0x10000;
            m_Z = ! (accu & 0xffff);

            m_gprRegisters[regx] = accu & 0xffff;

            MAKE_DBG((Register)regx << "," << (Register)regy);

            break;
        }
        case UniqueOpCode::OR_IMM:
        {
            int regx = (m_instruction & 0xf0) >> 4;
            int immval = ((m_imm & 0xfff) << 4) | m_instruction & 0x0f;

            std::uint32_t accu = m_gprRegisters[regx];
            accu |= immval;

            m_C = accu & 0x10000;
            m_Z = ! (accu & 0xffff);

            m_gprRegisters[regx] = accu & 0xffff;

            MAKE_DBG((Register)regx << "," << immval);

            break;
        }
        case UniqueOpCode::OR_REG:
        {
            int regx = (m_instruction & 0xf0) >> 4;
            int regy = m_instruction & 0x0f;

            std::uint32_t accu = m_gprRegisters[regx];
            accu              |= m_gprRegisters[regy];

            m_C = accu & 0x10000;
            m_Z = ! (accu & 0xffff);

            m_gprRegisters[regx] = accu & 0xffff;

            MAKE_DBG((Register)regx << "," << (Register)regy);

            break;
        }
        case UniqueOpCode::XOR_IMM:
        {
            int regx = (m_instruction & 0xf0) >> 4;
            int immval = ((m_imm & 0xfff) << 4) | m_instruction & 0x0f;

            std::uint32_t accu = m_gprRegisters[regx];
            accu ^= immval;

            m_C = accu & 0x10000;
            m_Z = ! (accu & 0xffff);

            m_gprRegisters[regx] = accu & 0xffff;

            MAKE_DBG((Register)regx << "," << immval);

            break;
        }
        case UniqueOpCode::XOR_REG:
        {
            int regx = (m_instruction & 0xf0) >> 4;
            int regy = m_instruction & 0x0f;

            std::uint32_t accu = m_gprRegisters[regx];
            accu              ^= m_gprRegisters[regy];

            m_C = accu & 0x10000;
            m_Z = ! (accu & 0xffff);

            m_gprRegisters[regx] = accu & 0xffff;

            MAKE_DBG((Register)regx << "," << (Register)regy);

            break;
        }
        case UniqueOpCode::SLA:
        {
            int regx = (m_instruction & 0xf0) >> 4;

            std::uint16_t accu = m_gprRegisters[regx];

            std::uint16_t shift = (accu << 1);
            if (m_C)
                shift |= 1;
            m_C = accu & 0x8000;

            m_gprRegisters[regx] = shift;

            MAKE_DBG((Register)regx);

            break;
        }
        case UniqueOpCode::SLX:
        {
            int regx = (m_instruction & 0xf0) >> 4;

            std::uint16_t accu = m_gprRegisters[regx];

            std::uint16_t shift = (accu << 1) | (accu & 0x1);

            m_C = accu & 0x8000;

            m_gprRegisters[regx] = shift;

            MAKE_DBG((Register)regx);

            break;
        }
        case UniqueOpCode::SL0:
        {
            int regx = (m_instruction & 0xf0) >> 4;

            std::uint16_t accu = m_gprRegisters[regx];

            std::uint16_t shift = (accu << 1);

            m_C = accu & 0x8000;

            m_gprRegisters[regx] = shift;

            MAKE_DBG((Register)regx);

            break;
        }
        case UniqueOpCode::SL1:
        {
            int regx = (m_instruction & 0xf0) >> 4;

            std::uint16_t accu = m_gprRegisters[regx];

            std::uint16_t shift = (accu << 1) | 1;

            m_C = accu & 0x8000;

            m_gprRegisters[regx] = shift;

            MAKE_DBG((Register)regx);

            break;
        }
        case UniqueOpCode::RL:
        {
            int regx = (m_instruction & 0xf0) >> 4;

            std::uint16_t accu = m_gprRegisters[regx];

            std::uint16_t shift = (accu << 1) | m_C ? 1 : 0;

            m_C = accu & 0x8000;

            m_gprRegisters[regx] = shift;

            MAKE_DBG((Register)regx);

            break;
        }
        case UniqueOpCode::SRA:
        {
            int regx = (m_instruction & 0xf0) >> 4;

            std::uint16_t accu = m_gprRegisters[regx];

            std::uint16_t shift = (accu >> 1);
            if (m_C)
                shift |= 0x8000;
            m_C = accu & 0x0001;

            m_gprRegisters[regx] = shift;

            MAKE_DBG((Register)regx);

            break;
        }
        case UniqueOpCode::SRX:
        {
            int regx = (m_instruction & 0xf0) >> 4;

            std::uint16_t accu = m_gprRegisters[regx];

            std::uint16_t shift = (accu >> 1) | (accu & 0x8000);

            m_C = accu & 0x1;

            m_gprRegisters[regx] = shift;

            MAKE_DBG((Register)regx);

            break;
        }
        case UniqueOpCode::SR0:
        {
            int regx = (m_instruction & 0xf0) >> 4;

            std::uint16_t accu = m_gprRegisters[regx];

            std::uint16_t shift = (accu >> 1);

            m_C = accu & 0x1;

            m_gprRegisters[regx] = shift;

            MAKE_DBG((Register)regx);

            break;
        }
        case UniqueOpCode::SR1:
        {
            int regx = (m_instruction & 0xf0) >> 4;

            std::uint16_t accu = m_gprRegisters[regx];

            std::uint16_t shift = (accu >> 1) | 0x8000;

            m_C = accu & 1;

            m_gprRegisters[regx] = shift;

            MAKE_DBG((Register)regx);

            break;
        }
        case UniqueOpCode::RR:
        {
            int regx = (m_instruction & 0xf0) >> 4;

            std::uint16_t accu = m_gprRegisters[regx];

            std::uint16_t shift = (accu >> 1) | m_C ? 0x8000 : 0;

            m_C = accu & 0x1;

            m_gprRegisters[regx] = shift;

            MAKE_DBG((Register)regx);

            break;
        }
        case UniqueOpCode::CMP_IMM:
        {
            int regx = (m_instruction & 0xf0) >> 4;
            int immval = ((m_imm & 0xfff) << 4) | m_instruction & 0x0f;

            std::uint32_t accu = m_gprRegisters[regx];
            accu -= immval;

            m_C = accu & 0x10000;
            m_Z = ! (accu & 0xffff);

            MAKE_DBG((Register)regx << "," << immval);

            break;
        }
        case UniqueOpCode::CMP_REG:
        {
            int regx = (m_instruction & 0xf0) >> 4;
            int regy = m_instruction & 0x0f;

            std::uint32_t accu = m_gprRegisters[regx];
            accu              -= m_gprRegisters[regy];

            m_C = accu & 0x10000;
            m_Z = ! (accu & 0xffff);

            MAKE_DBG((Register)regx << "," << (Register)regy);

            break;
        }
        case UniqueOpCode::TEST_IMM:
        {
            int regx = (m_instruction & 0xf0) >> 4;
            int immval = ((m_imm & 0xfff) << 4) | m_instruction & 0x0f;

            std::uint32_t accu = m_gprRegisters[regx];
            accu &= immval;

            m_Z = ! accu;

            std::uint32_t carry = 0;

            for (int i = 0 ; i < 16; i++)
            {
                carry = carry ^ ((accu >> 1) & 0x1);
            }

            m_C = carry;

            MAKE_DBG((Register)regx << "," << immval);

            break;
        }
        case UniqueOpCode::TEST_REG:
        {
            int regx = (m_instruction & 0xf0) >> 4;
            int regy =  m_instruction & 0x0f;

            std::uint32_t accu = m_gprRegisters[regx];
            accu              &= m_gprRegisters[regy];

            m_Z = ! accu;

            std::uint32_t carry = 0;

            for (int i = 0 ; i < 16; i++)
            {
                carry = carry ^ ((accu >> 1) & 0x1);
            }

            m_C = carry;

            MAKE_DBG((Register)regx << "," << (Register)regy);

            break;
        }
        case UniqueOpCode::LOAD_IMM:
        {
            int regx = (m_instruction & 0xf0) >> 4;
            int immval = ((m_imm & 0xfff) << 4) | m_instruction & 0x0f;

            m_gprRegisters[regx] = immval;

            MAKE_DBG((Register)regx << "," << immval);

            break;
        }
        case UniqueOpCode::LOAD_REG:
        {
            int regx = (m_instruction & 0xf0) >> 4;
            int regy =  m_instruction & 0x0f;

            m_gprRegisters[regx] = m_gprRegisters[regy];

            MAKE_DBG((Register)regx << "," << (Register)regy);

            break;
        }
        case UniqueOpCode::MUL_IMM:
            break;
        case UniqueOpCode::MUL_REG:
            break;
        case UniqueOpCode::MULS_IMM:
            break;
        case UniqueOpCode::MULS_REG:
            break;
        case UniqueOpCode::DIV_IMM:
            break;
        case UniqueOpCode::DIV_REG:
            break;
        case UniqueOpCode::DIVS_IMM:
            break;
        case UniqueOpCode::DIVS_REG:
            break;
        case UniqueOpCode::BSL:
        {
            int regx = (m_instruction & 0xf0) >> 4;
            int shift =  m_instruction & 0x0f;

            std::uint32_t regVal = m_gprRegisters[regx];

            //printf("BSL: %x %x %x\n", regx, shift, regVal);

            m_gprRegisters[regx] = (regVal << shift) | ((regVal & ((1 << (16 - shift)) - 1)) >> (16 - shift));
            break;
        }
        case UniqueOpCode::BSR:
        {
            int regx = (m_instruction & 0xf0) >> 4;
            int shift =  m_instruction & 0x0f;

            std::uint32_t regVal = m_gprRegisters[regx];

            //printf("BSR: %x %x %04x\n", regx, shift, regVal);

            m_gprRegisters[regx] = (regVal >> shift) | ((regVal & ((1 << (16 - shift)) - 1)) << (16 - shift));

            //printf("BSR OUT: %x\n", m_gprRegisters[regx]);
            break;
        }
        case UniqueOpCode::FMUL:
            break;
        case UniqueOpCode::FDIV:
            break;
        case UniqueOpCode::FADD:
            break;
        case UniqueOpCode::FSUB:
            break;
        case UniqueOpCode::FCMP:
            break;
        case UniqueOpCode::FINT:
            break;
        case UniqueOpCode::FFLT:
            break;
        case UniqueOpCode::NOP:
            break;
        case UniqueOpCode::SLEEP:
            m_sleep = true;
            break;
        case UniqueOpCode::JUMP:
        {
            pcNext = jumpTarget;

            MAKE_DBG(std::hex << std::setw(6) << std::setfill('0') << (jumpTarget << 1));
            break;
        }
        case UniqueOpCode::JUMPZ:
        {
            if (m_Z)
                pcNext = jumpTarget;

            MAKE_DBG(std::hex << std::setw(6) << std::setfill('0') << (jumpTarget << 1));
            break;
        }
        case UniqueOpCode::JUMPC:
        {
            if (m_C)
                pcNext = jumpTarget;

            MAKE_DBG(std::hex << std::setw(6) << std::setfill('0') << (jumpTarget << 1));
            break;
        }
        case UniqueOpCode::JUMPNZ:
        {
            if (! m_Z)
                pcNext = jumpTarget;

            MAKE_DBG(std::hex << std::setw(6) << std::setfill('0') << (jumpTarget << 1));
            break;
        }
        case UniqueOpCode::JUMPNC:
        {
            if (! m_C)
                pcNext = jumpTarget;

            MAKE_DBG(std::hex << std::setw(6) << std::setfill('0') << (jumpTarget << 1));
            break;
        }
        case UniqueOpCode::CALL:
        {
            pcNext = jumpTarget;
            m_sprRegisters[SPR_LR] = (m_pc + 1);

            MAKE_DBG(std::hex << std::setw(6) << std::setfill('0') << (jumpTarget << 1));
            break;
        }
        case UniqueOpCode::CALLZ:
        {
            if (m_Z)
            {
                pcNext = jumpTarget;
                m_sprRegisters[SPR_LR] = (m_pc + 1);
            }
            MAKE_DBG(std::hex << std::setw(6) << std::setfill('0') << (jumpTarget << 1));
            break;
        }
        case UniqueOpCode::CALLC:
        {
            if (m_C)
            {
                pcNext = jumpTarget;
                m_sprRegisters[SPR_LR] = (m_pc + 1);
            }
            MAKE_DBG(std::hex << std::setw(6) << std::setfill('0') << (jumpTarget << 1));
            break;
        }
        case UniqueOpCode::CALLNZ:
        {
            if (! m_Z)
            {
                pcNext = jumpTarget;
                m_sprRegisters[SPR_LR] = (m_pc + 1);
            }
            MAKE_DBG(std::hex << std::setw(6) << std::setfill('0') << (jumpTarget << 1));
            break;
        }
        case UniqueOpCode::CALLNC:
        {
            if (! m_C)
            {
                pcNext = jumpTarget;
                m_sprRegisters[SPR_LR] = (m_pc + 1);
            }
            MAKE_DBG(std::hex << std::setw(6) << std::setfill('0') << (jumpTarget << 1));
            break;
        }
        case UniqueOpCode::JUMP_REL:
        {
            pcNext = jumpTargetRel;
            MAKE_DBG(std::hex << std::setw(6) << std::setfill('0') << (jumpTargetRel << 1));
            break;
        }
        case UniqueOpCode::JUMPZ_REL:
        {
            if (m_Z)
                pcNext = jumpTargetRel;
            MAKE_DBG(std::hex << std::setw(6) << std::setfill('0') << (jumpTargetRel << 1));
            break;
        }
        case UniqueOpCode::JUMPC_REL:
        {
            if (m_C)
                pcNext = jumpTargetRel;
            MAKE_DBG(std::hex << std::setw(6) << std::setfill('0') << (jumpTargetRel << 1));
            break;
        }
        case UniqueOpCode::JUMPNZ_REL:
        {
            if (! m_Z)
                pcNext = jumpTargetRel;
            MAKE_DBG(std::hex << std::setw(6) << std::setfill('0') << (jumpTargetRel << 1));
            break;
        }
        case UniqueOpCode::JUMPNC_REL:
        {
            if (! m_C)
                pcNext = jumpTargetRel;
            MAKE_DBG(std::hex << std::setw(6) << std::setfill('0') << (jumpTargetRel << 1));
            break;
        }
        case UniqueOpCode::CALL_REL:
        {
            pcNext = jumpTargetRel;
            m_sprRegisters[SPR_LR] = (m_pc + 1);
            MAKE_DBG(std::hex << std::setw(6) << std::setfill('0') << (jumpTargetRel << 1));
            break;
        }
        case UniqueOpCode::CALLZ_REL:
        {
            if (m_Z)
            {
                pcNext = jumpTargetRel;
                m_sprRegisters[SPR_LR] = (m_pc + 1);
            }
            MAKE_DBG(std::hex << std::setw(6) << std::setfill('0') << (jumpTargetRel << 1));
            break;
        }
        case UniqueOpCode::CALLC_REL:
        {
            if (m_C)
            {
                pcNext = jumpTargetRel;
                m_sprRegisters[SPR_LR] = (m_pc + 1);
            }
            MAKE_DBG(std::hex << std::setw(6) << std::setfill('0') << (jumpTargetRel << 1));
            break;
        }
        case UniqueOpCode::CALLNZ_REL:
        {
            if (! m_Z)
            {
                pcNext = jumpTargetRel;
                m_sprRegisters[SPR_LR] = (m_pc + 1);
            }
            MAKE_DBG(std::hex << std::setw(6) << std::setfill('0') << (jumpTargetRel << 1));
            break;
        }
        case UniqueOpCode::CALLNC_REL:
        {
            if (! m_C)
            {
                pcNext = jumpTargetRel;
                m_sprRegisters[SPR_LR] = (m_pc + 1);
            }
            MAKE_DBG(std::hex << std::setw(6) << std::setfill('0') << (jumpTargetRel << 1));
            break;
        }
        case UniqueOpCode::SVC:
            break;
        case UniqueOpCode::RET:
        {
            pcNext = m_sprRegisters[SPR_LR];
            break;
        }
        case UniqueOpCode::RETI:
        {
            m_sprRegisters[SPR_MSR] |= MSR_IE; // restore interrupts
            pcNext = m_sprRegisters[SPR_ILR];
            break;
        }
        case UniqueOpCode::RETE:
            break;
        case UniqueOpCode::LDW_REG:
        {
            int regx = (m_instruction & 0xf0) >> 4;
            int regy = (m_instruction & 0x0f);
            int regi = (m_instruction & 0x300) >> 8;

            std::uint32_t memOffset = m_gprRegisters[regy] + m_sprRegisters[regi + 8];
            std::uint16_t word      = m_memory.readWord(memOffset >> 1);

            m_gprRegisters[regx] = word;

            MAKE_DBG((Register)regx << ",[" << (Register)(regi + 16 + 8) << "," << (Register)regy << "]");

            break;
        }
        case UniqueOpCode::LDW_IMM:
        {
            int regx = (m_instruction & 0xf0) >> 4;
            int immVal = (m_imm << 4) | (m_instruction & 0x0f);
            int regi = (m_instruction & 0x300) >> 8;

            std::uint32_t memOffset = m_sprRegisters[regi + 8] + immVal;
            std::uint16_t word      = m_memory.readWord(memOffset >> 1);

            m_gprRegisters[regx] = word;

            MAKE_DBG((Register)regx << ",[" << (Register)(regi + 16 + 8) << "," << immVal << "]");

            break;
        }
        case UniqueOpCode::STW_REG:
        {
            int regx = (m_instruction & 0xf0) >> 4;
            int regy = (m_instruction & 0x0f);
            int regi = (m_instruction & 0x300) >> 8;

            std::uint32_t memOffset = m_gprRegisters[regy] + m_sprRegisters[regi + 8];
            m_memory.writeWord(memOffset >> 1, m_gprRegisters[regx]);

            MAKE_DBG((Register)regx << ",[" << (Register)(regi + 16 + 8) << "," << (Register)regy << "]");

            break;
        }
        case UniqueOpCode::STW_IMM:
        {
            int regx = (m_instruction & 0xf0) >> 4;
            int immVal = (m_imm << 4) | (m_instruction & 0x0f);
            int regi = (m_instruction & 0x300) >> 8;

            std::uint32_t memOffset = m_sprRegisters[regi + 8] + immVal;
            m_memory.writeWord(memOffset >> 1, m_gprRegisters[regx]);

            MAKE_DBG((Register)regx << ",[" << (Register)(regi + 16 + 8) << "," << immVal << "]");

            break;
        }
        case UniqueOpCode::LDSPR:
        {
            int regx = (m_instruction & 0xf0) >> 4;
            int regy = (m_instruction & 0x0e);

            std::uint16_t wordlo = m_gprRegisters[regy];
            std::uint16_t wordhi = m_gprRegisters[regy+1];

            std::uint32_t dword = (wordhi << 16) | wordlo;

            m_sprRegisters[regx] = dword;

            MAKE_DBG((Register)(regx + 16) << "," << (Register)regy);

            break;

        }
        case UniqueOpCode::STSPR:
        {

            int regx = (m_instruction & 0xe0) >> 4;
            int regy = (m_instruction & 0x0f);

            std::uint32_t dword = m_sprRegisters[regy];

            m_gprRegisters[regx] = dword & 0xffff;
            m_gprRegisters[regx + 1] = dword & 0xffff0000;

            MAKE_DBG((Register)regx << "," << (Register)(regy + 16));


            break;
        }
        case UniqueOpCode::OUT:
        {
            int regx = (m_instruction & 0xf0) >> 4;
            int regy = (m_instruction & 0x0f);

            m_ioports.outPort(m_gprRegisters[regy], m_gprRegisters[regx]);

            MAKE_DBG((Register)regx << "," << (Register)regy);

            break;
        }
        case UniqueOpCode::IN:
        {
            int regx = (m_instruction & 0xf0) >> 4;
            int regy = (m_instruction & 0x0f);

            m_gprRegisters[regx] = m_ioports.inPort(m_gprRegisters[regy]);

            MAKE_DBG((Register)regx << "," << (Register)regy);

            break;
        }
        case UniqueOpCode::INCW:
        {
            int regx = (m_instruction & 0xf0) >> 4;

            m_sprRegisters[regx] += 2;

            MAKE_DBG((Register)(regx + 16));

            break;
        }
        case UniqueOpCode::DECW:
        {
            int regx = (m_instruction & 0xf0) >> 4;

            m_sprRegisters[regx] -= 2;

            MAKE_DBG((Register)(regx + 16));

            break;
        }
        default:
            break;
    }

    #ifdef DEBUG_INSTRUCTIONS
        std::cout << instructionText.str() << std::endl;

        for (int i = 0; i < 16; i ++)
            std::cout << std::hex << std::setw(4) << std::setfill('0') << m_gprRegisters[i] << " ";

        std::cout << std::endl;

        for (int i = 0; i < 16; i ++)
            std::cout << std::hex << std::setw(8) << std::setfill('0') << m_sprRegisters[i] << " ";

        std::cout << std::endl;

    #endif

    m_pc = pcNext;
    m_imm = immNext;

}
