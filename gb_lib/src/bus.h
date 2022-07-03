#ifndef __GANDALF_MEMORY_H
#define __GANDALF_MEMORY_H

#include <array>

#include <gandalf/types.h>
#include <gandalf/address_range.h>

namespace gandalf {

  const word VBK = 0xFF4F;
  const word LCDC = 0xFF40;
  const word LY = 0xFF44;
  const word LYC = 0xFF45;
  const word STAT = 0xFF41;
  const word SCY = 0xFF42;
  const word SCX = 0xFF43;
  const word WY = 0xFF4A;
  const word WX = 0xFF4B;
  const word BGP = 0xFF47;
  const word OBP0 = 0xFF48;
  const word OBP1 = 0xFF49;
  const word BCPS = 0xFF68;
  const word BCPD = 0xFF69;
  const word OCPS = 0xFF6A;
  const word OCPD = 0xFF6B;

  // Interrupts
  const word IF = 0xFF0F;
  const word IE = 0xFFFF;

  // Timer
  const word DIV = 0xFF04;
  const word TIMA = 0xFF05;
  const word TMA = 0xFF06;
  const word TAC = 0xFF07;

  // Joypad
  const word P1 = 0xFF00;

  // Serial
  const word SB = 0xFF01;
  const word SC = 0xFF02;

  // DMA
  const word DMA = 0xFF46;
  const word HDMA1 = 0xFF51;
  const word HDMA2 = 0xFF52;
  const word HDMA3 = 0xFF53;
  const word HDMA4 = 0xFF54;
  const word HDMA5 = 0xFF55;

  // Sound
  const word NR10 = 0xFF10;
  const word NR11 = 0xFF11;
  const word NR12 = 0xFF12;
  const word NR13 = 0xFF13;
  const word NR14 = 0xFF14;

  const word NR21 = 0xFF16;
  const word NR22 = 0xFF17;
  const word NR23 = 0xFF18;
  const word NR24 = 0xFF19;

  const word NR30 = 0xFF1A;
  const word NR31 = 0xFF1B;
  const word NR32 = 0xFF1C;
  const word NR33 = 0xFF1D;
  const word NR34 = 0xFF1E;

  const word NR41 = 0xFF20;
  const word NR42 = 0xFF21;
  const word NR43 = 0xFF22;
  const word NR44 = 0xFF23;

  const word NR50 = 0xFF24;
  const word NR51 = 0xFF25;
  const word NR52 = 0xFF26;

  // CGB
  const word KEY1 = 0xFF4D;
  const word RP = 0xFF56;
  const word OPRI = 0xFF6C;
  const word SVBK = 0xFF70;

  /**
   * Gives access to the Game Boy's 16-bit address space.
   */
  class Bus {
  public:
    Bus();
    ~Bus();

    /**
     * Writes the value to the specified address.
     *
     * @param address address that will be written
     * @param value value that will be written
     */
    void Write(word address, byte value);

    /**
     * Reads the value at the specified address.
     *
     * @param address the address that will be read.
     * @return Value
     */
    byte Read(word address) const;

    /**
     * Attaches a handler for a certain memory range to the bus.
     * @param handler the memory range to attach
     */
    void Attach(AddressRange* handler);

  private:
    std::array<AddressRange*, 0xFFFF> address_space_;
  };

} // namespace gandalf
#endif
