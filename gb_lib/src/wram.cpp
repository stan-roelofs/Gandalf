#include <gandalf/wram.h>

#include <cassert>

#include <gandalf/util.h>

namespace gandalf {
    WRAM::WRAM() : Bus::AddressHandler("WRAM") {
        data_.fill((byte)std::rand());
    }

    WRAM::~WRAM() = default;

    byte WRAM::Read(word address) const
    {
        assert(BETWEEN(address, 0xC000, 0xE000) || BETWEEN(address, 0xE000, 0xFE00));

        if (address >= 0xC000 && address < 0xE000)
            return data_[address - 0xC000];
        else if (address >= 0xE000 && address < 0xFE00)
            return data_[address - 0xE000];

        return 0xFF;
    }

    void WRAM::Write(word address, byte value)
    {
        assert(BETWEEN(address, 0xC000, 0xE000) || BETWEEN(address, 0xE000, 0xFE00));

        if (address >= 0xC000 && address < 0xE000)
            data_[address - 0xC000] = value;
        else if (address >= 0xE000 && address < 0xFE00)
            data_[address - 0xE000] = value;
    }

    std::set<word> WRAM::GetAddresses() const
    {
        std::set<word> result;

        for (word i = 0xC000; i < 0xE000; ++i)
            result.insert(i);

        for (word i = 0xE000; i < 0xFE00; ++i)
            result.insert(i);

        return result;

    }
} // namespace gandalf