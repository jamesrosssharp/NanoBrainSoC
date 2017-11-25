#pragma once

#include <cstdint>

#include "ledswitch.h"
#include "uart.h"

class IOPorts
{

public:

    enum
    {
        kPortUART       = 0,
        kPortLEDSwitch  = 1,
        kPortVGACon     = 2,
        kPortAudioCon   = 3,
        kPortBlitCon    = 4,
        kPortTexCon0    = 5,
        kPortTexCon1    = 6,
        kPortTexCon2    = 7,
        kPortSDCon      = 8,
        kPortFlashCon   = 9,
        kPortKbdCon     = 10,
        kPortIntCon     = 11,
        kPortTimer      = 12
    };

    std::uint16_t inPort(std::uint16_t port);
    void          outPort(std::uint16_t port, std::uint16_t value);

private:

    UART m_uart;
    LedSwitch m_ledSwitch;
};
