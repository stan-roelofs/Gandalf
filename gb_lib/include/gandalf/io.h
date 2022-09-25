#ifndef __GANDALF_IO_H
#define __GANDALF_IO_H

#include "apu.h"
#include "bus.h"
#include "dma.h"
#include "joypad.h"
#include "lcd.h"
#include "ppu.h"
#include "serial.h"
#include "timer.h"
#include "hdma.h"

namespace gandalf {
    class IO {
    public:
        IO(GameboyMode mode, Bus& bus);
        ~IO();

        void Tick(unsigned int cycles, bool double_speed);

        LCD& GetLCD() { return lcd_; }
        PPU& GetPPU() { return ppu_; }
        Joypad& GetJoypad() { return joypad_; }
        APU& GetAPU() { return apu_; }

    private:
        Bus& bus_;
        Timer timer_;
        LCD lcd_;
        PPU ppu_;
        Serial serial_;
        Joypad joypad_;
        APU apu_;
        DMA dma_;
        HDMA hdma_;

        GameboyMode mode_;
    };
} // namespace gandalf

#endif