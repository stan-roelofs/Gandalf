#ifndef __GANDALF_CARTRIDGE_MBC3_H
#define __GANDALF_CARTRIDGE_MBC3_H

#include <gandalf/types.h>
#include <gandalf/mbc.h>

namespace gandalf {
    class MBC3 : public MBC {
    public:
        MBC3(const ROM& rom, std::size_t rom_banks, std::size_t ram_banks, bool has_battery, bool has_timer);
        virtual ~MBC3();

        byte Read(word address) const override;
        void Write(word address, byte value) override;

    private:
        bool ram_enabled_;
        byte rom_bank_number_;
        byte ram_bank_number_;
        bool advanced_banking_mode_;
        bool has_battery_; // TODO: implement battery
        bool has_timer_;
    };
}

#endif