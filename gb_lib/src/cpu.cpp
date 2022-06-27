#include "cpu.h"

#include <stdexcept>

#define SET_ZFLAG() registers_.f |= kZFlagMask;
#define SET_NFLAG() registers_.f |= kNFlagMask;
#define SET_HFLAG() registers_.f |= kHFlagMask;
#define SET_CFLAG() registers_.f |= kCFlagMask;

#define CLEAR_ZFLAG() registers_.f &= ~kZFlagMask;
#define CLEAR_NFLAG() registers_.f &= ~kNFlagMask;
#define CLEAR_HFLAG() registers_.f &= ~kHFlagMask;
#define CLEAR_CFLAG() registers_.f &= ~kCFlagMask;

#define READ(address, destination)  \
    timing_handler_.Advance(4); \
    (destination) = memory_.Read(address); \

#define WRITE(address, value)  \
    timing_handler_.Advance(4); \
    memory_.Write(address, value); \

#define READ_PC(destination) READ(registers_.program_counter++, destination)

#define LD_RR_NN(r1, r2) \
    READ_PC(r2); \
    READ_PC(r1); \

#define INC_RR(r) {\
    timing_handler_.Advance(4); \
    ++(r);} \

#define DEC_RR(r) \
    timing_handler_.Advance(4); \
    --(r); \

#define INC_R(r) \
    ++(r); \
    registers_.f &= ~(kZFlagMask | kHFlagMask | kNFlagMask); \
    if ((r) == 0) SET_ZFLAG() \
    if ((r) & 0x0F) CLEAR_HFLAG() \

#define DEC_R(r) \
    --(r); \
    registers_.f &= ~(kZFlagMask | kHFlagMask); \
    registers_.f |= (kNFlagMask); \
    if ((r) == 0) SET_ZFLAG() \
    if (((r) & 0x0F) == 0x0F) SET_HFLAG() \

#define RLCA() {\
    registers_.f &= 0xF0; \
    bool carry = registers_.a & 0x1;\
    registers_.a = registers_.a >> 1;\
    if (carry) \
    registers_.af |= kCFlagMask | 0x100;}\

#define RRCA() {\
    registers_.f &= 0xF0; \
    bool carry = registers_.a & 0x80;\
    registers_.a = registers_.a << 1;\
    if (carry) \
    registers_.af |= kCFlagMask | 0x8000;}\

#define LD_NN_SP() {\
    Memory::Value low, high;\
    READ_PC(low)\
    READ_PC(high)\
    std::uint16_t address = (high << 8) | low;\
    WRITE(address, registers_.stack_pointer & 0xFF)\
    WRITE(address + 1, registers_.stack_pointer >> 8)}\

#define ADD_HL_RR(r) {\
    std::uint16_t hl = registers_.hl; \
    timing_handler_.Advance(4); \
    registers_.hl += (r);\
    registers_.f &= ~(kNFlagMask | kCFlagMask | kHFlagMask);\
    if (((hl & 0xFFF) + ((r) & 0xFFF)) > 0xFFF) SET_HFLAG() \
    if (registers_.hl < hl) SET_CFLAG() }\

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
    case 0x01: LD_RR_NN(registers_.b, registers_.c) break;
    case 0x02: WRITE(registers_.bc, registers_.a) break;
    case 0x03: INC_RR(registers_.bc) break;
    case 0x04: INC_R(registers_.b) break;
    case 0x05: DEC_R(registers_.b) break;
    case 0x06: READ_PC(registers_.b) break;
    case 0x07: RLCA() break;
    case 0x08: LD_NN_SP() break;
    case 0x09: ADD_HL_RR(registers_.bc) break;
    case 0x0A: READ(registers_.bc, registers_.a) break;
    case 0x0B: DEC_RR(registers_.bc) break;
    case 0x0C: INC_R(registers_.c) break;
    case 0x0D: DEC_R(registers_.c) break;
    case 0x0E: READ_PC(registers_.c) break;
    case 0x0F: RRCA() break;

    case 0x13: INC_RR(registers_.de) break;
    case 0x23: INC_RR(registers_.hl) break;
    case 0x33: INC_RR(registers_.stack_pointer) break;
    case 0x11: LD_RR_NN(registers_.d, registers_.d) break;
    case 0x21: LD_RR_NN(registers_.h, registers_.l) break;
    }
}

void CPU::CheckInterrupts()
{
    if (registers_.interrupt_flags & registers_.interrupt_enable) {
        halt_ = false;
    }
}