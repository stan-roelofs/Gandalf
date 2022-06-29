#ifndef __CPU_H
#define __CPU_H

#include "cpu_registers.h"
#include "memory.h"
#include "timing_handler.h"

namespace gandalf {

class CPU
{
public:
    CPU(TimingHandler& timing_handler, Memory& memory);
    ~CPU();

    void Run();

private:
    void CheckInterrupts();
    void Execute(std::uint8_t opcode);

    Registers registers_;
    Memory& memory_;
    TimingHandler& timing_handler_;
    std::uint8_t opcode_;
    bool halt_;
    bool stop_;
    bool halt_bug_;
};

} // namespace gandalf
#endif
