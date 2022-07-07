#ifndef __GANDALF_GAMEBOY_H
#define __GANDALF_GAMEBOY_H

#include <memory>

#include "bus.h"
#include "cpu.h"
#include "cartridge.h"
#include "constants.h"
#include "hram.h"
#include "io.h"
#include "wram.h"

namespace gandalf {
  // TODO move this somewhere else
  class BootROMHandler : public Bus::AddressHandler
  {
  public:
    BootROMHandler(const std::array<byte, 0x100>& boot_rom) : Bus::AddressHandler("Boot ROM"), should_unregister_(false), boot_rom_(boot_rom)
    {
    }
    virtual ~BootROMHandler() = default;

    void Write(word address, byte value) override
    {
      if (address != kBANK)
        return;

      if (value != 0)
        should_unregister_ = true;
    }

    byte Read(word address) const override
    {
      if (address < 0x100)
        return boot_rom_[address];

      if (address == kBANK)
        return 0xFF;

      throw Exception("Invalid read from boot ROM");
    }

    std::set<word> GetAddresses() const override
    {
      std::set<word> addresses;
      for (word i = 0; i < 0x100; ++i)
        addresses.insert(i);
      addresses.insert(kBANK);
      return addresses;
    }

    bool Done() const { return should_unregister_; }

  private:
    bool should_unregister_;
    const std::array<byte, 0x100>& boot_rom_;
  };

  class Gameboy {
  public:
    using BootROM = std::array<byte, 0x100>;

    Gameboy();
    ~Gameboy();

    bool Load(const Cartridge::ROM& rom);
    void LoadBootROM(const BootROM& boot_rom);

    void Run();

    const Cartridge& GetCartridge() const { return *cartridge_; }

    CPU& GetCPU() { return *cpu_; }
    Bus& GetBus() { return *bus_; }

  private:
    // Keep in this order! The bus needs to be destroyed last, and io needs to be destroyed before cpu.
    std::unique_ptr<Bus> bus_;
    std::unique_ptr<IO> io_;
    std::unique_ptr<CPU> cpu_;
    std::unique_ptr<WRAM> wram_;
    std::unique_ptr<HRAM> hram_;

    std::unique_ptr<Cartridge> cartridge_;

    std::unique_ptr<BootROMHandler> boot_rom_handler_;
  };
} // namespace gandalf
#endif
