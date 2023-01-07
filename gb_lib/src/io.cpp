#include <gandalf/io.h>

#include <cassert>

namespace gandalf {
    IO::IO(GameboyMode mode, Memory& memory):
        memory_(memory),
        timer_(memory),
        lcd_(mode),
        ppu_(mode, memory, lcd_),
        serial_(mode),
        joypad_(memory),
        dma_(memory),
        hdma_(mode, memory, lcd_),
        mode_(mode)
    {
        memory_.Register(ppu_);
        memory_.Register(lcd_);
        memory_.Register(timer_);
        memory_.Register(serial_);
        memory_.Register(joypad_);
        memory_.Register(apu_);
        memory_.Register(dma_);
        memory_.Register(hdma_);
    }

    IO::~IO() {
        memory_.Unregister(ppu_);
        memory_.Unregister(lcd_);
        memory_.Unregister(timer_);
        memory_.Unregister(serial_);
        memory_.Unregister(joypad_);
        memory_.Unregister(apu_);
        memory_.Unregister(dma_);
        memory_.Unregister(hdma_);
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