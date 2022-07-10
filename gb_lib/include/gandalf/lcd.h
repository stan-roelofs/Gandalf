#ifndef __GANDALF_LCD_H
#define __GANDALF_LCD_H

#include <array>

#include "bus.h"
#include "constants.h"

namespace gandalf {
    class LCD : public Bus::AddressHandler {
    public:
        using BGR565 = word;
        using VideoBuffer = std::array<BGR565, kScreenWidth* kScreenHeight>;

        LCD();
        virtual ~LCD();

        void Tick();

        byte Read(word address) const override;
        void Write(word address, byte value) override;
        std::set<word> GetAddresses() const override;

        byte& GetLCDControl() { return lcdc_; }
        byte& GetLCDStatus() { return stat_; }
        byte& GetLY() { return ly_; }
        byte& GetLYC() { return lyc_; }
        byte& GetSCX() { return scx_; }
        byte& GetSCY() { return scy_; }
        byte& GetBGP() { return bgp_; }
        byte& GetOBP0() { return obp0_; }
        byte& GetOBP1() { return obp1_; }

        /**
         * Pushes a pixel onto the screen.
         *
         * @param x the x position of the pixel
         * @param color_index the color index of the pixel (0-3)
         * @param is_sprite indicates whether this is a sprite pixel or not, we need this to determine whether the use the background or sprite palette
         * @param palette the index of the palette to use for the pixel.
         */
        void RenderPixel(byte x, byte color_index, bool is_sprite, byte palette_index);

        const VideoBuffer& GetVideoBuffer() const { return video_buffer_; }
    private:
        VideoBuffer video_buffer_;
        byte lcdc_;
        byte ly_;
        byte lyc_;
        byte stat_;
        byte scy_;
        byte scx_;
        byte wy_;
        byte wx_;
        byte bgp_;
        byte obp0_;
        byte obp1_;
        byte dma_;
    };
} // namespace gandalf

#endif