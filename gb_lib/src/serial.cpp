#include <gandalf/serial.h>

#include <gandalf/constants.h>

#include <iostream> // TODO remove

namespace gandalf {

    Serial::Serial() : Bus::AddressHandler("Serial"), sb_(0), sc_(0) {

    }

    Serial::~Serial() = default;

    void Serial::Tick() {

    }

    void Serial::Write(word address, byte value) {
        if (address == kSC) {
            if (value == 0x81)
                std::cout << sb_;
        }
        else if (address == kSB)
            sb_ = value;
       // else
          //  throw Exception("Invalid Serial address : " + std::to_string(address));
    }

    std::set<word> Serial::GetAddresses() const
    {
        return { kSB, kSC };
    }

    byte Serial::Read(word) const {
        return 0xFF; //TODO
    }

} // namespace gandalf