#ifndef __GANDALF_CPU_REGISTERS_H
#define __GANDALF_CPU_REGISTERS_H

#include  "types.h"

namespace gandalf {
  class Registers {
  public:
    Registers():
      af_combined(0),
      bc_combined(0),
      de_combined(0),
      hl_combined(0),
      stack_pointer(0),
      program_counter(0),
      interrupt_enable(0),
      interrupt_flags(0),
      interrupt_master_enable(false)
    {
    }
    ~Registers() = default;

    union {
      byte af_bytes[2];
      word af_combined;
    };
    byte& a() { return af_bytes[1]; }
    const byte& a() const { return af_bytes[1]; }
    byte& f() { return af_bytes[0]; }
    const byte& f() const { return af_bytes[0]; }
    word& af() { return af_combined; }
    const word& af() const { return af_combined; }

    union {
      byte bc_bytes[2];
      word bc_combined;
    };
    byte& b() { return bc_bytes[1]; }
    const byte& b() const { return bc_bytes[1]; }
    byte& c() { return bc_bytes[0]; }
    const byte& c() const { return bc_bytes[0]; }
    word& bc() { return bc_combined; }
    const word& bc() const { return bc_combined; }

    union {
      byte de_bytes[2];
      word de_combined;
    };
    byte& d() { return de_bytes[1]; }
    const byte& d() const { return de_bytes[1]; }
    byte& e() { return de_bytes[0]; }
    const byte& e() const { return de_bytes[0]; }
    word& de() { return de_combined; }
    const word& de() const { return de_combined; }

    union {
      byte hl_bytes[2];
      word hl_combined;
    };
    byte& h() { return hl_bytes[1]; }
    const byte& h() const { return hl_bytes[1]; }
    byte& l() { return hl_bytes[0]; }
    const byte& l() const { return hl_bytes[0]; }
    word& hl() { return hl_combined; }
    const word& hl() const { return hl_combined; }

    word stack_pointer;
    word program_counter;
    byte interrupt_enable;
    byte interrupt_flags;
    bool interrupt_master_enable;
  };
} // namespace gandalf
#endif
