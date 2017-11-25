#pragma once

#include "cpu.h"
#include "memory.h"
#include "ioports.h"

#include <functional>

class nbSoC
{

public:

    nbSoC();

    void configureBlockRam(std::string blockRamImg);
    void configureFlash(std::string flashImg);
    void configureSDCard(std::string configureSDCard);

    void onBlitToGfxRam(std::function<void ()> func);
    void onUARTTxChar(std::function<void (char c)> func);
    void onLedGreenWrite(std::function<void (uint16_t)> func );

    void start();

private:

    std::function<void (char c)> m_onUARTTxChar;

    Memory m_memory;
    IOPorts m_ioports;

    CPU m_cpu;

};
