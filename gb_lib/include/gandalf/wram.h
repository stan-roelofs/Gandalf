#ifndef __GANDALF_WRAM_H
#define __GANDALF_WRAM_H

#include "bus.h"

namespace gandalf {
    class WRAM : public Bus::AddressHandler {
    public:
        WRAM();
        virtual ~WRAM();

        byte Read(word address) const override;
        void Write(word address, byte value) override;
        std::set<word> GetAddresses() const override;

    private:
        std::array<byte, 0x2000> data_;
    };
} // namespace gandalf

#endif