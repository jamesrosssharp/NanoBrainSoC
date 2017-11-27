#include "ledswitch.h"

std::uint16_t LedSwitch::inPort(std::uint16_t reg)
{
    switch ((LedSwitchPort)reg)
    {
        case LedSwitchPort::SlideSwitches:
            return m_slideSwitches;
        case LedSwitchPort::GreenLeds:
            return m_greenLeds;
        case LedSwitchPort::RedLeds:
            return m_redLeds;
        case LedSwitchPort::Hex0:
            return m_hex0;
        case LedSwitchPort::Hex1:
            return m_hex1;
        case LedSwitchPort::Hex2:
            return m_hex2;
        case LedSwitchPort::Hex3:
            return m_hex3;
        default:
            return 0;
    }

}

void LedSwitch::hardReset()
{
    m_greenLeds = 0;
    m_onLedGreenWrite(0);

    m_redLeds = 0;
    m_onLedRedWrite(0);

    m_hex0 = 0;
    m_onHexWrite(0, 0);

    m_hex1 = 0;
    m_onHexWrite(1, 0);

    m_hex2 = 0;
    m_onHexWrite(2, 0);

    m_hex3 = 0;
    m_onHexWrite(3, 0);

}

void          LedSwitch::outPort(std::uint16_t reg, std::uint16_t value)
{
    switch ((LedSwitchPort)reg)
    {
        case LedSwitchPort::SlideSwitches:
            break;
        case LedSwitchPort::GreenLeds:
            m_greenLeds = value;
            m_onLedGreenWrite(value);
            break;
        case LedSwitchPort::RedLeds:
            m_redLeds = value;
            m_onLedRedWrite(value);
            break;
        case LedSwitchPort::Hex0:
            m_hex0 = value;
            m_onHexWrite(0, value);
            break;
        case LedSwitchPort::Hex1:
            m_hex1 = value;
            m_onHexWrite(1, value);
            break;
        case LedSwitchPort::Hex2:
            m_hex2 = value;
            m_onHexWrite(2, value);
            break;
        case LedSwitchPort::Hex3:
            m_hex3 = value;
            m_onHexWrite(3, value);
            break;
        default:
            break;
    }

}

void LedSwitch::onLedGreenWrite(std::function<void (uint16_t)> func )
{
    m_onLedGreenWrite = func;
}

void LedSwitch::onLedRedWrite(std::function<void (uint16_t)> func )
{
    m_onLedRedWrite = func;
}

void LedSwitch::onHexWrite(std::function<void (int, uint16_t)> func )
{
    m_onHexWrite = func;
}
