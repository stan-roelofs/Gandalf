#ifndef __CARTRIDGE_H
#define __CARTRIDGE_H

#include <cstdint>
#include <vector>
#include <optional>

namespace gandalf {
  class Cartridge {
  public:
    Cartridge();
    virtual ~Cartridge();

    struct Header {
      std::uint8_t logo[0x30];            // 0x104-0x133 - Logo
      std::uint8_t title[0x10];           // 0x134-0x143 - Title
      std::uint8_t manufacturer_code[4];  // 0x13F-0x142 - Manufacturer code
      std::uint8_t cgb_flag;              // 0x143 - CGB flag
      std::uint8_t new_licensee_code[2];  // 0x144-0x145 - New licensee code
      std::uint8_t sgb_flag;              // 0x146 - SGB flag
      std::uint8_t cartridge_type;        // 0x147 - Cartridge type
      std::uint8_t rom_size;              // 0x148 - ROM size
      std::uint8_t ram_size;              // 0x149 - RAM size
      std::uint8_t destination_code;      // 0x14A - Destination code
      std::uint8_t old_licensee_code;     // 0x14B - Old licensee code
      std::uint8_t mask_rom_version;      // 0x14C - Mask ROM version number
      std::uint8_t header_checksum;       // 0x14D - Header checksum
      std::uint8_t global_checksum[2];    // 0x14E-0x14F - Global checksum
    };

    /**
     * @brief Loads a ROM from a vector of bytes.
     *
     * @param bytes the cartridge bytes
     * @return true if loaded successfully, false otherwise
     */
    bool Load(const std::vector<std::uint8_t>& bytes);

    /**
     * @brief Get the cartridge header.
     * @return The header of the cartridge if loaded, otherwise nullopt.
     */
    std::optional<Header> GetHeader() const;

  private:
    std::optional<Header> header_;
  };

}
#endif