#ifndef __CPU_REGISTERS_H
#define __CPU_REGISTERS_H

#include <cstdint>

const std::uint8_t kZFlagMask = 0x80;
const std::uint8_t kNFlagMask = 0x40;
const std::uint8_t kHFlagMask = 0x20;
const std::uint8_t kCFlagMask = 0x10;

namespace gandalf {
class Registers
{    
public:
    union {
        std::uint8_t af_bytes[2];
        std::uint16_t af_combined;
    };
    std::uint8_t& a() { return af_bytes[1]; }
    std::uint8_t& f() { return af_bytes[0]; }
    std::uint16_t& af() { return af_combined; }

    union {
        std::uint8_t bc_bytes[2];
        std::uint16_t bc_combined;
    };
    std::uint8_t& b() { return bc_bytes[1]; }
    std::uint8_t& c() { return bc_bytes[0]; }
    std::uint16_t& bc() { return bc_combined; }

    union {
        std::uint8_t de_bytes[2];
        std::uint16_t de_combined;
    };
    std::uint8_t& d() { return de_bytes[1]; }
    std::uint8_t& e() { return de_bytes[0]; }
    std::uint16_t& de() { return de_combined; }

    union {
        std::uint8_t hl_bytes[2];
        std::uint16_t hl_combined;
    };
    std::uint8_t& h() { return hl_bytes[1]; }
    std::uint8_t& l() { return hl_bytes[0]; }
    std::uint16_t& hl() { return hl_combined; }

    std::uint16_t stack_pointer;
    std::uint16_t program_counter;
    std::uint8_t interrupt_enable;
    std::uint8_t interrupt_flags;
    bool interrupt_master_enable;
};
} // namespace gandalf
#endif
