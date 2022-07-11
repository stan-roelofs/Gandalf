#include <gandalf/hram.h>

#include <cassert>

#include <gandalf/util.h>

namespace gandalf {
    HRAM::HRAM() : Bus::AddressHandler("HRAM") {
        data_.fill((byte)std::rand());
    }

    HRAM::~HRAM() = default;

    byte HRAM::Read(word address) const
    {
        assert(BETWEEN(address, 0xFF80, 0xFFFF));

        return data_[address - 0xFF80];
    }

    void HRAM::Write(word address, byte value)
    {
        assert(BETWEEN(address, 0xFF80, 0xFFFF));

        data_[address - 0xFF80] = value;
    }

    std::set<word> HRAM::GetAddresses() const
    {
        std::set<word> result;

        for (word i = 0xFF80; i < 0xFFFF; ++i)
            result.insert(i);

        return result;
    }
} // namespace gandalf