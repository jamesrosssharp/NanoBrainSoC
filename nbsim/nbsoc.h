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
    void onLedGreenWrite(std::function<void (uint16_t)> func );
    void onLedRedWrite(std::function<void (uint16_t)> func );
    void onHexWrite(std::function<void (int, uint16_t)> func );

    void onResetButtonPressed(bool pressed);

    void start();
    void shutDown();

    CPU* getCPU() { return &m_cpu; }

private:


    Memory m_memory;
    IOPorts m_ioports;

    CPU m_cpu;

};
