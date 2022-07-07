#ifndef __GANDALF_HRAM_H
#define __GANDALF_HRAM_H

#include "bus.h"

namespace gandalf {
    class HRAM : public Bus::AddressHandler {
    public:
        HRAM();
        virtual ~HRAM();

        byte Read(word address) const override;
        void Write(word address, byte value) override;
        std::set<word> GetAddresses() const override;

    private:
        std::array<byte, 0x7F> data_;
    };
} // namespace gandalf

#endif