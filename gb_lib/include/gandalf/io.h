#ifndef __GANDALF_IO_H
#define __GANDALF_IO_H

#include "bus.h"
#include "timer.h"
#include "lcd.h"
#include "ppu.h"
#include "serial.h"

namespace gandalf {
    class IO {
    public:
        IO(Bus& bus);
        ~IO();

        void Tick(byte cycles);

    private:
        Bus& bus_;
        Timer timer_;
        LCD lcd_;
        PPU ppu_;
        Serial serial_;
    };
} // namespace gandalf

#endif