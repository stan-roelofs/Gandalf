#ifndef __GANDALF_LCD_H
#define __GANDALF_LCD_H

#include <array>

#include "bus.h"
#include "constants.h"

namespace gandalf {
    class LCD: public Bus::AddressHandler {
    public:
        using ABGR1555 = word;
        using VideoBuffer = std::array<ABGR1555, kScreenWidth* kScreenHeight>;

        LCD(GameboyMode mode);
        virtual ~LCD();

        void SetMode(GameboyMode mode) { mode_ = mode; }

        byte Read(word address) const override;
        void Write(word address, byte value) override;
        std::set<word> GetAddresses() const override;

        byte GetLCDControl() const { return lcdc_; }
        byte GetLCDStatus() const { return stat_; }
        void SetLCDStatus(byte status) { stat_ = status; }
        byte GetLY() const { return ly_; }
        void SetLY(byte ly) { ly_ = ly; }
        byte GetLYC() const { return lyc_; }
        byte GetSCX() const { return scx_; }
        byte GetSCY() const { return scy_; }
        byte GetBGP() const { return bgp_; }
        byte GetOBP0() const { return obp0_; }
        byte GetOBP1() const { return obp1_; }
        byte GetWY() const { return wy_; }
        byte GetWX() const { return wx_; }

        /**
         * Returns the color for the given color index and palette index
         * @param color_index The color index (0-3)
         * @param palette_index The palette index (0-1 for DMG, 0-7 for CGB)
        */
        ABGR1555 GetSpriteColor(byte color_index, byte palette_index) const;

        /**
         * Returns the color for the given color index and palette index
         * @param color_index The color index (0-3)
         * @param palette_index The palette index (0-1 for DMG, 0-7 for CGB)
        */
        ABGR1555 GetBackgroundColor(byte color_index, byte palette_index) const;

        enum class Mode
        {
            HBlank,
            VBlank,
            OamSearch,
            PixelTransfer
        };

        Mode GetMode() const;
        void SetMode(Mode mode);

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
        byte bcps_;
        byte ocps_;

        std::array<word, 32> bcpd_;
        std::array<word, 32> ocpd_;

        GameboyMode mode_;
    };
} // namespace gandalf

#endif