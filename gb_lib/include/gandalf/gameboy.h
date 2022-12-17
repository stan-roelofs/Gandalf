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
#include "timer.h"
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
    Timer& GetTimer() { return io_->GetTimer(); }

    GameboyMode GetMode() const { return mode_; }

  private:
    void OnBootROMFinished();
    void LoadROM(const ROM& rom);
    void LoadBootROM(const ROM& boot_rom);

    GameboyMode mode_;

    // Keep in this order! The bus needs to be destroyed last, and io needs to be destroyed before cpu.
    Bus bus_;
    std::unique_ptr<IO> io_;
    std::unique_ptr<CPU> cpu_;
    std::unique_ptr<WRAM> wram_;
    std::unique_ptr<HRAM> hram_;
    std::unique_ptr<Cartridge> cartridge_;

    class BootROMHandler: public Bus::AddressHandler
    {
    public:
      BootROMHandler(Gameboy& gb, const std::vector<byte> boot_rom): Bus::AddressHandler("Boot ROM"), key0_(0), boot_rom_(boot_rom), gb_(gb)
      {
      }
      virtual ~BootROMHandler() = default;

      void Write(word address, byte value) override
      {
        if (address == kBANK && value != 0)
        {
          gb_.OnBootROMFinished();
        }
        else if (address == kKEY0)
          key0_ = value;
      }

      byte Read(word address) const override
      {
        assert(address < boot_rom_.size() || address == kBANK || address == kKEY0);
        if (address < boot_rom_.size())
          return boot_rom_[address];

        if (address == kKEY0)
          return key0_;

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
        addresses.insert(kKEY0);
        return addresses;
      }

    private:
      byte key0_;
      const std::vector<byte> boot_rom_;
      Gameboy& gb_;
    };
    std::unique_ptr<BootROMHandler> boot_rom_handler_;
  };
} // namespace gandalf
#endif
