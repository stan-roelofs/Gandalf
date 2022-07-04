#include <gandalf/bus.h>

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

    throw std::runtime_error("Unsupported write to: " + std::to_string(address));
  }

  byte Bus::Read(word address) const {
    if (address_space_[address] != nullptr) {
      return address_space_[address]->Read(address);
    }

    throw std::runtime_error("Unsupported read from: " + std::to_string(address));
    // TODO
    //return 0xFF;
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