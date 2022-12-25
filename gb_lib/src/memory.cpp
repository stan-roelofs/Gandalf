#include <gandalf/memory.h>

namespace gandalf {
  Memory::AddressHandler::AddressHandler(const std::string& name): name_(name) {

  }

  Memory::AddressHandler::~AddressHandler() = default;

  Memory::Memory() {
    AddressWrapper w;
    w.access_level = AccessLevel::kNormal;
    w.handler = nullptr;
    address_space_.fill(w);
  }

  Memory::~Memory() = default;

  void Memory::Write(word address, byte value, AccessLevel level) {
    if (address_space_[address].access_level != kNormal && address_space_[address].access_level != level && level != Memory::AccessLevel::kDebug)
      return;

    if (address_space_[address].handler != nullptr) {
      address_space_[address].handler->Write(address, value);
    }
  }

  byte Memory::Read(word address, AccessLevel level) const {
    if (address_space_[address].access_level != kNormal && address_space_[address].access_level != level && level != Memory::AccessLevel::kDebug)
      return 0xFF;

    if (address_space_[address].handler != nullptr) {
      return address_space_[address].handler->Read(address);
    }

    return 0xFF;
  }

  void Memory::Register(AddressHandler& handler) {
    for (const word address : handler.GetAddresses()) {
      address_space_[address].handler = &handler;
    }
  }

  void Memory::Unregister(AddressHandler& handler)
  {
    for (const word address : handler.GetAddresses()) {
      address_space_[address].handler = nullptr;
    }
  }

  std::string Memory::GetAddressHandlerName(word address) const
  {
    if (!address_space_[address].handler)
      return "";

    return address_space_[address].handler->GetName();
  }

  void Memory::SetAccessLevel(AccessLevel level, word first, word last)
  {
    // TODO what if this overlaps? e.g. dma blocks access to OAM, as well as PPU
    if (first > last)
      std::swap(first, last);

    for (std::uint32_t start = first; start <= last; ++start)
      address_space_[start].access_level = level;
  }

} // namespace gandalf