#include "uart.h"

#include "stdio.h"

std::uint16_t UART::inPort(std::uint16_t reg)
{

    switch ((UARTReg)reg)
    {
        case UARTReg::RXFifo:
            return 0;
        case UARTReg::Status:
            return 0;
        case UARTReg::TXFifo:
            return 0;
    }

}

void          UART::outPort(std::uint16_t reg, std::uint16_t value)
{
    switch ((UARTReg)reg)
    {
        case UARTReg::RXFifo:
            break;
        case UARTReg::Status:
            break;
        case UARTReg::TXFifo:
            printf("%c", value & 0xff);
            //fflush(0);
            break;
    }

}


