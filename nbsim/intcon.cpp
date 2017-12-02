#include "intcon.h"

#include <iostream>

std::uint16_t IntCon::inPort(std::uint16_t reg)
{
    switch ((IntConPort)reg)
    {
        case IntConPort::Control:
            return m_control;
            break;
        case IntConPort::InterruptStatus:
            return m_interruptStatus;
            break;
        case IntConPort::InterruptEnable:
            return m_interruptEnable;
            break;
    }
}

void          IntCon::outPort(std::uint16_t reg, std::uint16_t value)
{
    switch ((IntConPort)reg)
    {
        case IntConPort::Control:
            m_control = value;
            break;
        case IntConPort::InterruptStatus:
            m_interruptStatus &= ~value;    // write to clear
            updateInterrupts();
            break;
        case IntConPort::InterruptEnable:
            m_interruptEnable = value;
            break;
    }

}

void IntCon::setIRQ(int IRQ, bool level)
{

    if (IRQ < 0 || IRQ > 15)
    {
        std::cout << "Invalid IRQ " << IRQ << std::endl;
        return;
    }

    if (level)
        m_interruptStatus |= 1 << IRQ;
    else
        m_interruptStatus &= ~(1 << IRQ);

    updateInterrupts();

}

void IntCon::updateInterrupts()
{
    if (m_interruptEnable & m_interruptStatus)
        m_cpuDel->setIRQ(true);
    else
        m_cpuDel->setIRQ(false);
}
