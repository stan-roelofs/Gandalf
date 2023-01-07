#ifndef __GANDALF_TIMER_H
#define __GANDALF_TIMER_H

#include "memory.h"

namespace gandalf {
    class Timer: public Memory::AddressHandler
    {
    public:
        Timer(Memory& memory);
        virtual ~Timer();

        void Write(word address, byte value) override;
        byte Read(word address) const override;
        std::set<word> GetAddresses() const override;

        void Tick();

        word GetInternalCounter() const { return div_; }
        word GetDIV() const { return div_ >> 8; }
        word GetTMA() const { return tma_; }
        word GetTIMA() const { return tima_; }
        word GetTAC() const { return tac_; }

    private:
        void OnDIVChanged(word old_div);
        word div_;
        byte tma_;
        byte tima_;
        byte tac_;
        Memory& memory_;
        byte reload_counter_;
        byte selected_bit_;

        bool enabled_;
    };
}

#endif