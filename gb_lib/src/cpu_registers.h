#ifndef __CPU_REGISTERS_H
#define __CPU_REGISTERS_H

#include <cstdint>

const std::uint8_t kZFlagMask = 0x80;
const std::uint8_t kNFlagMask = 0x40;
const std::uint8_t kHFlagMask = 0x20;
const std::uint8_t kCFlagMask = 0x10;

struct Registers
{
    struct
    {
        union
        {
            struct
            {
                std::uint8_t f;
                std::uint8_t a;
            };
            std::uint16_t af;
        };
    };
    struct
    {
        union
        {
            struct
            {
                std::uint8_t c;
                std::uint8_t b;
            };
            std::uint16_t bc;
        };
    };
    struct
    {
        union
        {
            struct
            {
                std::uint8_t e;
                std::uint8_t d;
            };
            std::uint16_t de;
        };
    };
    struct
    {
        union
        {
            struct
            {
                std::uint8_t l;
                std::uint8_t h;
            };
            std::uint16_t hl;
        };
    };

    std::uint16_t stack_pointer;
    std::uint16_t program_counter;
    std::uint8_t interrupt_enable;
    std::uint8_t interrupt_flags;
    bool interrupt_master_enable;
};

#endif
