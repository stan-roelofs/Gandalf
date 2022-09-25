#ifndef __GANDALF_GAMEBOY_H
#define __GANDALF_GAMEBOY_H

#include <memory>
#include <cassert>

#include "bus.h"
#include "cpu.h"
#include "cartridge.h"
#include "constants.h"
#include "hram.h"
#include "io.h"
#include "model.h"
#include "wram.h"

namespace gandalf {
  class Gameboy {
  public:
    using BootROM = std::vector<byte>;

    Gameboy(const BootROM& boot_rom, const ROM& rom);
    ~Gameboy();

    /// @returns Whether the Gameboy is ready to run (ROM is loaded and Boot ROM is loaded)
    bool Ready() const;

    void Run();

    const std::unique_ptr<Cartridge>& GetCartridge() const { return cartridge_; }

    CPU& GetCPU() { return cpu_; }
    Bus& GetBus() { return bus_; }
    LCD& GetLCD() { return io_.GetLCD(); }
    PPU& GetPPU() { return io_.GetPPU(); }
    Joypad& GetJoypad() { return io_.GetJoypad(); }
    APU& GetAPU() { return io_.GetAPU(); }

  private:
    void LoadROM(const ROM& rom);
    void LoadBootROM(const BootROM& boot_rom);

    // Keep in this order! The bus needs to be destroyed last, and io needs to be destroyed before cpu.
    Bus bus_;
    IO io_;
    CPU cpu_;
    WRAM wram_;
    HRAM hram_;
    std::unique_ptr<Cartridge> cartridge_;

    class BootROMHandler : public Bus::AddressHandler
    {
    public:
      BootROMHandler(const std::vector<byte> boot_rom) : Bus::AddressHandler("Boot ROM"), should_unregister_(false), boot_rom_(boot_rom)
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
        assert(address < boot_rom_.size() || address == kBANK);
        if (address < boot_rom_.size())
          return boot_rom_[address];

        return 0xFF;
      }

      std::set<word> GetAddresses() const override
      {
        std::set<word> addresses;
        for (word i = 0; i < boot_rom_.size() && i < 0x100; ++i)
          addresses.insert(i);

        for (word i = 0x200; i < boot_rom_.size(); ++i)
            addresses.insert(i);

        addresses.insert(kBANK);
        return addresses;
      }

      bool Done() const { return should_unregister_; }

    private:
      bool should_unregister_;
      const std::vector<byte> boot_rom_;
    };
    std::unique_ptr<BootROMHandler> boot_rom_handler_;
    bool executed_boot_rom_;
  };
} // namespace gandalf
#endif
