#ifndef __TIMING_HANDLER_H
#define __TIMING_HANDLER_H

class TimingHandler
{
public:
    virtual void Advance(uint8_t cycles) = 0;
};

#endif
