#ifndef __GANDALF_CARTRIDGE_H
#define __GANDALF_CARTRIDGE_H

#include <vector>
#include <optional>
#include <string>

#include <gandalf/types.h>

namespace gandalf {
  class Cartridge {
  public:
    Cartridge();
    virtual ~Cartridge();

    enum class Type : byte {
      kROMOnly = 0x00,
      kMBC1 = 0x01,
      kMBC1RAM = 0x02,
      kMBC1RAMBattery = 0x03,
      kMBC2 = 0x05,
      kMBC2Battery = 0x06,
      kROMRAM = 0x08,
      kROMRAMBattery = 0x09,
      kMMM01 = 0x0B,
      kMMM01RAM = 0x0C,
      kMMM01RAMBattery = 0x0D,
      kMBC3TimerBattery = 0x0F,
      kMBC3TimerRAMBattery = 0x10,
      kMBC3 = 0x11,
      kMBC3RAM = 0x12,
      kMBC3RAMBattery = 0x13,
      kMBC5 = 0x19,
      kMBC5RAM = 0x1A,
      kMBC5RAMBattery = 0x1B,
      kMBC5Rumble = 0x1C,
      kMBC5RumbleRAM = 0x1D,
      kMBC5RumbleRAMBattery = 0x1E,
      kMBC6 = 0x20,
      kMBC7 = 0x22,
      kPocketCamera = 0xFC,
      kBandaiTAMA5 = 0xFD,
      kHuC3 = 0xFE,
      kHuC1 = 0xFF,
    };

    class Header {
    public:
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
      std::string GetNewLicenseeCode() const;
      Type GetCartridgeType() const;
      size_t GetROMSizeBytes() const;
      size_t GetRAMSizeBytes() const;
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
    std::optional<Header> header_;
  };

}
#endif