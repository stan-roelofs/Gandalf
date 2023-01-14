#ifndef __GB_APP_DEBUGGER_H
#define __GB_APP_DEBUGGER_H

#include <gandalf/memory.h>

namespace gui
{
    namespace debugger
    {
        struct Instruction
        {
            const char* name;
            gandalf::byte length;
        };

        const Instruction& DecodeInstruction(gandalf::byte opcode);
    }
}

#endif