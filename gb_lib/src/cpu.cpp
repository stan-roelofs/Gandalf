#include "cpu.h"

#include <stdexcept>

#define SET_ZFLAG() registers_.f() |= kZFlagMask;
#define SET_NFLAG() registers_.f() |= kNFlagMask;
#define SET_HFLAG() registers_.f() |= kHFlagMask;
#define SET_CFLAG() registers_.f() |= kCFlagMask;

#define CLEAR_ZFLAG() registers_.f() &= ~kZFlagMask;
#define CLEAR_NFLAG() registers_.f() &= ~kNFlagMask;
#define CLEAR_HFLAG() registers_.f() &= ~kHFlagMask;
#define CLEAR_CFLAG() registers_.f() &= ~kCFlagMask;

#define READ(address, destination)  \
    timing_handler_.Advance(4); \
    (destination) = memory_.Read(address); \

#define WRITE(address, value)  \
    timing_handler_.Advance(4); \
    memory_.Write(address, value); \

#define READ_PC(destination) READ(registers_.program_counter++, destination)

#define LD_RR_NN(r) {\
    std::uint8_t low, high; \
    READ_PC(low); \
    READ_PC(high); \
    r = (high << 8) | low; }\

#define INC_RR(r) {\
    timing_handler_.Advance(4); \
    ++(r);} \

#define DEC_RR(r) \
    timing_handler_.Advance(4); \
    --(r); \

#define INC_R(r) \
    ++(r); \
    registers_.f() &= ~(kZFlagMask | kHFlagMask | kNFlagMask); \
    if ((r) == 0) SET_ZFLAG() \
    if ((r) & 0x0F) CLEAR_HFLAG() \

#define DEC_R(r) \
    --(r); \
    registers_.f() &= ~(kZFlagMask | kHFlagMask); \
    registers_.f() |= (kNFlagMask); \
    if ((r) == 0) SET_ZFLAG() \
    if (((r) & 0x0F) == 0x0F) SET_HFLAG() \

#define RLC(r) {\
    registers_.f() &= 0xF0; \
    const bool carry = (r) & 0x80;\
    (r) = ((r) << 1) | carry;\
    if (carry) {\
        SET_CFLAG() \
    } \
    if ((r) == 0) SET_ZFLAG() }\

#define RRC(r) {\
    registers_.f() &= 0xF0; \
    const bool carry = r & 0x1;\
    (r) = (r) >> 1;\
    if (carry) {\
        SET_CFLAG() \
        (r) |= 0x80; \
    } \
    if ((r) == 0) SET_ZFLAG() }\

#define RL(r) {\
    registers_.f() &= 0xF0; \
    const bool carry = registers_.f() & kCFlagMask;\
    const bool new_carry = (r) & 0x80; \
    (r) = ((r) << 1) | carry; \
    if (new_carry) SET_CFLAG() \
    if ((r) == 0) SET_ZFLAG() }\

#define RR(r) {\
    registers_.f() &= 0xF0; \
    const bool carry = registers_.f() & kCFlagMask;\
    const bool new_carry = (r) & 0x1; \
    (r) = (r) >> 1; \
    if (new_carry) { SET_CFLAG(); r |= 0x80; } \
    if ((r) == 0) SET_ZFLAG() }\

#define RRCA() {\
    registers_.f() &= 0xF0; \
    const bool carry = registers_.a() & 0x1;\
    registers_.a() = registers_.a() >> 1;\
    if (carry) {\
        SET_CFLAG() \
        registers_.a() |= 0x80; \
    } \
    CLEAR_ZFLAG() }\

#define RRA() {\
    registers_.f() &= 0xF0; \
    const bool carry = registers_.f() & kCFlagMask;\
    const bool new_carry = registers_.a() & 0x1; \
    registers_.a() = registers_.a() >> 1; \
    if (carry) registers_.a() |= 0x80; \
    if (new_carry) SET_CFLAG() }\

#define RLCA() {\
    registers_.f() &= 0xF0; \
    const bool carry = registers_.a() & 0x80;\
    registers_.a() = (registers_.a() << 1) | carry;\
    if (carry) {\
        SET_CFLAG() \
    } }\

#define RLA() {\
    registers_.f() &= 0xF0; \
    const bool carry = registers_.f() & kCFlagMask;\
    const bool new_carry = registers_.a() & 0x80; \
    registers_.a() = (registers_.a() << 1) | carry; \
    if (new_carry) SET_CFLAG() }\

#define LD_NN_SP() {\
    std::uint8_t low, high;\
    READ_PC(low)\
    READ_PC(high)\
    std::uint16_t address = (high << 8) | low;\
    WRITE(address, registers_.stack_pointer & 0xFF)\
    WRITE(address + 1, registers_.stack_pointer >> 8)}\

#define ADD_HL_RR(r) {\
    std::uint16_t hl = registers_.hl(); \
    timing_handler_.Advance(4); \
    registers_.hl() += (r);\
    registers_.f() &= ~(kNFlagMask | kCFlagMask | kHFlagMask);\
    if (((hl & 0xFFF) + ((r) & 0xFFF)) > 0xFFF) SET_HFLAG() \
    if (registers_.hl() < hl) SET_CFLAG() }\

#define JR_N() {\
    std::int8_t value;\
    READ_PC(value); \
    registers_.program_counter += value;\
    timing_handler_.Advance(4); }\

#define JR_CC_N(condition) {\
    std::int8_t value;\
    READ_PC(value) \
    if (condition) registers_.program_counter += value; }\

#define DAA() \
    if (registers_.f() & kNFlagMask) {\
        if (registers_.f() & kCFlagMask) \
            registers_.a() -= 0x60; \
        if (registers_.f() & kHFlagMask) \
            registers_.a() -= 0x06; \
    } else {\
        if (registers_.f() & kCFlagMask || registers_.a() > 0x99) { \
            registers_.a() += 0x60; \
            SET_CFLAG() \
        }\
        if (registers_.f() & kHFlagMask || (registers_.a() & 0x0F) > 0x09) \
            registers_.a() += 0x06; \
    } \
    if (registers_.a() == 0) SET_ZFLAG() \
    CLEAR_HFLAG() \

#define CPL() \
    registers_.a() = ~registers_.a(); \
    SET_NFLAG() \
    SET_HFLAG() \

#define INC_HL() {\
    std::uint8_t value; \
    READ(registers_.hl(), value); \
    ++value; \
    WRITE(registers_.hl(), value); \
    registers_.f() &= ~(kZFlagMask | kNFlagMask | kHFlagMask); \
    if (value == 0) SET_ZFLAG() \
    if ((value & 0x0F) == 0) SET_HFLAG() }\

#define DEC_HL() {\
    std::uint8_t value; \
    READ(registers_.hl(), value); \
    --value; \
    WRITE(registers_.hl(), value); \
    registers_.f() &= ~(kZFlagMask | kHFlagMask); \
    registers_.f() |= kNFlagMask; \
    if (value == 0) SET_ZFLAG() \
    if ((value & 0x0F) == 0x0F) SET_HFLAG() }\

#define LD_HL_N() {\
    std::uint8_t value; \
    READ_PC(value); \
    WRITE(registers_.hl(), value); }\

#define SCF() registers_.f() = (registers_.f() & 0xF8) | kCFlagMask;
#define CCF() registers_.f() = (registers_.f() & 0xF8) | ((registers_.f() & kCFlagMask) ^ kCFlagMask);

#define HALT() \
    if (registers_.interrupt_master_enable) { \
        halt_ = true; \
        return; \
    }\
    if ((registers_.interrupt_enable & registers_.interrupt_flags) == 0) \
        halt_ = true; \
    else \
        halt_bug_ = true; \

#define ADD_A(value) {\
    std::uint8_t a = registers_.a(); \
    registers_.a() += (value); \
    registers_.f() &= 0xF0; \
    if (a + (value) == 0) SET_ZFLAG() \
    if ((a & 0xF) + (value & 0xF) > 0xF) SET_HFLAG() \
    if (a + (value) > 0xFF) SET_CFLAG() \
}\

#define ADD_A_HL() {\
    std::uint8_t value; \
    READ(registers_.hl(), value); \
    ADD_A(value); }\

#define ADC_A(value) {\
    std::uint8_t a = registers_.a(); \
    const bool carry = registers_.f() & kCFlagMask; \
    registers_.a() += (value + carry); \
    registers_.f() &= 0xF0; \
    if (a + (value) + carry == 0) SET_ZFLAG() \
    if ((a & 0xF) + (value & 0xF) + carry > 0xF) SET_HFLAG() \
    if (a + (value) + carry > 0xFF) SET_CFLAG() \
}\

#define ADC_A_HL() {\
    std::uint8_t value; \
    READ(registers_.hl(), value); \
    ADC_A(value); }\

#define SUB_A(value) {\
    std::uint8_t a = registers_.a(); \
    registers_.a() -= (value); \
    registers_.f() = (registers_.f() & 0xF0) | kNFlagMask; \
    if (a == value) SET_ZFLAG() \
    if ((a & 0xF) < (value & 0xF)) SET_HFLAG() \
    if (a < value) SET_CFLAG() }\

#define SUB_A_HL() {\
    std::uint8_t value; \
    READ(registers_.hl(), value); \
    SUB_A(value); }\

#define SBC_A(value) {\
    std::uint8_t a = registers_.a(); \
    const bool carry = registers_.f() & kCFlagMask; \
    registers_.a() = registers_.a() - (value) - carry; \
    registers_.f() = (registers_.f() & 0xF0) | kNFlagMask; \
    if (a - value == carry) SET_ZFLAG() \
    if ((a & 0xF) < (value & 0xF) + carry) SET_HFLAG() \
    if (a < value + carry) SET_CFLAG() }\

#define SBC_A_HL() {\
    std::uint8_t value; \
    READ(registers_.hl(), value); \
    SBC_A(value); }\

#define AND_A(value) \
    registers_.a() &= (value); \
    registers_.f() = kHFlagMask | (registers_.a() == 0 ? kZFlagMask : 0); \

#define AND_A_HL() {\
    std::uint8_t value; \
    READ(registers_.hl(), value); \
    AND_A(value); }\

#define XOR_A(value) \
    registers_.a() ^= (value); \
    if (registers_.a() == 0) SET_ZFLAG() \

#define XOR_A_HL() {\
    std::uint8_t value; \
    READ(registers_.hl(), value); \
    XOR_A(value); }\

namespace gandalf {

CPU::CPU(TimingHandler& timing_handler, Memory& memory) : memory_(memory), timing_handler_(timing_handler), halt_(false), stop_(false), halt_bug_(false)
{
}

CPU::~CPU() = default;

void CPU::Run()
{
    if (stop_)
    {
        return; // TODO
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

void CPU::Execute(std::uint8_t opcode)
{
    switch (opcode)
    {
    case 0x00: break;
    case 0x01: LD_RR_NN(registers_.bc()) break;
    case 0x02: WRITE(registers_.bc(), registers_.a()) break;
    case 0x03: INC_RR(registers_.bc()) break;
    case 0x04: INC_R(registers_.b()) break;
    case 0x05: DEC_R(registers_.b()) break;
    case 0x06: READ_PC(registers_.b()) break;
    case 0x07: RLCA() break;
    case 0x08: LD_NN_SP() break;
    case 0x09: ADD_HL_RR(registers_.bc()) break;
    case 0x0A: READ(registers_.bc(), registers_.a()) break;
    case 0x0B: DEC_RR(registers_.bc()) break;
    case 0x0C: INC_R(registers_.c()) break;
    case 0x0D: DEC_R(registers_.c()) break;
    case 0x0E: READ_PC(registers_.c()) break;
    case 0x0F: RRCA() break;
    case 0x10:
        stop_ = true;
        break;
    case 0x11: LD_RR_NN(registers_.de()) break;
    case 0x12: WRITE(registers_.de(), registers_.a()) break;
    case 0x13: INC_RR(registers_.de()) break;
    case 0x14: INC_R(registers_.d()) break;
    case 0x15: DEC_R(registers_.d()) break;
    case 0x16: READ_PC(registers_.d()) break;
    case 0x17: RLA() break;
    case 0x18: JR_N() break;
    case 0x19: ADD_HL_RR(registers_.de()) break;
    case 0x1A: READ(registers_.de(), registers_.a()) break;
    case 0x1B: DEC_RR(registers_.de()) break;
    case 0x1C: INC_R(registers_.e()) break;
    case 0x1D: DEC_R(registers_.e()) break;
    case 0x1E: READ_PC(registers_.e()) break;
    case 0x1F: RRA() break;
    case 0x20: JR_CC_N((registers_.f() & kZFlagMask) == 0) break;
    case 0x21: LD_RR_NN(registers_.hl()) break;
    case 0x22: WRITE(registers_.hl()++, registers_.a()) break;
    case 0x23: INC_RR(registers_.hl()) break;
    case 0x24: INC_R(registers_.h()) break;
    case 0x25: DEC_R(registers_.h()) break;
    case 0x26: READ_PC(registers_.h()) break;
    case 0x27: DAA() break;
    case 0x28: JR_CC_N(registers_.f() & kZFlagMask) break;
    case 0x29: ADD_HL_RR(registers_.hl()) break;
    case 0x2A: READ(registers_.hl()++, registers_.a()) break;
    case 0x2B: DEC_RR(registers_.hl()) break;
    case 0x2C: INC_R(registers_.l()) break;
    case 0x2D: DEC_R(registers_.l()) break;
    case 0x2E: READ_PC(registers_.l()) break;
    case 0x2F: CPL() break;
    case 0x30: JR_CC_N((registers_.f() & kCFlagMask) == 0) break;
    case 0x31: LD_RR_NN(registers_.stack_pointer) break;
    case 0x32: WRITE(registers_.hl()--, registers_.a()) break;
    case 0x33: INC_RR(registers_.stack_pointer) break;
    case 0x34: INC_HL() break;
    case 0x35: DEC_HL() break;
    case 0x36: LD_HL_N() break;
    case 0x37: SCF() break;
    case 0x38: JR_CC_N(registers_.f() & kCFlagMask) break;
    case 0x39: ADD_HL_RR(registers_.stack_pointer) break;
    case 0x3A: READ(registers_.hl()--, registers_.a()) break;
    case 0x3B: DEC_RR(registers_.stack_pointer) break;
    case 0x3C: INC_R(registers_.a()) break;
    case 0x3D: DEC_R(registers_.a()) break;
    case 0x3E: READ_PC(registers_.a()) break;
    case 0x3F: CCF() break;
    case 0x40: break;
    case 0x41: registers_.b() = registers_.c(); break;
    case 0x42: registers_.b() = registers_.d(); break;
    case 0x43: registers_.b() = registers_.e(); break;
    case 0x44: registers_.b() = registers_.h(); break;
    case 0x45: registers_.b() = registers_.l(); break;
    case 0x46: READ(registers_.hl(), registers_.b()) break;
    case 0x47: registers_.b() = registers_.a(); break;
    case 0x48: registers_.c() = registers_.b(); break;
    case 0x49: break;
    case 0x4A: registers_.c() = registers_.d(); break;
    case 0x4B: registers_.c() = registers_.e(); break;
    case 0x4C: registers_.c() = registers_.h(); break;
    case 0x4D: registers_.c() = registers_.l(); break;
    case 0x4E: READ(registers_.hl(), registers_.c()) break;
    case 0x4F: registers_.c() = registers_.a(); break;
    case 0x50: registers_.d() = registers_.b(); break;
    case 0x51: registers_.d() = registers_.c(); break;
    case 0x52: break;
    case 0x53: registers_.d() = registers_.e(); break;
    case 0x54: registers_.d() = registers_.h(); break;
    case 0x55: registers_.d() = registers_.l(); break;
    case 0x56: READ(registers_.hl(), registers_.d()) break;
    case 0x57: registers_.d() = registers_.a(); break;
    case 0x58: registers_.e() = registers_.b(); break;
    case 0x59: registers_.e() = registers_.c(); break;
    case 0x5A: registers_.e() = registers_.d(); break;
    case 0x5B: break;
    case 0x5C: registers_.e() = registers_.h(); break;
    case 0x5D: registers_.e() = registers_.l(); break;
    case 0x5E: READ(registers_.hl(), registers_.e()) break;
    case 0x5F: registers_.e() = registers_.a(); break;
    case 0x60: registers_.h() = registers_.b(); break;
    case 0x61: registers_.h() = registers_.c(); break;
    case 0x62: registers_.h() = registers_.d(); break;
    case 0x63: registers_.h() = registers_.e(); break;
    case 0x64: break;
    case 0x65: registers_.h() = registers_.l(); break;
    case 0x66: READ(registers_.hl(), registers_.h()) break;
    case 0x67: registers_.h() = registers_.a(); break;
    case 0x68: registers_.l() = registers_.b(); break;
    case 0x69: registers_.l() = registers_.c(); break;
    case 0x6A: registers_.l() = registers_.d(); break;
    case 0x6B: registers_.l() = registers_.e(); break;
    case 0x6C: registers_.l() = registers_.h(); break;
    case 0x6D: break;
    case 0x6E: READ(registers_.hl(), registers_.l()) break;
    case 0x6F: registers_.l() = registers_.a(); break;
    case 0x70: WRITE(registers_.hl(), registers_.b()) break;
    case 0x71: WRITE(registers_.hl(), registers_.c()) break;
    case 0x72: WRITE(registers_.hl(), registers_.d()) break;
    case 0x73: WRITE(registers_.hl(), registers_.e()) break;
    case 0x74: WRITE(registers_.hl(), registers_.h()) break;
    case 0x75: WRITE(registers_.hl(), registers_.l()) break;
    case 0x76: HALT() break;
    case 0x77: WRITE(registers_.hl(), registers_.a()) break;
    case 0x78: registers_.a() = registers_.b(); break;
    case 0x79: registers_.a() = registers_.c(); break;
    case 0x7A: registers_.a() = registers_.d(); break;
    case 0x7B: registers_.a() = registers_.e(); break;
    case 0x7C: registers_.a() = registers_.h(); break;
    case 0x7D: registers_.a() = registers_.l(); break;
    case 0x7E: READ(registers_.hl(), registers_.a()) break;
    case 0x7F: break;
    case 0x80: ADD_A(registers_.b()) break;
    case 0x81: ADD_A(registers_.c()) break;
    case 0x82: ADD_A(registers_.d()) break;
    case 0x83: ADD_A(registers_.e()) break;
    case 0x84: ADD_A(registers_.h()) break;
    case 0x85: ADD_A(registers_.l()) break;
    case 0x86: ADD_A_HL() break;
    case 0x87: ADD_A(registers_.a()) break;
    case 0x88: ADC_A(registers_.b()) break;
    case 0x89: ADC_A(registers_.c()) break;
    case 0x8A: ADC_A(registers_.d()) break;
    case 0x8B: ADC_A(registers_.e()) break;
    case 0x8C: ADC_A(registers_.h()) break;
    case 0x8D: ADC_A(registers_.l()) break;
    case 0x8E: ADC_A_HL() break;
    case 0x8F: ADC_A(registers_.a()) break;
    case 0x90: SUB_A(registers_.b()) break;
    case 0x91: SUB_A(registers_.c()) break;
    case 0x92: SUB_A(registers_.d()) break;
    case 0x93: SUB_A(registers_.e()) break;
    case 0x94: SUB_A(registers_.h()) break;
    case 0x95: SUB_A(registers_.l()) break;
    case 0x96: SUB_A_HL() break;
    case 0x97: SUB_A(registers_.a()) break;
    case 0x98: SBC_A(registers_.b()) break;
    case 0x99: SBC_A(registers_.c()) break;
    case 0x9A: SBC_A(registers_.d()) break;
    case 0x9B: SBC_A(registers_.e()) break;
    case 0x9C: SBC_A(registers_.h()) break;
    case 0x9D: SBC_A(registers_.l()) break;
    case 0x9E: SBC_A_HL() break;
    case 0x9F: SBC_A(registers_.a()) break;
    case 0xA0: AND_A(registers_.b()) break;
    case 0xA1: AND_A(registers_.c()) break;
    case 0xA2: AND_A(registers_.d()) break;
    case 0xA3: AND_A(registers_.e()) break;
    case 0xA4: AND_A(registers_.h()) break;
    case 0xA5: AND_A(registers_.l()) break;
    case 0xA6: AND_A_HL() break;
    case 0xA7: registers_.f() = kHFlagMask | (registers_.a() == 0 ? kZFlagMask : 0); break;
    case 0xA8: XOR_A(registers_.b()) break;
    case 0xA9: XOR_A(registers_.c()) break;
    case 0xAA: XOR_A(registers_.d()) break;
    case 0xAB: XOR_A(registers_.e()) break;
    case 0xAC: XOR_A(registers_.h()) break;
    case 0xAD: XOR_A(registers_.l()) break;
    case 0xAE: XOR_A_HL() break;
    case 0xAF: registers_.af() = kZFlagMask; break; // XOR A, A

    }
}

void CPU::CheckInterrupts()
{
    if (registers_.interrupt_flags & registers_.interrupt_enable) {
        halt_ = false;
    }
}
} // namespace gandalf