#ifndef __GANDALF_CARTRIDGE_H
#define __GANDALF_CARTRIDGE_H

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include <gandalf/bus.h>
#include <gandalf/types.h>

#include "mbc.h"

namespace gandalf {
  class Cartridge : public Bus::AddressHandler {
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
    bool Load(const ROM& bytes);

    /**
     * @brief Get the cartridge header.
     * @return The header of the cartridge
     */
    Header GetHeader() const;

    void Write(word address, byte value) override;
    byte Read(word address) const override;
    std::set<word> GetAddresses() const override;

  private:
    Header header_;
    std::unique_ptr<MBC> mbc_;
  };
}
#endif