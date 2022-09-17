#ifndef __GANDALF_CARTRIDGE_H
#define __GANDALF_CARTRIDGE_H

#include <memory>
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

    enum class CGBFunctionality
    {
        kOnly,
        kSupported,
        kNotSupported
    };

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

      CGBFunctionality GetCGBFlag() const;

      std::string GetTitleString() const;
      std::string GetManufacturerCodeString() const;
      std::string GetDestinationString() const;
      std::string GetLicenseeString() const;
      std::string GetTypeString() const;
      std::string GetROMSizeString() const;
      std::string GetRAMSizeString() const;
      std::string GetCGBFlagString() const;
      std::string GetSGBFlagString() const;
    };

    /**
     * Loads a ROM from a vector of bytes. Any previously loaded data will be cleared when this is called.
     *
     * @param bytes the cartridge bytes
     * @return true if loaded successfully, false otherwise
     */
    bool Load(const ROM& bytes);

    /// @return True if a cartridge is loaded, false otherwise.
    bool Loaded() const;

    /**
     * @brief Get the cartridge header.
     * @return The header of the cartridge, or nullptr if not loaded.
    */
    std::shared_ptr<const Header> GetHeader() const;

    void Write(word address, byte value) override;
    byte Read(word address) const override;
    std::set<word> GetAddresses() const override;

  private:
    std::shared_ptr<const Header> header_;
    std::unique_ptr<MBC> mbc_;
  };
}
#endif