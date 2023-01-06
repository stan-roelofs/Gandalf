#include <gandalf/memory.h>

#include <gandalf/exception.h>

namespace gandalf {
  Memory::AddressHandler::AddressHandler(const std::string& name): name_(name) {

  }

  Memory::AddressHandler::~AddressHandler() = default;

  Memory::Memory() {
    AddressWrapper w;
    w.blocked = false;
    w.handler = nullptr;
    address_space_.fill(w);
  }

  Memory::~Memory() = default;

  void Memory::Write(word address, byte value, bool check_access) {
    if (check_access && address_space_[address].blocked) // TODO ?
      return;

    if (address_space_[address].handler != nullptr) {
      address_space_[address].handler->Write(address, value);
    }
  }

  byte Memory::Read(word address, bool check_access) const {
    if (check_access && address_space_[address].blocked)
      return 0xFF; // TODO this is not correct. It should return the value of the last read.

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

  Memory::Bus Memory::GetBus(word address)
  {
    if (address <= 0x7FFF)
      return Bus::External;
    else if (address <= 0x9FFF)
      return Bus::VideoRAM;
    else if (address <= 0xFDFF)
      return Bus::External;
    else if (address >= 0xFE00 && address <= 0xFE9F)
      return Bus::OAM;
    else
      throw Exception("Invalid address");
  }

  void Memory::Block(Bus bus, bool block)
  {
    switch (bus) {
    case Bus::External:
      for (std::uint32_t start = 0; start <= 0x7FFF; ++start)
        address_space_[start].blocked = block;
      for (std::uint32_t start = 0xA000; start <= 0xFDFF; ++start)
        address_space_[start].blocked = block;
      break;
    case Bus::VideoRAM:
      for (std::uint32_t start = 0x8000; start <= 0x9FFF; ++start)
        address_space_[start].blocked = block;
      break;
    case Bus::OAM:
      for (std::uint32_t start = 0xFE00; start <= 0xFE9F; ++start)
        address_space_[start].blocked = block;
      break;
    default:
      throw Exception("Invalid bus");
    }
  }

} // namespace gandalf