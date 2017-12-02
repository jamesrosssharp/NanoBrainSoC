#pragma once

#include "iportsink.h"

#include <cstdint>
#include <thread>

enum class TimerReg
{
    Control = 0,
    Status = 1,
    Count = 2,
    LoadCount = 3,
};

class TimerCounter : public IPortSink
{
public:

    TimerCounter();

    virtual std::uint16_t inPort(uint16_t reg) override;
    virtual void outPort(uint16_t reg, uint16_t value) override;

    void shutDown();

    const std::uint16_t kControlEnable      = 1 << 0;
    const std::uint16_t kControlLoad        = 1 << 1;
    const std::uint16_t kControlAutoReload  = 1 << 2;
    const std::uint16_t kControlInterruptEnable = 1 << 3;

    // prescaler:
    //  000b = no prescale
    //  001b = / 2
    //  010b = / 4
    //  011b = / 8
    //  100b = / 16
    //  101b = / 32
    //  110b = / 64
    //  111b = / 128
    //  ...
    //  1111b = / 32768

    const std::uint16_t kControlPrescale_shift = 4;
    const std::uint16_t kControlPrescale_mask = 0xf0;

    const std::uint16_t kStatusInterrupt = 1 << 0;

    const int kTimerIRQ = 0;

    void runThread();

    virtual void        setInterruptDelegate(IInterruptDelegate* intDel) override { m_intDel = intDel; }

private:

    void timerTick(int loops);

    std::uint16_t m_control;
    std::uint16_t m_status;
    std::uint16_t m_count;
    std::uint16_t m_loadCount;

    std::uint16_t m_prescaleCount;

    IInterruptDelegate* m_intDel = nullptr;

    bool m_threadExit;
    std::thread m_counterThread;

};


