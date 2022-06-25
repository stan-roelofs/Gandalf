#ifndef __MEMORY_H
#define __MEMORY_H

#include <stdint.h>
#include <array>

const uint16_t VBK = 0xFF4F;
const uint16_t LCDC = 0xFF40;
const uint16_t LY = 0xFF44;
const uint16_t LYC = 0xFF45;
const uint16_t STAT = 0xFF41;
const uint16_t SCY = 0xFF42;
const uint16_t SCX = 0xFF43;
const uint16_t WY = 0xFF4A;
const uint16_t WX = 0xFF4B;
const uint16_t BGP = 0xFF47;
const uint16_t OBP0 = 0xFF48;
const uint16_t OBP1 = 0xFF49;
const uint16_t BCPS = 0xFF68;
const uint16_t BCPD = 0xFF69;
const uint16_t OCPS = 0xFF6A;
const uint16_t OCPD = 0xFF6B;

// Interrupts
const uint16_t IF = 0xFF0F;
const uint16_t IE = 0xFFFF;

// Timer
const uint16_t DIV = 0xFF04;
const uint16_t TIMA = 0xFF05;
const uint16_t TMA = 0xFF06;
const uint16_t TAC = 0xFF07;

// Joypad
const uint16_t P1 = 0xFF00;

// Serial
const uint16_t SB = 0xFF01;
const uint16_t SC = 0xFF02;

// DMA
const uint16_t DMA = 0xFF46;
const uint16_t HDMA1 = 0xFF51;
const uint16_t HDMA2 = 0xFF52;
const uint16_t HDMA3 = 0xFF53;
const uint16_t HDMA4 = 0xFF54;
const uint16_t HDMA5 = 0xFF55;

// Sound
const uint16_t NR10 = 0xFF10;
const uint16_t NR11 = 0xFF11;
const uint16_t NR12 = 0xFF12;
const uint16_t NR13 = 0xFF13;
const uint16_t NR14 = 0xFF14;

const uint16_t NR21 = 0xFF16;
const uint16_t NR22 = 0xFF17;
const uint16_t NR23 = 0xFF18;
const uint16_t NR24 = 0xFF19;

const uint16_t NR30 = 0xFF1A;
const uint16_t NR31 = 0xFF1B;
const uint16_t NR32 = 0xFF1C;
const uint16_t NR33 = 0xFF1D;
const uint16_t NR34 = 0xFF1E;

const uint16_t NR41 = 0xFF20;
const uint16_t NR42 = 0xFF21;
const uint16_t NR43 = 0xFF22;
const uint16_t NR44 = 0xFF23;

const uint16_t NR50 = 0xFF24;
const uint16_t NR51 = 0xFF25;
const uint16_t NR52 = 0xFF26;

// CGB
const uint16_t KEY1 = 0xFF4D;
const uint16_t RP = 0xFF56;
const uint16_t OPRI = 0xFF6C;
const uint16_t SVBK = 0xFF70;

class Memory
{
public:
    Memory();
    ~Memory();

    uint8_t& operator[] (uint16_t address);

private:
    std::array<uint8_t, 0xFFFF> _memory; // TODO should we allocate this?
};

#endif
