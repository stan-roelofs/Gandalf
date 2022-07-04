#ifndef __GANDALF_BOOTROM_H
#define __GANDALF_BOOTROM_H

#include <gandalf/bus.h>
#include <gandalf/cpu_registers.h>
#include <gandalf/model.h>

namespace gandalf {

    void SimulateBootROM(Model model, Registers& registers, Bus& bus);

} // namespace gandalf
#endif
