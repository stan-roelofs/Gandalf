#ifndef __GANDALF_CARTRIDGE_MBC1_H
#define __GANDALF_CARTRIDGE_MBC1_H

#include <gandalf/types.h>
#include <gandalf/mbc.h>

namespace gandalf {
    class MBC1 : public MBC {
    public:
        MBC1(const ROM& rom, std::size_t rom_banks, std::size_t ram_banks, bool has_battery);
        virtual ~MBC1();

        byte Read(word address) const override;
        void Write(word address, byte value) override;

    private:
        bool ram_enabled_;
        word rom_bank_number_;
        word ram_bank_number_;
        bool advanced_banking_mode_;
        bool has_battery_; // TODO: implement battery
    };
}

#endif