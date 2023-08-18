#ifndef __GUI_VRAM_VIEW_H
#define __GUI_VRAM_VIEW_H

#include <memory>
#include <gandalf/gameboy.h>
#include <gandalf/lcd.h>

#include "gui_element.h"

#include <SDL_opengl.h>

namespace gui
{
    class VRAMView : public GUIElement, public gandalf::PPU::VBlankListener
    {
    public:
        VRAMView(const bool& debug_enabled);
        ~VRAMView();

        void OnGameboyChanged() override;
        void Render() override;

    private:
        void OnVBlank() override;
        void RenderBackground(bool window);
        void UpdateBackground(bool window);
        void UpdateTiles();
        void UpdateSprites();

        enum class Tab
        {
            Background,
            Window,
            Tiles,
            Sprites
        };

        std::array<gandalf::LCD::ABGR1555, gandalf::TotalScreenHeight* gandalf::TotalScreenWidth> vram_buffer_;
        std::array<gandalf::LCD::ABGR1555, 40 * 8 * 16> sprite_buffer_;
        Tab current_tab_;
        const bool& debug_enabled_;
        GLuint background_texture_;
        GLuint sprite_texture_;

        struct TileData
        {
            std::uint8_t tile_number;
            std::uint16_t tile_address;
            std::uint16_t map_offset;
            std::uint16_t map_address;
            std::uint8_t attributes;
            unsigned int vram_bank;
            std::uint8_t palette;
            bool x_flip;
            bool y_flip;
            bool priority;
        };

        struct SpriteData : TileData
        {
            std::uint16_t address;
            std::uint8_t y;
            std::uint8_t x;
        };

        std::array<std::array<TileData, 32>, 32> tile_data_;
        std::array<SpriteData, 40> sprite_data_;
    };
}

#endif