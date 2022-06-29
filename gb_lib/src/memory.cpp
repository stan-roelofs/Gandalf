#include "memory.h"

namespace gandalf {

Memory::Memory() = default;

Memory::~Memory() = default;

void Memory::Write(std::uint16_t address, std::uint8_t value)
{
    memory_[address] = value;
}

std::uint8_t Memory::Read(std::uint16_t address) const
{
    return memory_[address];
}

} // namespace gandalf