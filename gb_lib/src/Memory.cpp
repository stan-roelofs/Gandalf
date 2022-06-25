#include "Memory.h"

Memory::Memory() {}

Memory::~Memory() {}

uint8_t& Memory::operator[](uint16_t address)
{
    return _memory[address];
}