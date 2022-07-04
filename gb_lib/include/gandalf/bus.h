#ifndef __GANDALF_MEMORY_H
#define __GANDALF_MEMORY_H

#include <array>
#include <string>
#include <memory>
#include <stdexcept>
#include <set>

#include "types.h"

namespace gandalf {
  const word kVBK = 0xFF4F;
  const word kLCDC = 0xFF40;
  const word kLY = 0xFF44;
  const word kLYC = 0xFF45;
  const word kSTAT = 0xFF41;
  const word kSCY = 0xFF42;
  const word kSCX = 0xFF43;
  const word kWY = 0xFF4A;
  const word kWX = 0xFF4B;
  const word kBGP = 0xFF47;
  const word kOBP0 = 0xFF48;
  const word kOBP1 = 0xFF49;
  const word kBCPS = 0xFF68;
  const word kBCPD = 0xFF69;
  const word kOCPS = 0xFF6A;
  const word kOCPD = 0xFF6B;

  // Interrupts
  const word kIF = 0xFF0F;
  const word kIE = 0xFFFF;

  // Timer
  const word kDIV = 0xFF04;
  const word kTIMA = 0xFF05;
  const word kTMA = 0xFF06;
  const word kTAC = 0xFF07;

  // Joypad
  const word kP1 = 0xFF00;

  // Serial
  const word kSB = 0xFF01;
  const word kSC = 0xFF02;

  // DMA
  const word kDMA = 0xFF46;
  const word kHDMA1 = 0xFF51;
  const word kHDMA2 = 0xFF52;
  const word kHDMA3 = 0xFF53;
  const word kHDMA4 = 0xFF54;
  const word kHDMA5 = 0xFF55;

  // Sound
  const word kNR10 = 0xFF10;
  const word kNR11 = 0xFF11;
  const word kNR12 = 0xFF12;
  const word kNR13 = 0xFF13;
  const word kNR14 = 0xFF14;

  const word kNR21 = 0xFF16;
  const word kNR22 = 0xFF17;
  const word kNR23 = 0xFF18;
  const word kNR24 = 0xFF19;

  const word kNR30 = 0xFF1A;
  const word kNR31 = 0xFF1B;
  const word kNR32 = 0xFF1C;
  const word kNR33 = 0xFF1D;
  const word kNR34 = 0xFF1E;

  const word kNR41 = 0xFF20;
  const word kNR42 = 0xFF21;
  const word kNR43 = 0xFF22;
  const word kNR44 = 0xFF23;

  const word kNR50 = 0xFF24;
  const word kNR51 = 0xFF25;
  const word kNR52 = 0xFF26;

  const word kBANK = 0xFF50;

  // CGB
  const word kKEY1 = 0xFF4D;
  const word kRP = 0xFF56;
  const word kOPRI = 0xFF6C;
  const word kSVBK = 0xFF70;

  /**
   * Gives access to the Game Boy's 16-bit address space.
   */
  class Bus {
  public:
    class AddressHandler
    {
    public:
      /**
       * Writes the value to the specified address.
       *
       * @param address address that will be written
       * @param value value that will be written
       */
      virtual void Write(word address, byte value) = 0;

      /**
       * Reads the value at the specified address.
       *
       * @param address the address that will be read.
       * @return Value
       */
      virtual byte Read(word address) const = 0;

      /// @return The addresses that are managed by this object.
      virtual std::set<word> GetAddresses() const = 0;

    protected:
      AddressHandler(const std::string& name);
      virtual ~AddressHandler();
      std::string name_;
    };

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
     * Registers an address handler to the bus.
     * @param handler the handler
     */
    void Register(AddressHandler* handler);

    /**
     * Removes the specified address handler from the bus.
     *
     * @param handler the handler
    */
    void Unregister(AddressHandler* handler);

  private:
    std::array<AddressHandler*, 0x10000> address_space_;
  };

} // namespace gandalf
#endif
