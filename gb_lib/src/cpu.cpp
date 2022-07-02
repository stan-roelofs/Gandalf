#include "cpu.h"

#include <stdexcept>
#include <string>

namespace gandalf {

#define SET_ZFLAG() registers_.f() |= kZFlagMask;
#define SET_NFLAG() registers_.f() |= kNFlagMask;
#define SET_HFLAG() registers_.f() |= kHFlagMask;
#define SET_CFLAG() registers_.f() |= kCFlagMask;

#define CLEAR_ZFLAG() registers_.f() &= ~kZFlagMask;
#define CLEAR_NFLAG() registers_.f() &= ~kNFlagMask;
#define CLEAR_HFLAG() registers_.f() &= ~kHFlagMask;
#define CLEAR_CFLAG() registers_.f() &= ~kCFlagMask;

#define READ(address, destination)                                             \
  timing_handler_.Advance(4);                                                  \
  (destination) = memory_.Read(address);

#define WRITE(address, value)                                                  \
  timing_handler_.Advance(4);                                                  \
  memory_.Write(address, value);

#define READ_PC(destination) READ(registers_.program_counter++, destination)
#define READ_SP(destination) READ(registers_.stack_pointer++, destination)
#define WRITE_SP(destination) WRITE(--registers_.stack_pointer, destination)

#define LD_RR_NN(r)                                                            \
  {                                                                            \
    byte low, high;                                                    \
    READ_PC(low);                                                              \
    READ_PC(high);                                                             \
    r = (high << 8) | low;                                                     \
  }

#define INC_RR(r)                                                              \
  {                                                                            \
    timing_handler_.Advance(4);                                                \
    ++(r);                                                                     \
  }

#define DEC_RR(r)                                                              \
  timing_handler_.Advance(4);                                                  \
  --(r);

#define INC_R(r)                                                               \
  ++(r);                                                                       \
  registers_.f() &= ~(kZFlagMask | kHFlagMask | kNFlagMask);                   \
  if ((r) == 0)                                                                \
    SET_ZFLAG()                                                                \
  if ((r)&0x0F)                                                                \
  CLEAR_HFLAG()

#define DEC_R(r)                                                               \
  --(r);                                                                       \
  registers_.f() &= ~(kZFlagMask | kHFlagMask);                                \
  registers_.f() |= (kNFlagMask);                                              \
  if ((r) == 0)                                                                \
    SET_ZFLAG()                                                                \
  if (((r)&0x0F) == 0x0F)                                                      \
  SET_HFLAG()

#define RLC(r)                                                                 \
  {                                                                            \
    registers_.f() = 0;                                                        \
    const bool carry = (r)&0x80;                                               \
    (r) = ((r) << 1) if (carry) {                                              \
      ++r;                                                                     \
      SET_CFLAG()                                                              \
    }                                                                          \
    if ((r) == 0)                                                              \
      SET_ZFLAG()                                                              \
  }

#define RRC(r)                                                                 \
  {                                                                            \
    registers_.f() = 0;                                                        \
    const bool carry = r & 0x1;                                                \
    (r) = (r) >> 1;                                                            \
    if (carry) {                                                               \
      SET_CFLAG()                                                              \
      (r) |= 0x80;                                                             \
    }                                                                          \
    if ((r) == 0)                                                              \
      SET_ZFLAG()                                                              \
  }

#define RL(r)                                                                  \
  {                                                                            \
    registers_.f() = 0;                                                        \
    const bool carry = registers_.f() & kCFlagMask;                            \
    const bool new_carry = (r)&0x80;                                           \
    (r) = ((r) << 1);                                                          \
    if (carry)                                                                 \
      ++r;                                                                     \
    if (new_carry)                                                             \
      SET_CFLAG()                                                              \
    if ((r) == 0)                                                              \
      SET_ZFLAG()                                                              \
  }

#define RR(r)                                                                  \
  {                                                                            \
    registers_.f() = 0;                                                        \
    const bool carry = registers_.f() & kCFlagMask;                            \
    const bool new_carry = (r)&0x1;                                            \
    (r) = (r) >> 1;                                                            \
    if (new_carry) {                                                           \
      SET_CFLAG();                                                             \
      r |= 0x80;                                                               \
    }                                                                          \
    if ((r) == 0)                                                              \
      SET_ZFLAG()                                                              \
  }

#define RRCA()                                                                 \
  {                                                                            \
    registers_.f() = 0;                                                        \
    const bool carry = registers_.a() & 0x1;                                   \
    registers_.a() = registers_.a() >> 1;                                      \
    if (carry) {                                                               \
      SET_CFLAG()                                                              \
      registers_.a() |= 0x80;                                                  \
    }                                                                          \
  }

#define RRA()                                                                  \
  {                                                                            \
    registers_.f() = 0;                                                        \
    const bool carry = registers_.f() & kCFlagMask;                            \
    const bool new_carry = registers_.a() & 0x1;                               \
    registers_.a() = registers_.a() >> 1;                                      \
    if (carry)                                                                 \
      registers_.a() |= 0x80;                                                  \
    if (new_carry)                                                             \
      SET_CFLAG()                                                              \
  }

#define RLCA()                                                                 \
  {                                                                            \
    registers_.f() = 0;                                                        \
    const bool carry = registers_.a() & 0x80;                                  \
    registers_.a() = registers_.a() << 1;                                      \
    if (carry) {                                                               \
      registers_.a() |= 0x01;                                                  \
      SET_CFLAG()                                                              \
    }                                                                          \
  }

#define RLA()                                                                  \
  {                                                                            \
    registers_.f() = 0;                                                        \
    const bool carry = registers_.f() & kCFlagMask;                            \
    const bool new_carry = registers_.a() & 0x80;                              \
    registers_.a() = (registers_.a() << 1);                                    \
    if (carry)                                                                 \
      registers_.a() |= 0x01;                                                  \
    if (new_carry)                                                             \
      SET_CFLAG()                                                              \
  }

#define LD_NN_SP()                                                             \
  {                                                                            \
    byte low, high;                                                    \
    READ_PC(low)                                                               \
    READ_PC(high)                                                              \
    word address = (high << 8) | low;                                 \
    WRITE(address, registers_.stack_pointer & 0xFF)                            \
    WRITE(address + 1, registers_.stack_pointer >> 8)                          \
  }

#define ADD_HL_RR(r)                                                           \
  {                                                                            \
    word hl = registers_.hl();                                        \
    timing_handler_.Advance(4);                                                \
    registers_.hl() += (r);                                                    \
    registers_.f() &= ~(kNFlagMask | kCFlagMask | kHFlagMask);                 \
    if (((hl & 0xFFF) + ((r)&0xFFF)) > 0xFFF)                                  \
      SET_HFLAG()                                                              \
    if (registers_.hl() < hl)                                                  \
      SET_CFLAG()                                                              \
  }

#define JR_N()                                                                 \
  {                                                                            \
    signed_byte value;                                                         \
    READ_PC(value);                                                            \
    registers_.program_counter += value;                                       \
    timing_handler_.Advance(4);                                                \
  }

#define JR_CC_N(condition)                                                     \
  {                                                                            \
    signed_byte value;                                                         \
    READ_PC(value)                                                             \
    if (condition)                                                             \
      registers_.program_counter += value;                                     \
  }

#define DAA()                                                                  \
  if (registers_.f() & kNFlagMask) {                                           \
    if (registers_.f() & kCFlagMask)                                           \
      registers_.a() -= 0x60;                                                  \
    if (registers_.f() & kHFlagMask)                                           \
      registers_.a() -= 0x06;                                                  \
  } else {                                                                     \
    if (registers_.f() & kCFlagMask || registers_.a() > 0x99) {                \
      registers_.a() += 0x60;                                                  \
      SET_CFLAG()                                                              \
    }                                                                          \
    if (registers_.f() & kHFlagMask || (registers_.a() & 0x0F) > 0x09)         \
      registers_.a() += 0x06;                                                  \
  }                                                                            \
  if (registers_.a() == 0)                                                     \
    SET_ZFLAG()                                                                \
  CLEAR_HFLAG()

#define CPL()                                                                  \
  registers_.a() = ~registers_.a();                                            \
  SET_NFLAG()                                                                  \
  SET_HFLAG()

#define INC_HL()                                                               \
  {                                                                            \
    byte value;                                                        \
    READ(registers_.hl(), value);                                              \
    ++value;                                                                   \
    WRITE(registers_.hl(), value);                                             \
    registers_.f() &= ~(kZFlagMask | kNFlagMask | kHFlagMask);                 \
    if (value == 0)                                                            \
      SET_ZFLAG()                                                              \
    if ((value & 0x0F) == 0)                                                   \
      SET_HFLAG()                                                              \
  }

#define DEC_HL()                                                               \
  {                                                                            \
    byte value;                                                        \
    READ(registers_.hl(), value);                                              \
    --value;                                                                   \
    WRITE(registers_.hl(), value);                                             \
    registers_.f() &= ~(kZFlagMask | kHFlagMask);                              \
    registers_.f() |= kNFlagMask;                                              \
    if (value == 0)                                                            \
      SET_ZFLAG()                                                              \
    if ((value & 0x0F) == 0x0F)                                                \
      SET_HFLAG()                                                              \
  }

#define LD_HL_N()                                                              \
  {                                                                            \
    byte value;                                                        \
    READ_PC(value);                                                            \
    WRITE(registers_.hl(), value);                                             \
  }

#define SCF() registers_.f() = (registers_.f() & 0xF8) | kCFlagMask;
#define CCF()                                                                  \
  registers_.f() =                                                             \
      (registers_.f() & 0xF8) | ((registers_.f() & kCFlagMask) ^ kCFlagMask);

#define HALT()                                                                 \
  if (registers_.interrupt_master_enable) {                                    \
    halt_ = true;                                                              \
    return;                                                                    \
  }                                                                            \
  if ((registers_.interrupt_enable & registers_.interrupt_flags) == 0)         \
    halt_ = true;                                                              \
  else                                                                         \
    halt_bug_ = true;

#define ADD_A(value)                                                           \
  {                                                                            \
    byte a = registers_.a();                                           \
    registers_.a() += (value);                                                 \
    registers_.f() &= 0xF0;                                                    \
    if (a + (value) == 0)                                                      \
      SET_ZFLAG()                                                              \
    if ((a & 0xF) + (value & 0xF) > 0xF)                                       \
      SET_HFLAG()                                                              \
    if (a + (value) > 0xFF)                                                    \
      SET_CFLAG()                                                              \
  }

#define ADD_A_HL()                                                             \
  {                                                                            \
    byte value;                                                        \
    READ(registers_.hl(), value);                                              \
    ADD_A(value);                                                              \
  }

#define ADC_A(value)                                                           \
  {                                                                            \
    byte a = registers_.a();                                           \
    const byte carry = ((registers_.f() & kCFlagMask) > 0) ? 1 : 0;         \
    registers_.a() += (value + carry);                                         \
    registers_.f() &= 0xF0;                                                    \
    if (a + (value) + carry == 0)                                              \
      SET_ZFLAG()                                                              \
    if ((a & 0xF) + (value & 0xF) + carry > 0xF)                               \
      SET_HFLAG()                                                              \
    if (a + (value) + carry > 0xFF)                                            \
      SET_CFLAG()                                                              \
  }

#define ADC_A_HL()                                                             \
  {                                                                            \
    byte value;                                                        \
    READ(registers_.hl(), value);                                              \
    ADC_A(value);                                                              \
  }

#define SUB_A(value)                                                           \
  {                                                                            \
    byte a = registers_.a();                                           \
    registers_.a() -= (value);                                                 \
    registers_.f() = (registers_.f() & 0xF0) | kNFlagMask;                     \
    if (a == value)                                                            \
      SET_ZFLAG()                                                              \
    if ((a & 0xF) < (value & 0xF))                                             \
      SET_HFLAG()                                                              \
    if (a < value)                                                             \
      SET_CFLAG()                                                              \
  }

#define SUB_A_HL()                                                             \
  {                                                                            \
    byte value;                                                        \
    READ(registers_.hl(), value);                                              \
    SUB_A(value);                                                              \
  }

#define SBC_A(value)                                                           \
  {                                                                            \
    byte a = registers_.a();                                           \
    const byte carry = ((registers_.f() & kCFlagMask) > 0) ? 1 : 0;         \
    registers_.a() = registers_.a() - (value)-carry;                           \
    registers_.f() = (registers_.f() & 0xF0) | kNFlagMask;                     \
    if (a - value == carry)                                                    \
      SET_ZFLAG()                                                              \
    if ((a & 0xF) < (value & 0xF) + carry)                                     \
      SET_HFLAG()                                                              \
    if (a < value + carry)                                                     \
      SET_CFLAG()                                                              \
  }

#define SBC_A_HL()                                                             \
  {                                                                            \
    byte value;                                                        \
    READ(registers_.hl(), value);                                              \
    SBC_A(value);                                                              \
  }

#define AND_A(value)                                                           \
  registers_.a() &= (value);                                                   \
  registers_.f() = kHFlagMask | (registers_.a() == 0 ? kZFlagMask : 0);

#define AND_A_HL()                                                             \
  {                                                                            \
    byte value;                                                        \
    READ(registers_.hl(), value);                                              \
    AND_A(value);                                                              \
  }

#define XOR_A(value)                                                           \
  registers_.a() ^= (value);                                                   \
  if (registers_.a() == 0)                                                     \
  SET_ZFLAG()

#define XOR_A_HL()                                                             \
  {                                                                            \
    byte value;                                                        \
    READ(registers_.hl(), value);                                              \
    XOR_A(value);                                                              \
  }

#define OR_A(value)                                                            \
  registers_.a() |= (value);                                                   \
  registers_.f() = (registers_.a() == 0 ? kZFlagMask : 0);

#define OR_A_HL()                                                              \
  {                                                                            \
    byte value;                                                        \
    READ(registers_.hl(), value);                                              \
    OR_A(value);                                                               \
  }

#define CP_A(value)                                                            \
  {                                                                            \
    byte a = registers_.a();                                           \
    registers_.f() = (registers_.f() & 0xF0) | kNFlagMask;                     \
    if (a == value)                                                            \
      SET_ZFLAG()                                                              \
    if ((a & 0xF) < (value & 0xF))                                             \
      SET_HFLAG()                                                              \
    if (a < value)                                                             \
      SET_CFLAG()                                                              \
  }

#define CP_A_HL()                                                              \
  {                                                                            \
    byte value;                                                        \
    READ(registers_.hl(), value);                                              \
    CP_A(value);                                                               \
  }

#define RET()                                                                  \
  {                                                                            \
    byte low, high;                                                    \
    READ_PC(low);                                                              \
    READ_PC(high);                                                             \
    registers_.program_counter = low | (high << 8);                            \
    timing_handler_.Advance(4);                                                \
  }

#define RET_CC(condition)                                                      \
  timing_handler_.Advance(4);                                                  \
  if (condition) {                                                             \
    RET();                                                                     \
  }

#define POP_RR(destination)                                                    \
  {                                                                            \
    byte low, high;                                                    \
    READ_SP(low);                                                              \
    READ_SP(high);                                                             \
    (destination) = low | (high << 8);                                         \
  }

#define JP_CC_NN(condition)                                                    \
  {                                                                            \
    byte low, high;                                                    \
    READ_PC(low);                                                              \
    READ_PC(high);                                                             \
    if (condition) {                                                           \
      timing_handler_.Advance(4);                                              \
      registers_.program_counter = low | (high << 8);                          \
    }                                                                          \
  }

#define JP_NN()                                                                \
  {                                                                            \
    byte low, high;                                                    \
    READ_PC(low);                                                              \
    READ_PC(high);                                                             \
    timing_handler_.Advance(4);                                                \
    registers_.program_counter = low | (high << 8);                            \
  }

#define PUSH_RR(rr)                                                            \
  timing_handler_.Advance(4);                                                  \
  WRITE_SP((rr) >> 8)                                                          \
  WRITE_SP((rr)&0xFF);

#define CALL(address)                                                          \
  PUSH_RR(registers_.program_counter);                                         \
  registers_.program_counter = address;

#define CALL_NN()                                                              \
  {                                                                            \
    byte low, high;                                                    \
    READ_PC(low);                                                              \
    READ_PC(high);                                                             \
    CALL(low | (high << 8));                                                   \
  }

#define CALL_CC_NN(condition)                                                  \
  {                                                                            \
    byte low, high;                                                    \
    READ_PC(low);                                                              \
    READ_PC(high);                                                             \
    if (condition) {                                                           \
      CALL(low | (high << 8));                                                 \
    }                                                                          \
  }

#define ADD_A_N()                                                              \
  {                                                                            \
    byte value;                                                        \
    READ_PC(value);                                                            \
    ADD_A(value);                                                              \
  }

#define RST(address)                                                           \
  PUSH_RR(registers_.program_counter);                                         \
  registers_.program_counter = (address);

#define ADC_A_N()                                                              \
  {                                                                            \
    byte value;                                                        \
    READ_PC(value);                                                            \
    ADC_A(value);                                                              \
  }

#define SUB_A_N()                                                              \
  {                                                                            \
    byte value;                                                        \
    READ_PC(value);                                                            \
    SUB_A(value);                                                              \
  }

#define RETI()                                                                 \
  registers_.interrupt_master_enable = true;                                   \
  RET();

#define SBC_A_N()                                                              \
  {                                                                            \
    byte value;                                                        \
    READ_PC(value);                                                            \
    SBC_A(value);                                                              \
  }

#define LDH_N_A()                                                              \
  {                                                                            \
    word value;                                                       \
    READ_PC(value);                                                            \
    WRITE(value + 0xFF00, registers_.a());                                     \
  }

#define LDH_C_A() WRITE(0xFF00 + registers_.c(), registers_.a());

#define LDH_A_C()                                                              \
  {                                                                            \
    byte value;                                                        \
    READ(registers_.c() + 0xFF00, value);                                      \
    registers_.a() = value;                                                    \
  }

#define AND_A_N()                                                              \
  {                                                                            \
    byte value;                                                        \
    READ_PC(value);                                                            \
    AND_A(value);                                                              \
  }

#define ADD_SP_N()                                                             \
  {                                                                            \
    signed_byte value;                                                         \
    READ_PC(value);                                                            \
    word sp = registers_.stack_pointer;                                    \
    registers_.stack_pointer += value;                                         \
    registers_.f() = 0;                                                        \
    if ((sp & 0xFF) + (value) > 0xFF)                                          \
      SET_CFLAG();                                                             \
    if ((sp & 0xF) + (value & 0xF) > 0xF)                                      \
      SET_HFLAG();                                                             \
    timing_handler_.Advance(8);                                                \
  }

#define LD_NN_A()                                                              \
  {                                                                            \
    byte low, high;                                                    \
    READ_PC(low);                                                              \
    READ_PC(high);                                                             \
    word address = low | (high << 8);                                 \
    READ(address, registers_.a());                                             \
  }

#define XOR_A_N()                                                              \
  {                                                                            \
    byte value;                                                        \
    READ_PC(value);                                                            \
    XOR_A(value);                                                              \
  }

#define LDH_A_N()                                                              \
  {                                                                            \
    word value;                                                       \
    READ_PC(value);                                                            \
    READ(0xFF00 + value, registers_.a());                                      \
  }

#define OR_A_N()                                                               \
  {                                                                            \
    byte value;                                                        \
    READ_PC(value);                                                            \
    OR_A(value);                                                               \
  }

#define LD_HL_SP_N()                                                           \
  {                                                                            \
    signed_byte value;                                                         \
    READ_PC(value);                                                            \
    registers_.hl() = registers_.stack_pointer + value;                        \
    registers_.f() = 0;                                                        \
    if ((registers_.stack_pointer & 0xFF) + (value) > 0xFF)                    \
      SET_CFLAG();                                                             \
    if ((registers_.stack_pointer & 0xF) + (value & 0xF) > 0xF)                \
      SET_HFLAG();                                                             \
  }

#define LD_A_NN()                                                              \
  {                                                                            \
    byte low, high;                                                    \
    READ_PC(low);                                                              \
    READ_PC(high);                                                             \
    word address = low | (high << 8);                                 \
    READ(address, registers_.a());                                             \
  }

#define CP_A_N()                                                               \
  {                                                                            \
    byte value;                                                        \
    READ_PC(value);                                                            \
    CP_A(value);                                                               \
  }

  CPU::CPU(TimingHandler& timing_handler, Memory& memory)
    : memory_(memory), timing_handler_(timing_handler), halt_(false),
    stop_(false), halt_bug_(false) {}

  CPU::~CPU() = default;

  void CPU::Run() {
    if (stop_) {
      return; // TODO
    }

    byte interrupt_queue =
      registers_.interrupt_enable & registers_.interrupt_flags & 0x1F;
    if (interrupt_queue) {
      halt_ = false;

      registers_.interrupt_master_enable = false;
    }

    if (!halt_) {
      READ_PC(opcode_);
      Execute(opcode_);
    }
  }

  void CPU::Execute(byte opcode) {
    switch (opcode) {
    case 0x00:
      break;
    case 0x01:
      LD_RR_NN(registers_.bc()) break;
    case 0x02:
      WRITE(registers_.bc(), registers_.a()) break;
    case 0x03:
      INC_RR(registers_.bc()) break;
    case 0x04:
      INC_R(registers_.b()) break;
    case 0x05:
      DEC_R(registers_.b()) break;
    case 0x06:
      READ_PC(registers_.b()) break;
    case 0x07:
      RLCA() break;
    case 0x08:
      LD_NN_SP() break;
    case 0x09:
      ADD_HL_RR(registers_.bc()) break;
    case 0x0A:
      READ(registers_.bc(), registers_.a()) break;
    case 0x0B:
      DEC_RR(registers_.bc()) break;
    case 0x0C:
      INC_R(registers_.c()) break;
    case 0x0D:
      DEC_R(registers_.c()) break;
    case 0x0E:
      READ_PC(registers_.c()) break;
    case 0x0F:
      RRCA() break;
    case 0x10:
      stop_ = true;
      break;
    case 0x11:
      LD_RR_NN(registers_.de()) break;
    case 0x12:
      WRITE(registers_.de(), registers_.a()) break;
    case 0x13:
      INC_RR(registers_.de()) break;
    case 0x14:
      INC_R(registers_.d()) break;
    case 0x15:
      DEC_R(registers_.d()) break;
    case 0x16:
      READ_PC(registers_.d()) break;
    case 0x17:
      RLA() break;
    case 0x18:
      JR_N() break;
    case 0x19:
      ADD_HL_RR(registers_.de()) break;
    case 0x1A:
      READ(registers_.de(), registers_.a()) break;
    case 0x1B:
      DEC_RR(registers_.de()) break;
    case 0x1C:
      INC_R(registers_.e()) break;
    case 0x1D:
      DEC_R(registers_.e()) break;
    case 0x1E:
      READ_PC(registers_.e()) break;
    case 0x1F:
      RRA() break;
    case 0x20:
      JR_CC_N((registers_.f() & kZFlagMask) == 0) break;
    case 0x21:
      LD_RR_NN(registers_.hl()) break;
    case 0x22:
      WRITE(registers_.hl()++, registers_.a()) break;
    case 0x23:
      INC_RR(registers_.hl()) break;
    case 0x24:
      INC_R(registers_.h()) break;
    case 0x25:
      DEC_R(registers_.h()) break;
    case 0x26:
      READ_PC(registers_.h()) break;
    case 0x27:
      DAA() break;
    case 0x28:
      JR_CC_N(registers_.f() & kZFlagMask) break;
    case 0x29:
      ADD_HL_RR(registers_.hl()) break;
    case 0x2A:
      READ(registers_.hl()++, registers_.a()) break;
    case 0x2B:
      DEC_RR(registers_.hl()) break;
    case 0x2C:
      INC_R(registers_.l()) break;
    case 0x2D:
      DEC_R(registers_.l()) break;
    case 0x2E:
      READ_PC(registers_.l()) break;
    case 0x2F:
      CPL() break;
    case 0x30:
      JR_CC_N((registers_.f() & kCFlagMask) == 0) break;
    case 0x31:
      LD_RR_NN(registers_.stack_pointer) break;
    case 0x32:
      WRITE(registers_.hl()--, registers_.a()) break;
    case 0x33:
      INC_RR(registers_.stack_pointer) break;
    case 0x34:
      INC_HL() break;
    case 0x35:
      DEC_HL() break;
    case 0x36:
      LD_HL_N() break;
    case 0x37:
      SCF() break;
    case 0x38:
      JR_CC_N(registers_.f() & kCFlagMask) break;
    case 0x39:
      ADD_HL_RR(registers_.stack_pointer) break;
    case 0x3A:
      READ(registers_.hl()--, registers_.a()) break;
    case 0x3B:
      DEC_RR(registers_.stack_pointer) break;
    case 0x3C:
      INC_R(registers_.a()) break;
    case 0x3D:
      DEC_R(registers_.a()) break;
    case 0x3E:
      READ_PC(registers_.a()) break;
    case 0x3F:
      CCF() break;
    case 0x40:
      break;
    case 0x41:
      registers_.b() = registers_.c();
      break;
    case 0x42:
      registers_.b() = registers_.d();
      break;
    case 0x43:
      registers_.b() = registers_.e();
      break;
    case 0x44:
      registers_.b() = registers_.h();
      break;
    case 0x45:
      registers_.b() = registers_.l();
      break;
    case 0x46:
      READ(registers_.hl(), registers_.b()) break;
    case 0x47:
      registers_.b() = registers_.a();
      break;
    case 0x48:
      registers_.c() = registers_.b();
      break;
    case 0x49:
      break;
    case 0x4A:
      registers_.c() = registers_.d();
      break;
    case 0x4B:
      registers_.c() = registers_.e();
      break;
    case 0x4C:
      registers_.c() = registers_.h();
      break;
    case 0x4D:
      registers_.c() = registers_.l();
      break;
    case 0x4E:
      READ(registers_.hl(), registers_.c()) break;
    case 0x4F:
      registers_.c() = registers_.a();
      break;
    case 0x50:
      registers_.d() = registers_.b();
      break;
    case 0x51:
      registers_.d() = registers_.c();
      break;
    case 0x52:
      break;
    case 0x53:
      registers_.d() = registers_.e();
      break;
    case 0x54:
      registers_.d() = registers_.h();
      break;
    case 0x55:
      registers_.d() = registers_.l();
      break;
    case 0x56:
      READ(registers_.hl(), registers_.d()) break;
    case 0x57:
      registers_.d() = registers_.a();
      break;
    case 0x58:
      registers_.e() = registers_.b();
      break;
    case 0x59:
      registers_.e() = registers_.c();
      break;
    case 0x5A:
      registers_.e() = registers_.d();
      break;
    case 0x5B:
      break;
    case 0x5C:
      registers_.e() = registers_.h();
      break;
    case 0x5D:
      registers_.e() = registers_.l();
      break;
    case 0x5E:
      READ(registers_.hl(), registers_.e()) break;
    case 0x5F:
      registers_.e() = registers_.a();
      break;
    case 0x60:
      registers_.h() = registers_.b();
      break;
    case 0x61:
      registers_.h() = registers_.c();
      break;
    case 0x62:
      registers_.h() = registers_.d();
      break;
    case 0x63:
      registers_.h() = registers_.e();
      break;
    case 0x64:
      break;
    case 0x65:
      registers_.h() = registers_.l();
      break;
    case 0x66:
      READ(registers_.hl(), registers_.h()) break;
    case 0x67:
      registers_.h() = registers_.a();
      break;
    case 0x68:
      registers_.l() = registers_.b();
      break;
    case 0x69:
      registers_.l() = registers_.c();
      break;
    case 0x6A:
      registers_.l() = registers_.d();
      break;
    case 0x6B:
      registers_.l() = registers_.e();
      break;
    case 0x6C:
      registers_.l() = registers_.h();
      break;
    case 0x6D:
      break;
    case 0x6E:
      READ(registers_.hl(), registers_.l()) break;
    case 0x6F:
      registers_.l() = registers_.a();
      break;
    case 0x70:
      WRITE(registers_.hl(), registers_.b()) break;
    case 0x71:
      WRITE(registers_.hl(), registers_.c()) break;
    case 0x72:
      WRITE(registers_.hl(), registers_.d()) break;
    case 0x73:
      WRITE(registers_.hl(), registers_.e()) break;
    case 0x74:
      WRITE(registers_.hl(), registers_.h()) break;
    case 0x75:
      WRITE(registers_.hl(), registers_.l()) break;
    case 0x76:
      HALT() break;
    case 0x77:
      WRITE(registers_.hl(), registers_.a()) break;
    case 0x78:
      registers_.a() = registers_.b();
      break;
    case 0x79:
      registers_.a() = registers_.c();
      break;
    case 0x7A:
      registers_.a() = registers_.d();
      break;
    case 0x7B:
      registers_.a() = registers_.e();
      break;
    case 0x7C:
      registers_.a() = registers_.h();
      break;
    case 0x7D:
      registers_.a() = registers_.l();
      break;
    case 0x7E:
      READ(registers_.hl(), registers_.a()) break;
    case 0x7F:
      break;
    case 0x80:
      ADD_A(registers_.b()) break;
    case 0x81:
      ADD_A(registers_.c()) break;
    case 0x82:
      ADD_A(registers_.d()) break;
    case 0x83:
      ADD_A(registers_.e()) break;
    case 0x84:
      ADD_A(registers_.h()) break;
    case 0x85:
      ADD_A(registers_.l()) break;
    case 0x86:
      ADD_A_HL() break;
    case 0x87:
      ADD_A(registers_.a()) break;
    case 0x88:
      ADC_A(registers_.b()) break;
    case 0x89:
      ADC_A(registers_.c()) break;
    case 0x8A:
      ADC_A(registers_.d()) break;
    case 0x8B:
      ADC_A(registers_.e()) break;
    case 0x8C:
      ADC_A(registers_.h()) break;
    case 0x8D:
      ADC_A(registers_.l()) break;
    case 0x8E:
      ADC_A_HL() break;
    case 0x8F:
      ADC_A(registers_.a()) break;
    case 0x90:
      SUB_A(registers_.b()) break;
    case 0x91:
      SUB_A(registers_.c()) break;
    case 0x92:
      SUB_A(registers_.d()) break;
    case 0x93:
      SUB_A(registers_.e()) break;
    case 0x94:
      SUB_A(registers_.h()) break;
    case 0x95:
      SUB_A(registers_.l()) break;
    case 0x96:
      SUB_A_HL() break;
    case 0x97:
      SUB_A(registers_.a()) break;
    case 0x98:
      SBC_A(registers_.b()) break;
    case 0x99:
      SBC_A(registers_.c()) break;
    case 0x9A:
      SBC_A(registers_.d()) break;
    case 0x9B:
      SBC_A(registers_.e()) break;
    case 0x9C:
      SBC_A(registers_.h()) break;
    case 0x9D:
      SBC_A(registers_.l()) break;
    case 0x9E:
      SBC_A_HL() break;
    case 0x9F:
      SBC_A(registers_.a()) break;
    case 0xA0:
      AND_A(registers_.b()) break;
    case 0xA1:
      AND_A(registers_.c()) break;
    case 0xA2:
      AND_A(registers_.d()) break;
    case 0xA3:
      AND_A(registers_.e()) break;
    case 0xA4:
      AND_A(registers_.h()) break;
    case 0xA5:
      AND_A(registers_.l()) break;
    case 0xA6:
      AND_A_HL() break;
    case 0xA7:
      registers_.f() = kHFlagMask | (registers_.a() == 0 ? kZFlagMask : 0);
      break;
    case 0xA8:
      XOR_A(registers_.b()) break;
    case 0xA9:
      XOR_A(registers_.c()) break;
    case 0xAA:
      XOR_A(registers_.d()) break;
    case 0xAB:
      XOR_A(registers_.e()) break;
    case 0xAC:
      XOR_A(registers_.h()) break;
    case 0xAD:
      XOR_A(registers_.l()) break;
    case 0xAE:
      XOR_A_HL() break;
    case 0xAF:
      registers_.af() = kZFlagMask;
      break; // XOR A, A
    case 0xB0:
      OR_A(registers_.b()) break;
    case 0xB1:
      OR_A(registers_.c()) break;
    case 0xB2:
      OR_A(registers_.d()) break;
    case 0xB3:
      OR_A(registers_.e()) break;
    case 0xB4:
      OR_A(registers_.h()) break;
    case 0xB5:
      OR_A(registers_.l()) break;
    case 0xB6:
      OR_A_HL() break;
    case 0xB7:
      registers_.f() = (registers_.a() == 0 ? kZFlagMask : 0);
      break;
    case 0xB8:
      CP_A(registers_.b()) break;
    case 0xB9:
      CP_A(registers_.c()) break;
    case 0xBA:
      CP_A(registers_.d()) break;
    case 0xBB:
      CP_A(registers_.e()) break;
    case 0xBC:
      CP_A(registers_.h()) break;
    case 0xBD:
      CP_A(registers_.l()) break;
    case 0xBE:
      CP_A_HL() break;
    case 0xBF:
      registers_.f() = kZFlagMask | kNFlagMask;
      break;
    case 0xC0:
      RET_CC((registers_.f() & kZFlagMask) == 0) break;
    case 0xC1:
      POP_RR(registers_.bc()) break;
    case 0xC2:
      JP_CC_NN((registers_.f() & kZFlagMask) == 0) break;
    case 0xC3:
      JP_NN() break;
    case 0xC4:
      CALL_CC_NN((registers_.f() & kZFlagMask) == 0) break;
    case 0xC5:
      PUSH_RR(registers_.bc()) break;
    case 0xC6:
      ADD_A_N() break;
    case 0xC7:
      RST(0x00) break;
    case 0xC8:
      RET_CC(registers_.f() & kZFlagMask) break;
    case 0xC9:
      RET() break;
    case 0xCA:
      JP_CC_NN(registers_.f() & kZFlagMask) break;
    case 0xCB:
      READ_PC(opcode_)

        switch (opcode_) {
        case 0:
          break;
          // TODO
        default:
          throw std::runtime_error("Unknown opcode: " + std::to_string(opcode_));
          break;
        }
      break;
    case 0xCC:
      CALL_CC_NN(registers_.f() & kZFlagMask) break;
    case 0xCD:
      CALL_NN() break;
    case 0xCE:
      ADC_A_N() break;
    case 0xCF:
      RST(0x08) break;
    case 0xD0:
      RET_CC((registers_.f() & kCFlagMask) == 0) break;
    case 0xD1:
      POP_RR(registers_.de()) break;
    case 0xD2:
      JP_CC_NN((registers_.f() & kCFlagMask) == 0) break;
    case 0xD4:
      CALL_CC_NN((registers_.f() & kCFlagMask) == 0) break;
    case 0xD5:
      PUSH_RR(registers_.de()) break;
    case 0xD6:
      SUB_A_N() break;
    case 0xD7:
      RST(0x10) break;
    case 0xD8:
      RET_CC((registers_.f() & kCFlagMask) != 0) break;
    case 0xD9:
      RETI() break;
    case 0xDA:
      JP_CC_NN((registers_.f() & kCFlagMask) != 0) break;
    case 0xDC:
      CALL_CC_NN((registers_.f() & kCFlagMask) != 0) break;
    case 0xDE:
      SBC_A_N() break;
    case 0xDF:
      RST(0x18) break;
    case 0xE0:
      LDH_N_A() break;
    case 0xE1:
      POP_RR(registers_.hl()) break;
    case 0xE2:
      LDH_C_A() break;
    case 0xE5:
      PUSH_RR(registers_.hl()) break;
    case 0xE6:
      AND_A_N() break;
    case 0xE7:
      RST(0x20) break;
    case 0xE8:
      ADD_SP_N() break;
    case 0xE9:
      registers_.program_counter = registers_.hl();
      break;
    case 0xEA:
      LD_NN_A() break;
    case 0xEE:
      XOR_A_N() break;
    case 0xEF:
      RST(0x28) break;
    case 0xF0:
      LDH_A_N() break;
    case 0xF1:
      POP_RR(registers_.af()) break;
    case 0xF2:
      LDH_A_C() break;
    case 0xF3:
      registers_.interrupt_master_enable = false;
      ei_executed_ = false;
      break;
    case 0xF5:
      PUSH_RR(registers_.af()) break;
    case 0xF6:
      OR_A_N() break;
    case 0xF7:
      RST(0x30) break;
    case 0xF8:
      LD_HL_SP_N() break;
    case 0xF9:
      registers_.stack_pointer = registers_.hl();
      break;
    case 0xFA:
      LD_A_NN() break;
    case 0xFB:
      ei_executed_ = true;
      break;
    case 0xFE:
      CP_A_N() break;
    case 0xFF:
      RST(0x38) break;
    default:
      throw std::runtime_error("Unknown opcode: " + std::to_string(opcode_));
      break;
    }
  }

  void CPU::CheckInterrupts() {
    if (registers_.interrupt_flags & registers_.interrupt_enable) {
      halt_ = false;
    }
  }
} // namespace gandalf