#include <gandalf/ppu.h>

#include <gandalf/constants.h>

namespace gandalf {
    PPU::PPU(Bus& bus) : Bus::AddressHandler("PPU"), bus_(bus)
    {
        vram_.fill(0xFF);
        oam_.fill(0xFF);
    }

    PPU::~PPU() = default;

    void PPU::Tick()
    {

    }

    byte PPU::Read(word address) const
    {
        if (address >= 0x8000 && address < 0xA000)
        {
            return vram_[address - 0x8000];
        }
        else if (address >= 0xFE00 && address < 0xFEA0)
        {
            return oam_[address - 0xFE00];
        }
        throw Exception("Invalid PPU address : " + std::to_string(address));
    }

    void PPU::Write(word address, byte value)
    {
        if (address >= 0x8000 && address < 0xA000)
        {
            vram_[address - 0x8000] = value;
        }
        else if (address >= 0xFE00 && address < 0xFEA0)
        {
            oam_[address - 0xFE00] = value;
        }
        else
            throw Exception("Invalid PPU address : " + std::to_string(address));
    }

    std::set<word> PPU::GetAddresses() const
    {
        std::set<word> result;
        for (word i = 0x8000; i < 0xA000; ++i)
            result.insert(i);

        for (word i = 0xFE00; i < 0xFEA0; ++i)
            result.insert(i);

        return result;
    }
} // namespace gandalf