#include "memory.h"

Memory::Memory() = default;

Memory::~Memory() = default;

void Memory::Write(Address address, Value value)
{
    memory_[address] = value;
}

Memory::Value Memory::Read(Address address) const
{
    return memory_[address];
}