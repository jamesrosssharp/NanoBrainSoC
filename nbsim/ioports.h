#pragma once

#include <cstdint>
#include <functional>

#include "ledswitch.h"
#include "uart.h"
#include "timercounter.h"
#include "intcon.h"

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

    IOPorts();

    std::uint16_t inPort(std::uint16_t port);
    void          outPort(std::uint16_t port, std::uint16_t value);

    void onLedGreenWrite(std::function<void (uint16_t)> func);
    void onLedRedWrite  (std::function<void (uint16_t)> func);
    void onHexWrite     (std::function<void (int, uint16_t)> func);

    void hardReset()
    {
        m_ledSwitch.hardReset();
    }

    void shutDown()
    {
        m_timerCounter.shutDown();
    }

    void setCPUInterruptDelegate(ICPUInterruptDelegate* cpu) { m_intCon.setCPUInterruptDelegate(cpu); }

private:

    UART m_uart;
    LedSwitch m_ledSwitch;
    TimerCounter m_timerCounter;
    IntCon m_intCon;
};
