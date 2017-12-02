#pragma once

class IInterruptDelegate
{
public:

    virtual void setIRQ(int IRQ, bool level) = 0;


};

class ICPUInterruptDelegate
{
public:

    virtual void setIRQ(bool level) = 0;
};

