#include <gameboy/cartridge.h>

#include <algorithm>
#include <array>
#include <stdexcept>
#include <iostream>
#include <string>

namespace gandalf {

    Cartridge::Cartridge() {}

    Cartridge::~Cartridge() = default;

    bool Cartridge::Load(const std::vector<std::uint8_t>& bytes) {
        if (bytes.size() < 0x150) // The header is located at 0x100-0x14F, so bytes must be at least 0x150 bytes long.
            return false;

        Header result;
        std::copy(bytes.begin() + 0x104, bytes.begin() + 0x134, result.logo);
        std::copy(bytes.begin() + 0x134, bytes.begin() + 0x144, result.title);
        std::copy(bytes.begin() + 0x13F, bytes.begin() + 0x143, result.manufacturer_code);
        result.cgb_flag = bytes.at(0x143);
        std::copy(bytes.begin() + 0x144, bytes.begin() + 0x146, result.new_licensee_code);
        result.sgb_flag = bytes.at(0x146);
        result.cartridge_type = bytes.at(0x147);
        result.rom_size = bytes.at(0x148);
        result.ram_size = bytes.at(0x149);
        result.destination_code = bytes.at(0x14A);
        result.old_licensee_code = bytes.at(0x14B);
        result.mask_rom_version = bytes.at(0x14C);
        result.header_checksum = bytes.at(0x14D);
        std::copy(bytes.begin() + 0x14E, bytes.begin() + 0x150, result.global_checksum);
        header_ = result;
        return true;
    }

    std::optional<Cartridge::Header> Cartridge::GetHeader() const {
        return header_;
    }
}