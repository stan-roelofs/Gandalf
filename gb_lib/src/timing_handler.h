#ifndef __TIMING_HANDLER_H
#define __TIMING_HANDLER_H

namespace gandalf {

class TimingHandler
{
public:
    virtual void Advance(std::uint8_t cycles) = 0;
};

} // namespace gandalf

#endif
