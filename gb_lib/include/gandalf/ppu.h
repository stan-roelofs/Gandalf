#ifndef __GANDALF_PPU_H
#define __GANDALF_PPU_H

#include <queue>
#include <functional>

#include "bus.h"
#include "lcd.h"

namespace gandalf {
    class VBlankListener
    {
    public:
        virtual void OnVBlank() = 0;
    };

    class PPU : public Bus::AddressHandler {
    public:
        PPU(Bus& bus, LCD& lcd);
        virtual ~PPU();

        void Tick();

        byte Read(word address) const override;
        void Write(word address, byte value) override;
        std::set<word> GetAddresses() const override;

        void SetVBlankListener(VBlankListener* listener)
        {
            vblank_listener_ = listener;
        }

    private:
        void IncrementLY();
        Bus& bus_;
        LCD& lcd_;
        int line_ticks_;

        using VRAM = std::array<byte, 0x2000>;
        VRAM vram_;
        std::array<byte, 0xA0> oam_;
        VBlankListener* vblank_listener_;

        class Pipeline {
        public:
            Pipeline(LCD& lcd, VRAM& vram);
            ~Pipeline();

            void Process(int line_ticks);
            void Reset();
            bool Done() const;
        private:
            void RenderPixel();
            void StateMachine();
            void TryPush();

            enum class FetcherState
            {
                kFetchTileSleep,
                kFetchTile,
                kFetchDataLowSleep,
                kFetchDataLow,
                kFetchDataHighSleep,
                kFetchDataHigh,
                kSleep1,
                kSleep2,
                kPush
            };

            struct Pixel
            {
                byte color : 2; // The color index of the pixel
                byte palette : 3; // The palette index to use for the pixel
                byte background_priority : 1;
                byte sprite_priority : 6; // Only for CGB. The index of the sprite in OAM
            };

            LCD& lcd_;
            const VRAM& vram_;
            std::queue<Pixel> background_fifo_;
            std::queue<Pixel> sprite_fifo_;
            FetcherState fetcher_state_;
            byte fetch_x_; // The X coordinate of the tile to fetch (0-31 because every tile has 8 pixels)
            byte fetch_y_; // The Y coordinate of the tile to fetch (0-255)
            byte pixels_pushed_; // The number of pixels we pushed so far
            byte tile_number_; // The tile number that is fetched during the first state of the fetcher
            byte tile_data_low_; // The low byte of the tile data that is fetched during the second state of the fetcher
            byte tile_data_high_; // The high byte of the tile data that is fetched during the third state of the fetcher
        };

        Pipeline pipeline_;
    };
}

#endif