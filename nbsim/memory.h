#pragma once

#include <cstdint>
#include <string>

class Memory
{
public:
    Memory();
    ~Memory();

    std::uint16_t readWord(std::uint32_t address); // word address
    void          writeWord(std::uint32_t address, std::uint16_t word); // word address

    void configureBlockRam(std::string bramFile);
    void configureFlash(std::string flashFile);

private:

    const std::uint32_t kDDRSizeInWords   = 4*1024*1024; // 8MiB
    const std::uint32_t kFlashSizeInWords = 2*1024*1024; // 4 MiB
    const std::uint32_t kBRAMSizeInWords  = 512; // 1024k

    const std::uint32_t kBRAMAddressMask = 0x1ff; // word mask (512), used to and with address to wrap BRAM
                                                  // address space

    // all addresses in words
    const std::uint32_t kBRAMStartAddress  = 0x000000;
    const std::uint32_t kFlashStartAddress = 0x200000;
    const std::uint32_t kDDRStartAddress   = 0x400000;

    const std::uint32_t kBRAMEndAddress  = 0x1FFFFF;
    const std::uint32_t kFlashEndAddress = 0x3FFFFF;
    const std::uint32_t kDDREndAddress   = 0x7FFFFF;

    std::uint16_t* m_ddr;
    std::uint16_t* m_flash;
    std::uint16_t* m_bram;
};

