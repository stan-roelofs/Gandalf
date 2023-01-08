#include "mbc1.h"

#include <cassert>

#include <gandalf/util.h>

namespace gandalf {
    MBC1::MBC1(const ROM& rom, std::size_t rom_banks, std::size_t ram_banks, bool has_battery) : MBC(rom, rom_banks, ram_banks),
        ram_enabled_(false), rom_bank_number_(1), ram_bank_number_(0), advanced_banking_mode_(false), has_battery_(has_battery)
    {
        assert(rom_banks % 2 == 0 && rom_banks <= 128);
        assert(ram_banks == 0 || ram_banks == 1 || ram_banks == 4);

        (void)has_battery_;
    }

    MBC1::~MBC1() = default;

    byte MBC1::Read(word address) const {
        assert(address <= 0x8000 || BETWEEN(address, 0xA000, 0xC000));

        if (address < 0x4000) {
            word bank = 0;
            if (advanced_banking_mode_) {
                bank = (ram_bank_number_ << 5) % static_cast<byte>(rom_.size());
            }
            return rom_[bank][address];
        }
        else if (address < 0x8000) {
            byte bank = (rom_bank_number_ | (ram_bank_number_ << 5)) % static_cast<byte>(rom_.size());
            return rom_[bank][address - 0x4000];
        }
        else if (BETWEEN(address, 0xA000, 0xC000)) {
            if (ram_.size() == 0 || !ram_enabled_)
                return 0xFF;

            word bank_number = 0;
            if (advanced_banking_mode_ && ram_bank_number_ < ram_.size())
                bank_number = ram_bank_number_;

            return ram_[bank_number][address - 0xA000];
        }

        return 0xFF;
    }

    void MBC1::Write(word address, byte value) {
        assert(address <= 0x8000 || BETWEEN(address, 0xA000, 0xC000));

        if (address < 0x2000)
            ram_enabled_ = (value & 0x0F) == 0x0A;
        else if (address < 0x4000) {
            rom_bank_number_ = (value & 0x1F);
            /* MBC1 doesn’t allow the BANK1 register to contain zero (bit pattern 0b00000), so the initial value at reset
             * is 0b00001 and attempting to write 0b00000 will write 0b00001 instead. */
            if (rom_bank_number_ == 0)
                rom_bank_number_ = 1;
        }
        else if (address < 0x6000)
            ram_bank_number_ = (value & 0x3);
        else if (address < 0x8000)
            advanced_banking_mode_ = (value & 0x1);
        else if (BETWEEN(address, 0xA000, 0xC000)) {
            if (ram_.empty() || !ram_enabled_)
                return;

            word bank_number = 0;
            if (advanced_banking_mode_ && ram_bank_number_ < ram_.size())
                bank_number = ram_bank_number_;

            ram_[bank_number][address - 0xA000] = value;
        }
    }
}