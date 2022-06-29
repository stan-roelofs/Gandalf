#ifndef __MEMORY_H
#define __MEMORY_H

#include <cstdint>
#include <array>

namespace gandalf {

const std::uint16_t VBK = 0xFF4F;
const std::uint16_t LCDC = 0xFF40;
const std::uint16_t LY = 0xFF44;
const std::uint16_t LYC = 0xFF45;
const std::uint16_t STAT = 0xFF41;
const std::uint16_t SCY = 0xFF42;
const std::uint16_t SCX = 0xFF43;
const std::uint16_t WY = 0xFF4A;
const std::uint16_t WX = 0xFF4B;
const std::uint16_t BGP = 0xFF47;
const std::uint16_t OBP0 = 0xFF48;
const std::uint16_t OBP1 = 0xFF49;
const std::uint16_t BCPS = 0xFF68;
const std::uint16_t BCPD = 0xFF69;
const std::uint16_t OCPS = 0xFF6A;
const std::uint16_t OCPD = 0xFF6B;

// Interrupts
const std::uint16_t IF = 0xFF0F;
const std::uint16_t IE = 0xFFFF;

// Timer
const std::uint16_t DIV = 0xFF04;
const std::uint16_t TIMA = 0xFF05;
const std::uint16_t TMA = 0xFF06;
const std::uint16_t TAC = 0xFF07;

// Joypad
const std::uint16_t P1 = 0xFF00;

// Serial
const std::uint16_t SB = 0xFF01;
const std::uint16_t SC = 0xFF02;

// DMA
const std::uint16_t DMA = 0xFF46;
const std::uint16_t HDMA1 = 0xFF51;
const std::uint16_t HDMA2 = 0xFF52;
const std::uint16_t HDMA3 = 0xFF53;
const std::uint16_t HDMA4 = 0xFF54;
const std::uint16_t HDMA5 = 0xFF55;

// Sound
const std::uint16_t NR10 = 0xFF10;
const std::uint16_t NR11 = 0xFF11;
const std::uint16_t NR12 = 0xFF12;
const std::uint16_t NR13 = 0xFF13;
const std::uint16_t NR14 = 0xFF14;

const std::uint16_t NR21 = 0xFF16;
const std::uint16_t NR22 = 0xFF17;
const std::uint16_t NR23 = 0xFF18;
const std::uint16_t NR24 = 0xFF19;

const std::uint16_t NR30 = 0xFF1A;
const std::uint16_t NR31 = 0xFF1B;
const std::uint16_t NR32 = 0xFF1C;
const std::uint16_t NR33 = 0xFF1D;
const std::uint16_t NR34 = 0xFF1E;

const std::uint16_t NR41 = 0xFF20;
const std::uint16_t NR42 = 0xFF21;
const std::uint16_t NR43 = 0xFF22;
const std::uint16_t NR44 = 0xFF23;

const std::uint16_t NR50 = 0xFF24;
const std::uint16_t NR51 = 0xFF25;
const std::uint16_t NR52 = 0xFF26;

// CGB
const std::uint16_t KEY1 = 0xFF4D;
const std::uint16_t RP = 0xFF56;
const std::uint16_t OPRI = 0xFF6C;
const std::uint16_t SVBK = 0xFF70;

/**
 * Gives access to the Game Boy's 16-bit address space.
 */
class Memory
{
public:
    Memory();
    ~Memory();

    /**
     * Writes the value to the specified address.
     *
     * @param address address that will be written
     * @param value value that will be written
     */
    void Write(std::uint16_t address, std::uint8_t value);

    /**
     * Reads the value at the specified address.
     *
     * @param address the address that will be read.
     * @return Value
     */
    std::uint8_t Read(std::uint16_t address) const;

private:
    std::array<std::uint8_t, 0xFFFF> memory_;
};

} // namespace gandalf
#endif
