#include <gandalf/bus.h>

#include <iostream> // TODO remove this

namespace gandalf {
  Bus::AddressHandler::AddressHandler(const std::string& name) : name_(name) {

  }

  Bus::AddressHandler::~AddressHandler() = default;

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
    // TODO if blocked, return 0xFF
    return DebugRead(address);
  }

  byte Bus::DebugRead(word address) const {
    if (address_space_[address] != nullptr) {
      return address_space_[address]->Read(address);
    }

    return 0xFF;
  }

  void Bus::Register(AddressHandler* handler) {
    if (handler == nullptr)
      return;

    for (const word address : handler->GetAddresses()) {
      address_space_[address] = handler;
    }
  }

  void Bus::Unregister(AddressHandler* handler)
  {
    if (handler == nullptr)
      return;

    for (const word address : handler->GetAddresses()) {
      address_space_[address] = nullptr;
    }
  }

} // namespace gandalf