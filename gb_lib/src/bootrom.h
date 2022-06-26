#ifndef BOOTROM
#define BOOTROM

#include <gameboy/model.h>

#include "cpu_registers.h"
#include "memory.h"

void SimulateBootROM(Model model, Registers& registers, Memory& memory);

#endif
