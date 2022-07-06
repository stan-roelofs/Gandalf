#include <gandalf/wram.h>

namespace gandalf {
    WRAM::WRAM() : Bus::AddressHandler("WRAM") {
        data_.fill(0xFF);
    }

    WRAM::~WRAM() = default;

    byte WRAM::Read(word address) const
    {
        if (address >= 0xC000 && address < 0xE000)
            return data_[address - 0xC000];
        else if (address >= 0xE000 && address < 0xFE00)
            return data_[address - 0xE000];

        throw Exception("Invalid WRAM address : " + std::to_string(address));
    }

    void WRAM::Write(word address, byte value)
    {
        if (address >= 0xC000 && address < 0xE000)
            data_[address - 0xC000] = value;
        else if (address >= 0xE000 && address < 0xFE00)
            data_[address - 0xE000] = value;
        else
            throw Exception("Invalid WRAM address : " + std::to_string(address));
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