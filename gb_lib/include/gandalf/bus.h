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

      /// @returns The name of this handler.
      std::string GetName() const { return name_; }

    protected:
      AddressHandler(const std::string& name);
      virtual ~AddressHandler();
      std::string name_;
    };

    enum AccessLevel: byte
    {
      kNormal,
      kOEMDMA,
      kDebug // Always has access
    };

    Bus();
    ~Bus();

    /**
     * Writes the value to the specified address.
     *
     * @param address address that will be written
     * @param value value that will be written
     * @param access_level In some cases a memory region may be blocked for certain components, this parameter determines whether the callee gets access or not.
     */
    void Write(word address, byte value, AccessLevel access_level = kNormal);

    /**
     * Reads the value at the specified address.
     *
     * @param address the address that will be read.
     * @param access_level In some cases a memory region may be blocked for certain components, this parameter determines whether the callee gets access or not.
     * @return byte The value of the given address. If no access is granted this returns 0xFF.
     */
    byte Read(word address, AccessLevel access_level = kNormal) const;

    /**
     * Registers an address handler to the bus.
     * @param handler the handler
     */
    void Register(AddressHandler& handler);

    /**
     * Removes the specified address handler from the bus.
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
     * Sets the access level of a memory region. This can be used to block reads/writes to a region, for example when DMA is active the CPU can only access HRAM.
     * @param level The required level to be able to read/write to the specified region
     * @param start_address The first address that should be blocked
     * @param end_address The last address that should be blocked
    */
    void SetAccessLevel(AccessLevel level, word start_address, word end_address);

  private:
    struct AddressWrapper
    {
      AddressHandler* handler;
      AccessLevel access_level;
    };
    std::array<AddressWrapper, 0x10000> address_space_;
  };

} // namespace gandalf
#endif
