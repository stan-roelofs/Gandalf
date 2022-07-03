#ifndef __GANDALF_BOOTROM_H
#define __GANDALF_BOOTROM_H

#include <gandalf/model.h>

#include "cpu_registers.h"
#include "bus.h"

namespace gandalf {

    void SimulateBootROM(Model model, Registers& registers, Bus& bus);

} // namespace gandalf
#endif
