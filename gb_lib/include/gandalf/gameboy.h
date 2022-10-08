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
    Gameboy(const ROM& boot_rom, const ROM& rom, std::shared_ptr<APU::OutputHandler> audio_handler);
    ~Gameboy();

    /// @returns Whether the Gameboy is ready to run (ROM is loaded and Boot ROM is loaded)
    bool Ready() const;

    void Run();

    const std::unique_ptr<Cartridge>& GetCartridge() const { return cartridge_; }

    CPU& GetCPU() { return *cpu_; }
    Bus& GetBus() { return bus_; }
    LCD& GetLCD() { return io_->GetLCD(); }
    PPU& GetPPU() { return io_->GetPPU(); }
    Joypad& GetJoypad() { return io_->GetJoypad(); }
    APU& GetAPU() { return io_->GetAPU(); }

  private:
    void LoadROM(const ROM& rom);
    void LoadBootROM(const ROM& boot_rom);

    // Keep in this order! The bus needs to be destroyed last, and io needs to be destroyed before cpu.
    Bus bus_;
    std::unique_ptr<IO> io_;
    std::unique_ptr<CPU> cpu_;
    std::unique_ptr<WRAM> wram_;
    std::unique_ptr<HRAM> hram_;
    std::unique_ptr<Cartridge> cartridge_;

    class BootROMHandler : public Bus::AddressHandler
    {
    public:
      BootROMHandler(const std::vector<byte> boot_rom, Cartridge& cartridge, Bus& bus) : Bus::AddressHandler("Boot ROM"), boot_rom_(boot_rom), cartridge_(cartridge), bus_(bus)
      {
        bus_.Register(*this);
      }
      virtual ~BootROMHandler() = default;

      void Write(word address, byte value) override
      {
        if (address != kBANK)
          return;

        if (value != 0)
        {
          bus_.Unregister(*this);
          bus_.Register(cartridge_);
        }
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

    private:
      const std::vector<byte> boot_rom_;
      Cartridge& cartridge_;
      Bus& bus_;
    };
    std::unique_ptr<BootROMHandler> boot_rom_handler_;
  };
} // namespace gandalf
#endif
