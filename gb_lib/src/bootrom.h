#ifndef __GANDALF_BOOTROM_H
#define __GANDALF_BOOTROM_H

#include <gandalf/memory.h>
#include <gandalf/cpu_registers.h>
#include <gandalf/model.h>

namespace gandalf {
    std::vector<byte> GetBootROM(Model model);
} // namespace gandalf
#endif
