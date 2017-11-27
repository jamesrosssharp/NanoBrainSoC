#pragma once

#include "iportsink.h"

#include <functional>

/*
--      0                       |       green LEDs
--      1                       |       red LEDs
--      2                       |       Switches
--      3                       |       Hex 0
--      4                       |       Hex 1
--      5                       |       Hex 2
--      6                       |       Hex 3
*/

enum class LedSwitchPort
{
    GreenLeds       = 0,
    RedLeds         = 1,
    SlideSwitches   = 2,
    Hex0            = 3,
    Hex1            = 4,
    Hex2            = 5,
    Hex3            = 6
};

class LedSwitch : public IPortSink
{
public:
    virtual std::uint16_t inPort(std::uint16_t reg) override;
    virtual void          outPort(std::uint16_t reg, std::uint16_t value) override;

    void onLedGreenWrite(std::function<void (uint16_t)> func );
    void onLedRedWrite(std::function<void (uint16_t)> func );
    void onHexWrite(std::function<void (int, uint16_t)> func );

    void hardReset();

private:

    std::uint16_t m_slideSwitches;
    std::uint16_t m_greenLeds;
    std::uint16_t m_redLeds;
    std::uint16_t m_hex0;
    std::uint16_t m_hex1;
    std::uint16_t m_hex2;
    std::uint16_t m_hex3;

    std::function<void (uint16_t)> m_onLedGreenWrite;
    std::function<void (uint16_t)> m_onLedRedWrite;
    std::function<void (int, uint16_t)> m_onHexWrite;
};


