#include <gandalf/wram.h>

#include <cassert>

#include <gandalf/constants.h>
#include <gandalf/util.h>

namespace gandalf {
    WRAM::WRAM(GameboyMode mode) : Memory::AddressHandler("WRAM"),
        wram_bank_(1),
        mode_(mode)
    {
    }

    WRAM::~WRAM() = default;

    byte WRAM::Read(word address) const
    {
        assert(BETWEEN(address, 0xC000, 0xE000) || BETWEEN(address, 0xE000, 0xFE00) || address == kSVBK);

        if (address >= 0xC000 && address < 0xD000)
            return data_[0][address - 0xC000];
        else if (address >= 0xD000 && address < 0xE000)
            return data_[wram_bank_][address - 0xD000];
        else if (address >= 0xE000 && address < 0xF000)
            return data_[0][address - 0xE000];
        else if (address >= 0xF000 && address < 0xFE00)
            return data_[wram_bank_][address - 0xF000];
        else if (mode_ == GameboyMode::CGB && address == kSVBK)
            return (byte)(0xF8 | wram_bank_);

        return 0xFF;
    }

    void WRAM::Write(word address, byte value)
    {
        assert(BETWEEN(address, 0xC000, 0xE000) || BETWEEN(address, 0xE000, 0xFE00) || address == kSVBK);

        if (address >= 0xC000 && address < 0xD000)
            data_[0][address - 0xC000] = value;
        else if (address >= 0xD000 && address < 0xE000)
            data_[wram_bank_][address - 0xD000] = value;
        else if (address >= 0xE000 && address < 0xF000)
            data_[0][address - 0xE000] = value;
        else if (address >= 0xF000 && address < 0xFE00)
            data_[wram_bank_][address - 0xF000] = value;
        else if (mode_ == GameboyMode::CGB && address == kSVBK) {
            wram_bank_ = value & 0x7;
            if (wram_bank_ == 0)
                wram_bank_ = 1;
        }
    }

    std::set<word> WRAM::GetAddresses() const
    {
        std::set<word> result;

        for (word i = 0xC000; i < 0xE000; ++i)
            result.insert(i);

        for (word i = 0xE000; i < 0xFE00; ++i)
            result.insert(i);

        result.insert(kSVBK);
        return result;

    }
} // namespace gandalf