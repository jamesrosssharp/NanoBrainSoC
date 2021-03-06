#include "ioports.h"

#include "ledswitch.h"
#include "uart.h"

#define MAKE_PORT_NUM(x) ((x & 0xf000) >> 12)
#define MAKE_PORT_REG(x) (x & 0xfff)

IOPorts::IOPorts()
{
    m_timerCounter.setInterruptDelegate(&m_intCon);
}

std::uint16_t IOPorts::inPort(std::uint16_t port)
{

    switch (MAKE_PORT_NUM(port))
    {
        case kPortUART:
            return m_uart.inPort(MAKE_PORT_REG(port));
        case kPortLEDSwitch:
            return m_ledSwitch.inPort(MAKE_PORT_REG(port));
        case kPortVGACon:
        case kPortAudioCon:
        case kPortBlitCon:
        case kPortTexCon0:
        case kPortTexCon1:
        case kPortTexCon2:
        case kPortSDCon:
        case kPortFlashCon:
        case kPortKbdCon:
        case kPortIntCon:
            return m_intCon.inPort(MAKE_PORT_REG(port));
        case kPortTimer:
            return m_timerCounter.inPort(MAKE_PORT_REG(port));
        default:
            return 0;
    }

}

void          IOPorts::outPort(std::uint16_t port, std::uint16_t value)
{
    switch (MAKE_PORT_NUM(port))
    {
        case kPortUART:
            m_uart.outPort(MAKE_PORT_REG(port), value);
            break;
        case kPortLEDSwitch:
            m_ledSwitch.outPort(MAKE_PORT_REG(port), value);
            break;
        case kPortVGACon:
        case kPortAudioCon:
        case kPortBlitCon:
        case kPortTexCon0:
        case kPortTexCon1:
        case kPortTexCon2:
        case kPortSDCon:
        case kPortFlashCon:
        case kPortKbdCon:
        case kPortIntCon:
            m_intCon.outPort(MAKE_PORT_REG(port), value);
            break;
        case kPortTimer:
            m_timerCounter.outPort(MAKE_PORT_REG(port), value);
            break;
        default:
            break;
    }
}

void IOPorts::onLedGreenWrite(std::function<void (uint16_t)> func )
{
    m_ledSwitch.onLedGreenWrite(func);
}

void IOPorts::onLedRedWrite(std::function<void (uint16_t)> func )
{
    m_ledSwitch.onLedRedWrite(func);
}

void IOPorts::onHexWrite(std::function<void (int, uint16_t)> func )
{
    m_ledSwitch.onHexWrite(func);
}

