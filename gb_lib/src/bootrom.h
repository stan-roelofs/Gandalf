#ifndef BOOTROM
#define BOOTROM

#include <gandalf/model.h>

#include "cpu_registers.h"
#include "memory.h"

namespace gandalf {

    void SimulateBootROM(Model model, Registers& registers, Memory& memory);

} // namespace gandalf
#endif
