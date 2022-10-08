#include <gandalf/io.h>

#include <cassert>

namespace gandalf {
    IO::IO(GameboyMode mode, Bus& bus, std::shared_ptr<APU::OutputHandler> audio_handler) :
        bus_(bus),
        timer_(bus),
        lcd_(mode),
        ppu_(mode, bus, lcd_),
        apu_(audio_handler),
        dma_(bus),
        hdma_(bus, lcd_),
        mode_(mode)
    {
        bus_.Register(ppu_);
        bus_.Register(lcd_);
        bus_.Register(timer_);
        bus_.Register(serial_);
        bus_.Register(joypad_);
        bus_.Register(apu_);
        bus_.Register(dma_);
        bus_.Register(hdma_);
    }

    IO::~IO() {
        bus_.Unregister(ppu_);
        bus_.Unregister(lcd_);
        bus_.Unregister(timer_);
        bus_.Unregister(serial_);
        bus_.Unregister(joypad_);
        bus_.Unregister(apu_);
        bus_.Unregister(dma_);
        bus_.Unregister(hdma_);
    }

    void IO::Tick(unsigned int cycles, bool double_speed)
    {
        for (unsigned int i = 0; i < cycles; ++i) {
            timer_.Tick();
            serial_.Tick();
            dma_.Tick();
        }

        const unsigned int speed_cycles = double_speed ? (cycles / 2) : cycles;
        for (unsigned int i = 0; i < speed_cycles; ++i) {
            ppu_.Tick();
            apu_.Tick();

            if (mode_ == GameboyMode::CGB)
                hdma_.Tick();
        }

        if (mode_ == GameboyMode::CGB && hdma_.GetRemainingGDMACycles() > 0)
            Tick(hdma_.GetRemainingGDMACycles(), double_speed);
    }

} // namespace gandalf