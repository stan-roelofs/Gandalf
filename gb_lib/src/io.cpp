#include <gandalf/io.h>

#include <cassert>

namespace gandalf {
    IO::IO(GameboyMode mode, Bus& bus, std::shared_ptr<APU::OutputHandler> audio_handler):
        bus_(bus),
        timer_(bus),
        lcd_(mode),
        ppu_(mode, bus, lcd_),
        serial_(mode),
        joypad_(bus),
        apu_(audio_handler),
        dma_(bus),
        hdma_(mode, bus, lcd_),
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

    void IO::SetMode(GameboyMode mode)
    {
        mode_ = mode;
        ppu_.SetMode(mode);
        lcd_.SetMode(mode);
        serial_.SetMode(mode);
        hdma_.SetMode(mode);
    }

    void IO::Tick(unsigned int cycles, bool double_speed)
    {
        assert(cycles % 2 == 0);

        for (unsigned int i = 0; i < cycles; ++i) {
            timer_.Tick();
            serial_.Tick();
            dma_.Tick();

            // In double speed the components above operate twice as fast. 
            // We implement this by running the components below twice as slow. 
            if (!double_speed || i % 2 == 0)
            {
                ppu_.Tick();
                apu_.Tick();

                if (mode_ == GameboyMode::CGB)
                    hdma_.Tick();
            }
        }

        // When using this transfer method, all data is transferred at once. The execution of the program is halted until the transfer has completed.
        if (mode_ == GameboyMode::CGB && hdma_.GetRemainingGDMACycles() > 0)
            Tick(double_speed ? hdma_.GetRemainingGDMACycles() * 2 : hdma_.GetRemainingGDMACycles(), double_speed);
    }

} // namespace gandalf