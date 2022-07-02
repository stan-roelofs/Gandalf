#include "memory.h"

namespace gandalf {

  Memory::Memory() = default;

  Memory::~Memory() = default;

  void Memory::Write(word address, byte value) {
    memory_[address] = value;
  }

  byte Memory::Read(word address) const {
    return memory_[address];
  }

} // namespace gandalf