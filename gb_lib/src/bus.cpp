#include "bus.h"

#include <cassert>

namespace gandalf {

  Bus::Bus() {
    address_space_.fill(nullptr);
  }

  Bus::~Bus() = default;

  void Bus::Write(word address, byte value) {
    if (address_space_[address] != nullptr) {
      address_space_[address]->Write(address, value);
    }
  }

  byte Bus::Read(word address) const {
    if (address_space_[address] != nullptr) {
      return address_space_[address]->Read(address);
    }

    return 0xFF;
  }

  void Bus::Attach(AddressRange* handler) {
    for (word address : handler->GetAddresses()) {
      assert(address_space_[address] == nullptr);
      address_space_[address] = handler;
    }
  }

} // namespace gandalf