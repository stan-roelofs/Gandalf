#include <gandalf/memory.h>

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

  void Memory::Write(word address, byte value) {
    //if (address_space_[address].blocked) // TODO ?
      //return;

    if (address_space_[address].handler != nullptr) {
      address_space_[address].handler->Write(address, value);
    }
  }

  byte Memory::Read(word address) const {
    if (address_space_[address].blocked)
	  return 0xFF; // TODO this is not correct. It should return the value of the last read.

    if (address_space_[address].handler != nullptr) {
      return address_space_[address].handler->Read(address);
    }

    return 0xFF;
  }

  byte Memory::DebugRead(word address) const {
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
          return Bus::kExternal;
      else if (address <= 0x9FFF)
          return Bus::kVideoRAM;
      else
		  throw std::runtime_error("Invalid address");
  }

  void Memory::Block(Bus bus, bool block)
  {
    word first, last;
    switch (bus) {
    case Bus::kExternal:
      first = 0x0000;
      last = 0x7FFF;
      break;
    case Bus::kVideoRAM:
      first = 0x8000;
      last = 0x9FFF;
      break;
    default:
      throw std::runtime_error("Invalid bus");
    }

    for (std::uint32_t start = first; start <= last; ++start)
      address_space_[start].blocked = block;
  }

} // namespace gandalf