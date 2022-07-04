#include <gandalf/gameboy.h>

namespace gandalf {

    Gameboy::Gameboy() : bus_(std::make_unique<Bus>()), timer_(*bus_) {

    }

    Gameboy::~Gameboy()
    {
        bus_.release();
    }

} // namespace gandalf