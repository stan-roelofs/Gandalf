#ifndef BOOTROM
#define BOOTROM

#include <Gameboy/Model.h>

#include "Cpu.h"
#include "Memory.h"

void SimulateBootROM(Model model, CPU::Registers& registers, Memory& memory);

#endif
