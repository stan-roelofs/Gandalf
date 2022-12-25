#ifndef __GANDALF_WRAM_H
#define __GANDALF_WRAM_H

#include "memory.h"
#include "constants.h"

namespace gandalf {
    class WRAM: public Memory::AddressHandler {
    public:
        WRAM(GameboyMode mode);
        virtual ~WRAM();

        byte Read(word address) const override;
        void Write(word address, byte value) override;
        std::set<word> GetAddresses() const override;

        void SetMode(GameboyMode mode) { mode_ = mode; }

    private:
        std::array<std::array<byte, 0x1000>, 8> data_;
        int wram_bank_;
        GameboyMode mode_;
    };
} // namespace gandalf

#endif