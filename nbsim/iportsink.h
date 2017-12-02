#pragma once

#include <cstdint>

#include "iinterruptdelegate.h"

class IPortSink
{
public:

    virtual std::uint16_t inPort(std::uint16_t reg) = 0;
    virtual void          outPort(std::uint16_t reg, std::uint16_t value) = 0;

    virtual void          setInterruptDelegate(IInterruptDelegate* intDel) { };

};

