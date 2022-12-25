#ifndef __GANDALF_IO_H
#define __GANDALF_IO_H

#include "apu.h"
#include "memory.h"
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
        IO(GameboyMode mode, Memory& memory, std::shared_ptr<APU::OutputHandler> audio_handler);
        ~IO();

        void Tick(unsigned int cycles, bool double_speed);

        LCD& GetLCD() { return lcd_; }
        PPU& GetPPU() { return ppu_; }
        Joypad& GetJoypad() { return joypad_; }
        APU& GetAPU() { return apu_; }
        Timer& GetTimer() { return timer_; }

        void SetMode(GameboyMode mode);

    private:
        Memory& memory_;
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