#pragma once

#include "iinterruptdelegate.h"
#include "iportsink.h"

enum class IntConPort
{
    Control = 0,
    InterruptEnable = 1,
    InterruptStatus = 2,
};

class IntCon    : public IInterruptDelegate, public IPortSink
{
public:
    virtual ~IntCon() {}

    virtual std::uint16_t inPort(std::uint16_t reg) override;
    virtual void          outPort(std::uint16_t reg, std::uint16_t value) override;

    virtual void setIRQ(int IRQ, bool level) override;

    void setCPUInterruptDelegate(ICPUInterruptDelegate* cpu) { m_cpuDel = cpu; }

private:

    const int kControlInterruptEnable = 0;

    void updateInterrupts();

    std::uint16_t m_control;
    std::uint16_t m_interruptEnable;
    std::uint16_t m_interruptStatus;

    ICPUInterruptDelegate* m_cpuDel;

};


