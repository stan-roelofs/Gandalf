#include "memory.h"

Memory::Memory() {}

Memory::~Memory() {}

Memory::Value Memory::Read(Address address)
{
    return memory_[address];
}

void Memory::Write(Address address, Value value)
{
    memory_[address] = value;
}