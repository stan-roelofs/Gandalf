#ifndef __GANDALF_MEMORY_H
#define __GANDALF_MEMORY_H

#include <array>
#include <string>
#include <memory>
#include <stdexcept>
#include <set>

#include "types.h"

namespace gandalf {
  /**
   * Gives access to the Game Boy's 16-bit address space.
   */
  class Memory {
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

      /// @returns The name of this handler.
      std::string GetName() const { return name_; }

    protected:
      AddressHandler(const std::string& name);
      virtual ~AddressHandler();
      std::string name_;
    };

    enum Bus: byte
    {
      kExternal,
      kVideoRAM,
    };

    Memory();
    ~Memory();

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
     * @return byte The value of the given address. If no access is granted this returns 0xFF.
     */
    byte Read(word address) const;

    /**
     * Reads the value at the specified address without checking for access.
     *
     * @param address the address that will be read.
     * @return byte The value of the given address.
     */
    byte DebugRead(word address) const;

    /**
     * Registers an address handler to the memory.
     * @param handler the handler
     */
    void Register(AddressHandler& handler);

    /**
     * Removes the specified address handler from the memory.
     *
     * @param handler the handler
    */
    void Unregister(AddressHandler& handler);

    /**
     * @param address Address for which the name is requested.
     * @returns The name of the object that owns the specified address.
     */
    std::string GetAddressHandlerName(word address) const;

	/**
	 * @param address Address for which the bus is requested.
	 * @returns Which Bus handles reads/writes of the given address.
	 */
    static Bus GetBus(word address);

    void Block(Bus bus, bool block = true);

  private:
    struct AddressWrapper
    {
      AddressHandler* handler;
      bool blocked;
    };
    std::array<AddressWrapper, 0x10000> address_space_;
  };

} // namespace gandalf
#endif
