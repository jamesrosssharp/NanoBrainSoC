#include "ioports.h"

#include "ledswitch.h"
#include "uart.h"

#define MAKE_PORT_NUM(x) ((x & 0xf000) >> 12)
#define MAKE_PORT_REG(x) (x & 0xfff)

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
        case kPortTimer:
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
        case kPortTimer:
        default:
            break;
    }
}


