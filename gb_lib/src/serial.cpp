#include <gandalf/serial.h>

#include <cassert>

#include <gandalf/constants.h>

namespace gandalf {

    Serial::Serial() : Bus::AddressHandler("Serial"), sb_(0), in_progress_(false), fast_clock_speed_(false), internal_clock_(false)
    {
    }

    Serial::~Serial() = default;

    void Serial::Tick() {

    }

    void Serial::Write(word address, byte value) {
        assert(address == kSB || address == kSC);

        if (address == kSB)
            sb_ = value;
        else if (address == kSC) {
            in_progress_ = value & 0x80;
            fast_clock_speed_ = value & 0x2;
            internal_clock_ = value & 0x1;
        }
    }

    std::set<word> Serial::GetAddresses() const
    {
        return { kSB, kSC };
    }

    byte Serial::Read(word address) const {
        assert(address == kSB || address == kSC);

        if (address == kSB)
            return sb_;
        else if (address == kSC)
            return !!in_progress_ << 7 | !!fast_clock_speed_ << 1 | !!internal_clock_;

        return 0xFF;
    }

} // namespace gandalf