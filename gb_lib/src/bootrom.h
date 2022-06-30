#ifndef BOOTROM
#define BOOTROM

#include <gameboy/model.h>

#include "cpu_registers.h"
#include "memory.h"

namespace gandalf {

void SimulateBootROM(Model model, Registers &registers, Memory &memory);

} // namespace gandalf
#endif
