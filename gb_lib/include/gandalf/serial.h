#ifndef __GANDALF_SERIAL_H
#define __GANDALF_SERIAL_H

#include "bus.h"
#include "constants.h"

namespace gandalf {
    class Serial : public Bus::AddressHandler {
    public:
        Serial(GameboyMode mode);
        virtual ~Serial();

        void Tick();

        byte Read(word address) const override;
        void Write(word address, byte value) override;
        std::set<word> GetAddresses() const override;

        void SetMode(GameboyMode mode) { mode_ = mode; }

    private:
        byte sb_;

        bool in_progress_;
        bool fast_clock_speed_;
        bool internal_clock_;
        GameboyMode mode_;
    };
} // namespace gandalf

#endif