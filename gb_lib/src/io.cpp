#include <gandalf/io.h>

namespace gandalf {
    IO::IO(Bus& bus) : bus_(bus), timer_(bus), ppu_(bus), lcd_(bus) {
        bus.Register(&ppu_);
        bus.Register(&lcd_);
        bus.Register(&timer_);
    }

    IO::~IO() {
        bus_.Unregister(&ppu_);
        bus_.Unregister(&lcd_);
        bus_.Unregister(&timer_);
    }

    void IO::Tick(byte cycles)
    {
        for (byte i = 0; i < cycles; ++i) {
            timer_.Tick();
            ppu_.Tick();
            lcd_.Tick();
        }
    }

} // namespace gandalf