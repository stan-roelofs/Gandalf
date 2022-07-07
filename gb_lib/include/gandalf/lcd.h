#ifndef __GANDALF_LCD_H
#define __GANDALF_LCD_H

#include "bus.h"

namespace gandalf {
    class LCD : public Bus::AddressHandler {
    public:
        LCD();
        virtual ~LCD();

        void Tick();

        byte Read(word address) const override;
        void Write(word address, byte value) override;
        std::set<word> GetAddresses() const override;

        byte& GetLCDStatus() { return stat_; }
        byte& GetLY() { return ly_; }
        byte& GetLYC() { return lyc_; }

    private:
        byte lcdc_;
        byte ly_;
        byte lyc_;
        byte stat_;
        byte scy_;
        byte scx_;
        byte wy_;
        byte wx_;
        byte bgp_;
        byte obp0_;
        byte obp1_;
        byte dma_;
    };
} // namespace gandalf

#endif