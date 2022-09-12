#include "mbc3.h"

#include <cassert>

#include <gandalf/util.h>

namespace gandalf {
    MBC3::MBC3(const ROM& rom, std::size_t rom_banks, std::size_t ram_banks, bool has_battery, bool has_timer) : MBC(rom, rom_banks, ram_banks),
        ram_enabled_(false),
        rom_bank_number_(0),
        ram_bank_number_(0),
        has_battery_(has_battery),
        has_timer_(has_timer)
    {
        assert(rom_banks % 2 == 0 && rom_banks <= 128);
        assert(ram_banks == 0 || ram_banks == 1 || ram_banks == 4);

        (void)has_battery_;
        (void)has_timer_;
    }

    MBC3::~MBC3() = default;

    byte MBC3::Read(word address) const {
        assert(address <= 0x8000 || BETWEEN(address, 0xA000, 0xC000));

        if (address < 0x4000) {
            return rom_[0][address];
        }
        else if (address < 0x8000) {
            return rom_[rom_bank_number_][address - 0x4000];
        }
        else if (BETWEEN(address, 0xA000, 0xC000)) {
            if (ram_.size() == 0 || !ram_enabled_)
                return 0xFF;

            return ram_[ram_bank_number_][address - 0xA000];
        }

        return 0xFF;
    }

    void MBC3::Write(word address, byte value) {
        assert(address <= 0x8000 || BETWEEN(address, 0xA000, 0xC000));

        if (address < 0x2000)
            ram_enabled_ = (value & 0x0F) == 0x0A;
        else if (address < 0x4000) {
            rom_bank_number_ = (value & 0x7F) % rom_.size();
            if (rom_bank_number_ == 0)
                rom_bank_number_ = 1;
        }
        else if (address < 0x6000)
            ram_bank_number_ = (value & 0x3);
        else if (address < 0x8000)
            (void)value; // TODO latch
        else if (BETWEEN(address, 0xA000, 0xC000)) {
            if (ram_.size() == 0 || !ram_enabled_)
                return;

            ram_[ram_bank_number_][address - 0xA000] = value;
        }
    }
}