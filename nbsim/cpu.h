#pragma once

#include "memory.h"
#include "ioports.h"

#include <cstdint>
#include <thread>
#include <condition_variable>
#include <mutex>

class CPU
{
public:

    CPU(Memory& mem, IOPorts& ioports);

    void hardReset();
    void run();

    void runThread();

private:

    void clockTick();
    void fetchInstruction();
    void executeInstruction();

    const std::uint32_t SPR_MSR = 0;
    const std::uint32_t MSR_IE  = 0;
    const std::uint32_t MSR_EE  = 1;

    const std::uint32_t SPR_LR = 1;

    std::uint32_t m_pc;
    std::uint16_t m_imm;
    bool m_C;
    bool m_Z;

    std::uint16_t m_gprRegisters[16];
    std::uint32_t m_sprRegisters[16];

    bool m_interrupt;
    bool m_exception;
    bool m_svc;

    std::uint16_t m_instruction;

    Memory& m_memory;
    IOPorts& m_ioports;

    std::mutex m_mutex;
    std::condition_variable m_cond;

    std::thread m_cpuThread;

};
