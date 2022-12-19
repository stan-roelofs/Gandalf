#include <gandalf/bus.h>

namespace gandalf {
  Bus::AddressHandler::AddressHandler(const std::string& name): name_(name) {

  }

  Bus::AddressHandler::~AddressHandler() = default;

  Bus::Bus() {
    AddressWrapper w;
    w.access_level = AccessLevel::kNormal;
    w.handler = nullptr;
    address_space_.fill(w);
  }

  Bus::~Bus() = default;

  void Bus::Write(word address, byte value, AccessLevel level) {
    if (address_space_[address].access_level != kNormal && address_space_[address].access_level != level && level != Bus::AccessLevel::kDebug)
      return;

    if (address_space_[address].handler != nullptr) {
      address_space_[address].handler->Write(address, value);
    }
  }

  byte Bus::Read(word address, AccessLevel level) const {
    if (address_space_[address].access_level != kNormal && address_space_[address].access_level != level && level != Bus::AccessLevel::kDebug)
      return 0xFF;

    if (address_space_[address].handler != nullptr) {
      return address_space_[address].handler->Read(address);
    }

    return 0xFF;
  }

  void Bus::Register(AddressHandler& handler) {
    for (const word address : handler.GetAddresses()) {
      address_space_[address].handler = &handler;
    }
  }

  void Bus::Unregister(AddressHandler& handler)
  {
    for (const word address : handler.GetAddresses()) {
      address_space_[address].handler = nullptr;
    }
  }

  std::string Bus::GetAddressHandlerName(word address) const
  {
    if (!address_space_[address].handler)
      return "";

    return address_space_[address].handler->GetName();
  }

  void Bus::SetAccessLevel(AccessLevel level, word first, word last)
  {
    // TODO what if this overlaps? e.g. dma blocks access to OAM, as well as PPU
    if (first > last)
      std::swap(first, last);

    for (std::uint32_t start = first; start <= last; ++start)
      address_space_[start].access_level = level;
  }

} // namespace gandalf