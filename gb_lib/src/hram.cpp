#include <gandalf/hram.h>

namespace gandalf {
    HRAM::HRAM() : Bus::AddressHandler("HRAM") {
        data_.fill(0xFF);
    }

    HRAM::~HRAM() = default;

    byte HRAM::Read(word address) const
    {
        if (address >= 0xFF80 && address < 0xFFFF)
            return data_[address - 0xFF80];

        return 0xFF; // TODO
        // throw Exception("Invalid HRAM address : " + std::to_string(address));
    }

    void HRAM::Write(word address, byte value)
    {
        if (address >= 0xFF80 && address < 0xFFFF)
            data_[address - 0xFF80] = value;
        //else
           // throw Exception("Invalid HRAM address : " + std::to_string(address));
    }

    std::set<word> HRAM::GetAddresses() const
    {
        std::set<word> result;

        for (word i = 0xFF80; i < 0xFFFF; ++i)
            result.insert(i);

        return result;
    }
} // namespace gandalf