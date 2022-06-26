#include "cpu.h"

#include <stdexcept>

#define READ(destination, address) { \
    (destination) = memory_.Read(address); \
    timing_handler_.Advance(4); } \

#define WRITE(destination, value) { \
    memory_.Write(destination, value); \
    timing_handler_.Advance(4); }\

#define READ_PC(destination) READ(destination, registers_.program_counter++);

#define LD_RR_NN(r1, r2) {\
    std::uint16_t value; \
    READ(value, r2); \
    READ(value, r1); }\

#define INC_RR(r) {\
    ++(r); \
    timing_handler_.Advance(4); } \


CPU::CPU(TimingHandler& timing_handler, Memory& memory) : memory_(memory), timing_handler_(timing_handler)
{
}

CPU::~CPU() = default;

void CPU::Run()
{
    if (stop_)
    {
        // TODO
    }

    std::uint8_t interrupt_queue = registers_.interrupt_enable & registers_.interrupt_flags & 0x1F;
    if (interrupt_queue) {
        halt_ = false;

        registers_.interrupt_master_enable = false;
    }

    if (!halt_) {
        READ_PC(opcode_);
        Execute(opcode_);
    }
}

void CPU::Execute(Memory::Value opcode)
{
    switch (opcode)
    {
    case 0x00: break;
    case 0x01: LD_RR_NN(registers_.b, registers_.c); break;
    case 0x02: WRITE(registers_.bc, registers_.a); break;
    case 0x03: INC_RR(registers_.bc); break;
    case 0x13: INC_RR(registers_.de); break;
    case 0x23: INC_RR(registers_.hl); break;
    case 0x33: INC_RR(registers_.stack_pointer); break;
    case 0x11: LD_RR_NN(registers_.d, registers_.d); break;
    case 0x21: LD_RR_NN(registers_.h, registers_.l); break;
    }
}

void CPU::CheckInterrupts()
{
    if (registers_.interrupt_flags & registers_.interrupt_enable) {
        halt_ = false;
    }
}