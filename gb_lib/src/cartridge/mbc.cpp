#include <gandalf/cartridge.h>

#include <cassert>

#include <gandalf/util.h>

namespace gandalf {
    MBC::MBC(const ROM& rom, std::size_t rom_banks, std::size_t ram_banks) {
        rom_.resize(rom_banks);
        ram_.resize(ram_banks);

        for (size_t bank = 0; bank < rom_banks; ++bank)
            std::copy(rom.begin() + bank * 0x4000, rom.begin() + (bank + 1) * 0x4000, rom_[bank].begin());

        for (size_t bank = 0; bank < ram_banks; ++bank)
            ram_[bank].fill(0);
    }
    MBC::~MBC() = default;
}