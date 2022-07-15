#include <gandalf/io.h>

namespace gandalf {
    IO::IO(Bus& bus) : bus_(bus), timer_(bus), ppu_(bus, lcd_) {
        bus.Register(&ppu_);
        bus.Register(&lcd_);
        bus.Register(&timer_);
        bus.Register(&serial_);
        bus.Register(&joypad_);
        bus.Register(&apu_);
    }

    IO::~IO() {
        bus_.Unregister(&ppu_);
        bus_.Unregister(&lcd_);
        bus_.Unregister(&timer_);
        bus_.Unregister(&serial_);
        bus_.Unregister(&joypad_);
        bus_.Unregister(&apu_);
    }

    void IO::Tick(byte cycles)
    {
        for (byte i = 0; i < cycles; ++i) {
            timer_.Tick();
            ppu_.Tick();
            serial_.Tick();
            apu_.Tick();
        }
    }

} // namespace gandalf