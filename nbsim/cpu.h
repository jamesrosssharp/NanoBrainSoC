#pragma once

#include "memory.h"
#include "ioports.h"
#include "iinterruptdelegate.h"

#include <cstdint>
#include <thread>
#include <condition_variable>
#include <mutex>

class CPU   : public ICPUInterruptDelegate
{
public:

    CPU(Memory& mem, IOPorts& ioports);
    ~CPU() {}

    void hardReset();
    void run();
    void pause();
    void singleStep();
    void shutDown();

    void runThread();

    void holdInReset(bool hold);

    virtual void setIRQ(bool level) override;

    std::string dumpRegisters();
    std::string dumpDisas();

private:

    void clockTick();
    void fetchInstruction();
    void executeInstruction();

    const std::uint32_t SPR_MSR = 0;
    const std::uint32_t MSR_IE  = 1 << 0;
    const std::uint32_t MSR_EE  = 1 << 1;

    const std::uint32_t SPR_LR = 1;

    const std::uint32_t SPR_ILR = 2;

    const std::uint32_t SPR_VBAR = 5;

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

    std::condition_variable m_pauseCond;
    std::condition_variable m_singleStepCond;

    std::thread m_cpuThread;

    bool m_threadExit;
    volatile bool m_threadPause;
    bool m_singleStep;

    bool m_paused;

    bool m_sleep;
};
