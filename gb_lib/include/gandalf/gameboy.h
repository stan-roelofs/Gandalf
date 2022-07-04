#ifndef __GANDALF_GAMEBOY_H
#define __GANDALF_GAMEBOY_H

#include <memory>

#include "bus.h"
#include "cpu.h"
#include "timer.h"
#include "cartridge.h"

namespace gandalf {
  class Gameboy : public TimingHandler {
  public:
    Gameboy();
    ~Gameboy();

    bool Boot(const std::array<byte, 0x100>& boot_rom, const std::vector<byte>& rom);
    void Run();

    void Advance(byte) override {}

  private:
    Bus bus_;
    Timer timer_;
    CPU cpu_;
    Cartridge cartridge_;
  };
} // namespace gandalf
#endif
