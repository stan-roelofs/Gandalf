#ifndef __CPU_H
#define __CPU_H

#include <stdint.h>

class CPU
{
    CPU();
    ~CPU();

private:
    struct Registers
    {
        struct
        {
            union
            {
                struct
                {
                    int8_t F;
                    int8_t A;
                };
                int16_t AF;
            };
        };
        struct
        {
            union
            {
                struct
                {
                    int8_t C;
                    int8_t B;
                };
                int16_t BC;
            };
        };
        struct
        {
            union
            {
                struct
                {
                    int8_t E;
                    int8_t D;
                };
                int16_t DE;
            };
        };
        struct
        {
            union
            {
                struct
                {
                    int8_t L;
                    int8_t H;
                };
                int16_t HL;
            };
        };
    };

    Registers registers;
};

#endif
