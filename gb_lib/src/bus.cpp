#include "bus.h"

namespace gandalf {

  Bus::Bus() = default;

  Bus::~Bus() = default;

  void Bus::Write(word address, byte value) {
    memory_[address] = value;
  }

  byte Bus::Read(word address) const {
    return memory_[address];
  }

} // namespace gandalf