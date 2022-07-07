#ifndef __GANDALF_PPU_H
#define __GANDALF_PPU_H

#include "bus.h"
#include "lcd.h"

namespace gandalf {
    class PPU : public Bus::AddressHandler {
    public:
        PPU(Bus& bus, LCD& lcd);
        virtual ~PPU();

        void Tick();

        byte Read(word address) const override;
        void Write(word address, byte value) override;
        std::set<word> GetAddresses() const override;

    private:
        void IncrementLY();
        Bus& bus_;
        LCD& lcd_;
        int line_ticks_;

        std::array<byte, 0x2000> vram_;
        std::array<byte, 0xA0> oam_;
    };
}

#endif