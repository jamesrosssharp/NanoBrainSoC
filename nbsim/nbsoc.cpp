#include "nbsoc.h"

nbSoC::nbSoC() :
    m_cpu(m_memory, m_ioports)
{

}

void nbSoC::configureBlockRam(std::string blockRamImg)
{
    m_memory.configureBlockRam(blockRamImg);
}

void nbSoC::configureFlash(std::string flashImg)
{
    m_memory.configureFlash(flashImg);
}

void nbSoC::configureSDCard(std::string configureSDCard)
{
}

void nbSoC::onBlitToGfxRam(std::function<void ()> func)
{
}


void nbSoC::onLedGreenWrite(std::function<void (uint16_t)> func )
{
    m_ioports.onLedGreenWrite(func);
}

void nbSoC::onLedRedWrite(std::function<void (uint16_t)> func )
{
    m_ioports.onLedRedWrite(func);
}

void nbSoC::onHexWrite(std::function<void (int, uint16_t)> func )
{
    m_ioports.onHexWrite(func);
}

void nbSoC::start()
{

    m_cpu.hardReset();

    m_cpu.run();

}

void nbSoC::shutDown()
{
    m_cpu.shutDown();
}

void nbSoC::onResetButtonPressed(bool pressed)
{
    m_cpu.holdInReset(pressed);
}

