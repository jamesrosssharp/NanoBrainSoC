#include "timercounter.h"

#include <sys/time.h>

TimerCounter::TimerCounter()    :
    m_control(0),
    m_status(0),
    m_count(0),
    m_loadCount(0),
    m_prescaleCount(0),
    m_threadExit(false),
    m_counterThread([] (TimerCounter* tc) { tc->runThread(); }, this)
{
    // Start thread to tick counter in
}

void TimerCounter::shutDown()
{
    m_threadExit = true;
    m_counterThread.join();
}

void TimerCounter::runThread()
{
    const int kPeriod = 20;

    struct timeval tv1;
    gettimeofday(&tv1, nullptr);
    // uint64_t couldn't possibly overflow, could it?
    //uint64_t nsec1 = (uint64_t)tv1.tv_sec * 1000000000 + tv1.tv_usec * 1000;

    uint64_t nsecElapsed = 0;

    while (! m_threadExit)
    {

        struct timeval tv2;
        gettimeofday(&tv2, nullptr);
        uint64_t nsec2 = (uint64_t)(tv2.tv_sec - tv1.tv_sec) * 1000000000 + (tv2.tv_usec - tv1.tv_usec) * 1000;

        int loops = 0;

        loops = (nsec2 - nsecElapsed) / kPeriod;

        timerTick(loops);

        nsecElapsed += loops * kPeriod;

        //printf("Elapsed: %llu %d\n", nsecElapsed, loops);

        struct timespec tm1, tm2;

        tm1.tv_sec = 0;
        tm1.tv_nsec = kPeriod;
        nanosleep(&tm1, &tm2);

    }
}

void TimerCounter::timerTick(int loops)
{
    int countRem = 0;

    if (m_control & kControlEnable)
    {
        int scale = 1 << ((m_control & kControlPrescale_mask) >> kControlPrescale_shift);

        int prescaleCounts = loops / scale;
        int prescaleRem = loops % scale;

        m_prescaleCount += prescaleRem;

        if (m_prescaleCount > scale)
        {
            m_prescaleCount -= scale;
            prescaleCounts++;
        }

        if (m_count > prescaleCounts)
        {
            m_count -= prescaleCounts;
        }
        else
        {
            m_count = 0;
            countRem = prescaleCounts - m_count;
            countRem %= 65536;
        }
    }

    if ((m_control & kControlEnable) && m_count == 0)
    {
        if (! (m_status & kStatusInterrupt))
        {
            m_status |= kStatusInterrupt;

            if (m_control & kControlInterruptEnable)
            {
                // signal interrupt controller
                if (m_intDel != nullptr)
                    m_intDel->setIRQ(kTimerIRQ, true);
            }
        }

        if (m_control & kControlAutoReload)
        {
            m_count = m_loadCount;
            if (m_count > countRem)
                m_count -= countRem;
            else
                m_count = (m_count + 65536 - countRem);
        }
    }
}

std::uint16_t TimerCounter::inPort(uint16_t reg)
{
    switch ((TimerReg)reg)
    {
        case TimerReg::Control:
            return m_control;
        case TimerReg::Status:
            return m_status;
        case TimerReg::Count:
            return m_count;
        case TimerReg::LoadCount:
            return m_loadCount;
        default:
            return 0;
    }
}

void TimerCounter::outPort(uint16_t reg, uint16_t value)
{
    switch ((TimerReg)reg)
    {
        case TimerReg::Control:
            m_control = value;

            if (m_control & kControlLoad)
                m_count = m_loadCount;

            break;
        case TimerReg::Status:
            if (value & kStatusInterrupt)  // write 1 to clear
            {
                printf("Clearing bit\n");
                m_status &= ~kStatusInterrupt;

                if (m_control & kControlInterruptEnable)
                {
                    // signal interrupt controller
                    if (m_intDel != nullptr)
                        m_intDel->setIRQ(kTimerIRQ, false);
                }
            }
            break;
        case TimerReg::Count:
            m_count = value;
            break;
        case TimerReg::LoadCount:
            m_loadCount = value;
            break;
        default:
            break;
    }
}
