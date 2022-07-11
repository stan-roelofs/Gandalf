#ifndef __GANDALF_IO_H
#define __GANDALF_IO_H

#include "bus.h"
#include "joypad.h"
#include "lcd.h"
#include "ppu.h"
#include "serial.h"
#include "timer.h"

namespace gandalf {
    class IO {
    public:
        IO(Bus& bus);
        ~IO();

        void Tick(byte cycles);

        LCD& GetLCD() { return lcd_; }
        PPU& GetPPU() { return ppu_; }
        Joypad& GetJoypad() { return joypad_; }

    private:
        Bus& bus_;
        Timer timer_;
        LCD lcd_;
        PPU ppu_;
        Serial serial_;
        Joypad joypad_;
    };
} // namespace gandalf

#endif