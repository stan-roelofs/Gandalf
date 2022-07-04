#ifndef __GANDALF_CARTRIDGE_H
#define __GANDALF_CARTRIDGE_H

#include <vector>
#include <optional>
#include <string>

#include "types.h"

namespace gandalf {
  class Cartridge {
  public:
    Cartridge();
    virtual ~Cartridge();

    struct Header {
      byte logo[0x30];            // 0x104-0x133 - Logo
      byte title[0x10];           // 0x134-0x143 - Title
      byte manufacturer_code[4];  // 0x13F-0x142 - Manufacturer code
      byte cgb_flag;              // 0x143 - CGB flag
      byte new_licensee_code[2];  // 0x144-0x145 - New licensee code
      byte sgb_flag;              // 0x146 - SGB flag
      byte cartridge_type;        // 0x147 - Cartridge type
      byte rom_size;              // 0x148 - ROM size
      byte ram_size;              // 0x149 - RAM size
      byte destination_code;      // 0x14A - Destination code
      byte old_licensee_code;     // 0x14B - Old licensee code
      byte mask_rom_version;      // 0x14C - Mask ROM version number
      byte header_checksum;       // 0x14D - Header checksum
      byte global_checksum[2];    // 0x14E-0x14F - Global checksum

      std::string GetTitle() const;
      std::string GetManufacturerCode() const;
      std::string GetDestination() const;
      std::string GetLicensee() const;
      std::string GetType() const;
      std::string GetROMSize() const;
      std::string GetRAMSize() const;
      std::string GetCGBFlag() const;
      std::string GetSGBFlag() const;
    };

    /**
     * @brief Loads a ROM from a vector of bytes.
     *
     * @param bytes the cartridge bytes
     * @return true if loaded successfully, false otherwise
     */
    bool Load(const std::vector<byte>& bytes);

    /**
     * @brief Get the cartridge header.
     * @return The header of the cartridge if loaded, otherwise nullopt.
     */
    std::optional<Header> GetHeader() const;

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

  private:
    class Type {

    };

    std::optional<Header> header_;
  };

}
#endif