#include <gandalf/io.h>

namespace gandalf {
    IO::IO(Bus& bus) : bus_(bus), timer_(bus), ppu_(bus, lcd_), dma_(bus) {
        bus_.Register(&ppu_);
        bus_.Register(&lcd_);
        bus_.Register(&timer_);
        bus_.Register(&serial_);
        bus_.Register(&joypad_);
        bus_.Register(&apu_);
        bus_.Register(&dma_);
    }

    IO::~IO() {
        bus_.Unregister(&ppu_);
        bus_.Unregister(&lcd_);
        bus_.Unregister(&timer_);
        bus_.Unregister(&serial_);
        bus_.Unregister(&joypad_);
        bus_.Unregister(&apu_);
        bus_.Unregister(&dma_);
    }

    void IO::Tick(unsigned int cycles, bool double_speed)
    {
        for (unsigned int i = 0; i < (double_speed ? cycles * 2 : cycles); ++i) {
            timer_.Tick();
            serial_.Tick();
            dma_.Tick();
        }

        for (unsigned int i = 0; i < cycles; ++i) {
            ppu_.Tick();
            apu_.Tick();
        }
    }

} // namespace gandalf