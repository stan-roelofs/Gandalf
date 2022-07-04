#ifndef __GANDALF_TIMER_H
#define __GANDALF_TIMER_H

#include "bus.h"

namespace gandalf {
    class Timer : public Bus::AddressHandler
    {
    public:
        Timer(Bus& bus);
        virtual ~Timer();

        void Write(word address, byte value) override;
        byte Read(word address) const override;
        std::set<word> GetAddresses() const override;

        void Tick();

    private:
        word div_;
        byte tma_;
        byte tima_;
        byte tac_;
        Bus& bus_;
    };
}

#endif