#ifndef __CPU_H
#define __CPU_H

#include <stdint.h>

class CPU
{
public:
    struct Registers
    {
        struct
        {
            union
            {
                struct
                {
                    uint8_t F;
                    uint8_t A;
                };
                uint16_t AF;
            };
        };
        struct
        {
            union
            {
                struct
                {
                    uint8_t C;
                    uint8_t B;
                };
                uint16_t BC;
            };
        };
        struct
        {
            union
            {
                struct
                {
                    uint8_t E;
                    uint8_t D;
                };
                uint16_t DE;
            };
        };
        struct
        {
            union
            {
                struct
                {
                    uint8_t L;
                    uint8_t H;
                };
                uint16_t HL;
            };
        };

        uint16_t SP;
        uint16_t PC;
    };

    CPU();
    ~CPU();

    void Step();

private:
    Registers registers;
};

#endif
