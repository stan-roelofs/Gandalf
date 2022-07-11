#include "rom_only.h"

#include <cassert>

#include <gandalf/util.h>

namespace gandalf {
    ROMOnly::ROMOnly(const ROM& rom, std::size_t ram_banks) : MBC(rom, 2, ram_banks) {
        assert(ram_banks <= 1);
    }

    ROMOnly::~ROMOnly() = default;

    byte ROMOnly::Read(word address) const {
        assert(address <= 0x8000 || BETWEEN(address, 0xA000, 0xC000));
        if (address <= 0x8000)
            return rom_[address / kROMBankSize][address % kROMBankSize];
        else if (BETWEEN(address, 0xA000, 0xC000)) {
            if (ram_.size() > 0)
                return ram_[0][address % 0xA000];
            return 0xFF;
        }

        return 0xFF;
    }

    void ROMOnly::Write(word address, byte value) {
        assert(address <= 0x8000 || BETWEEN(address, 0xA000, 0xC000));

        if (address <= 0x8000)
            return;
        else if (BETWEEN(address, 0xA000, 0xC000)) {
            if (ram_.size() > 0)
                ram_[0][address % 0xA000] = value;
        }
    }

} // namespace gandalf