#ifndef __GANDALF_SERIAL_H
#define __GANDALF_SERIAL_H

#include "bus.h"

namespace gandalf {
    class Serial : public Bus::AddressHandler {
    public:
        Serial();
        virtual ~Serial();

        void Tick();

        byte Read(word address) const override;
        void Write(word address, byte value) override;
        std::set<word> GetAddresses() const override;

        byte sb_;
        byte sc_;
    };
} // namespace gandalf

#endif