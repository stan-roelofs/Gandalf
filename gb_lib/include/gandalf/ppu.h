#ifndef __GANDALF_PPU_H
#define __GANDALF_PPU_H

#include <deque>
#include <map>

#include "bus.h"
#include "lcd.h"

namespace gandalf {
    class PPU : public Bus::AddressHandler {
    public:
        class VBlankListener
        {
        public:
            virtual void OnVBlank() = 0;
        };

        PPU(GameboyMode mode, Bus& bus, LCD& lcd);
        virtual ~PPU();

        void Tick();

        byte Read(word address) const override;
        void Write(word address, byte value) override;
        std::set<word> GetAddresses() const override;

        void SetVBlankListener(VBlankListener* listener) { vblank_listener_ = listener; }

    private:
        void IncrementLY();

        Bus& bus_;
        LCD& lcd_;
        int line_ticks_;

        GameboyMode mode_;

        using VRAMBank = std::array<byte, 0x2000>;
        using VRAM = std::array<VRAMBank, 2>;
        VRAM vram_;
        int current_vram_bank_;
        byte opri_;
        std::array<byte, 0xA0> oam_;
        VBlankListener* vblank_listener_;
        struct Sprite {
            byte y;
            byte x;
            byte tile_index;
            byte attributes;
            byte tile_data_low;
            byte tile_data_high;
            byte oam_index;

            bool operator<(const Sprite& other) { return x < other.x; }
            bool operator<(int position) { return x < position; }
        };
        using FetchedSprites = std::map<byte, std::deque<Sprite>>;
        FetchedSprites fetched_sprites_;

        class Pipeline {
        public:
            Pipeline(GameboyMode mode, LCD& lcd, VRAM& vram, const int& vram_bank, FetchedSprites& fetched_sprites, const byte& opri);
            ~Pipeline();

            void Process();
            void Reset();
            bool Done() const;
        private:
            void RenderPixel();
            void TileStateMachine();
            void SpriteStateMachine();
            void TryPush();
            void PushSprite();

            enum class FetcherState
            {
                kFetchTileSleep,
                kFetchTile,
                kFetchDataLowSleep,
                kFetchDataLow,
                kFetchDataHighSleep,
                kFetchDataHigh,
                kPush
            };

            enum class SpriteState
            {
                kReadOAMSleep,
                kReadOAM,
                kReadDataLowSleep,
                kReadDataLow,
                kReadDataHighSleep,
                kReadDataHigh,
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
            const int& current_vram_bank_;
            std::deque<Pixel> background_fifo_;
            std::deque<Pixel> sprite_fifo_;
            FetcherState fetcher_state_;
            byte fetch_x_; // The X coordinate of the tile to fetch (0-31 because every tile has 8 pixels)
            byte fetch_y_; // The Y coordinate of the tile to fetch (0-255)
            byte pixels_pushed_; // The number of pixels we pushed so far
            byte tile_number_; // The tile number that is fetched during the first state of the fetcher
            byte tile_data_low_; // The low byte of the tile data that is fetched during the second state of the fetcher
            byte tile_data_high_; // The high byte of the tile data that is fetched during the third state of the fetcher
            bool sprite_in_progress_;
            FetchedSprites& fetched_sprites_;
            Sprite current_sprite_;
            SpriteState sprite_state_;
            int sprite_line_;
            int drop_pixels_;
            bool window_triggered_;
            GameboyMode mode_;
            byte tile_attributes_; // Tile attributes (CGB only)
            const byte& opri_;
        };

        Pipeline pipeline_;
    };
}

#endif