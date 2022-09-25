#ifndef __GANDALF_WRAM_H
#define __GANDALF_WRAM_H

#include "bus.h"
#include "constants.h"

namespace gandalf {
    class WRAM : public Bus::AddressHandler {
    public:
        WRAM(GameboyMode mode);
        virtual ~WRAM();

        byte Read(word address) const override;
        void Write(word address, byte value) override;
        std::set<word> GetAddresses() const override;

    private:
        std::array<std::array<byte, 0x1000>, 8> data_;
        int wram_bank_;
        GameboyMode mode_;
    };
} // namespace gandalf

#endif