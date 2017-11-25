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

void nbSoC::onUARTTxChar(std::function<void (char c)> func)
{
    m_onUARTTxChar = func;
}

void nbSoC::onLedGreenWrite(std::function<void (uint16_t)> func )
{
}

void nbSoC::start()
{

    m_cpu.hardReset();

    m_cpu.run();

}

