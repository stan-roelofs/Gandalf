#ifndef __CPU_H
#define __CPU_H

#include "cpu_registers.h"
#include "memory.h"
#include "timing_handler.h"

class CPU
{
public:
    CPU(TimingHandler& timing_handler, Memory& memory);
    ~CPU();

    void Run();

private:
    void CheckInterrupts();
    void Execute(Memory::Value opcode);

    Registers registers_;
    Memory& memory_;
    TimingHandler& timing_handler_;
    Memory::Value opcode_;
    bool halt_;
    bool stop_;
};

#endif
