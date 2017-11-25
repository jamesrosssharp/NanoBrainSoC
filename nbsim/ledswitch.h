#pragma once

#include "iportsink.h"

class LedSwitch : public IPortSink
{
public:
    virtual std::uint16_t inPort(std::uint16_t reg) override;
    virtual void          outPort(std::uint16_t reg, std::uint16_t value) override;

};


