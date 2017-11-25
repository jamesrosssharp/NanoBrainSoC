#pragma once

#include "iportsink.h"

enum class UARTReg
{
    RXFifo = 1,
    Status = 3,
    TXFifo = 0
};

class UART : public IPortSink
{
public:

    virtual std::uint16_t inPort(std::uint16_t reg) override;
    virtual void          outPort(std::uint16_t reg, std::uint16_t value) override;

};
