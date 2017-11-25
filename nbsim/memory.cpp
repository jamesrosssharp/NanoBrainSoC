#include "memory.h"

#include <fstream>
#include <iostream>
#include <iomanip>

Memory::Memory()
{
    m_ddr   = new std::uint16_t [kDDRSizeInWords];
    m_bram  = new std::uint16_t [kBRAMSizeInWords];
    m_flash = new std::uint16_t [kFlashSizeInWords];
}

Memory::~Memory()
{
    delete [] m_ddr;
    delete [] m_bram;
    delete [] m_flash;

}

void Memory::configureBlockRam(std::string bramFile)
{
    std::ifstream bramData;

    // Todo: throw if file doesn't exist?
    bramData.open(bramFile);

    std::uint16_t* data = m_bram;
    std::uintptr_t bramEnd = (std::uintptr_t)m_bram + kBRAMSizeInWords;

    while ((std::uintptr_t)data < bramEnd)
    {
        bramData.read((char*)data, 2);
        data++;
    }

}

void Memory::configureFlash(std::string flashFile)
{

}

std::uint16_t Memory::readWord(std::uint32_t address)
{

    if (address <= kBRAMEndAddress)
    {
        address &= kBRAMAddressMask;
        return m_bram[address];
    }
    else if (address <= kFlashEndAddress)
    {
        address -= kFlashStartAddress;
        return m_flash[address];
    }
    else if (address <= kDDREndAddress)
    {
        address -= kDDRStartAddress;
        return m_ddr[address];
    }
    else
    {
        throw std::runtime_error("Invalid memory access");
    }

}

void Memory::writeWord(std::uint32_t address, std::uint16_t word)
{
    if (address <= kBRAMEndAddress)
    {
        address &= kBRAMAddressMask;
        m_bram[address] = word;
    }
    else if (address <= kFlashEndAddress)
    {
        // Flash is read only from memory interface.
    }
    else if (address <= kDDREndAddress)
    {
        address -= kDDRStartAddress;
        m_ddr[address] = word;
    }
    else
    {
        throw std::runtime_error("Invalid memory access");
    }
}

