#ifndef __GUI_VRAM_VIEW_H
#define __GUI_VRAM_VIEW_H

#include <memory>
#include <gandalf/gameboy.h>
#include <gandalf/lcd.h>

#include "gui_element.h"

#include <SDL_opengl.h>

namespace gui
{
    class VRAMView: public GUIElement, public gandalf::PPU::VBlankListener
    {
    public:
        VRAMView(const bool& debug_enabled);
        ~VRAMView();

        void OnGameboyChanged() override;
        void Render() override;

    private:
        void OnVBlank() override;

        std::array<gandalf::LCD::ABGR1555, gandalf::kTotalScreenHeight* gandalf::kTotalScreenWidth> vram_buffer_;
        bool visible_;
        const bool& debug_enabled_;
        GLuint texture_;

        struct TileData
        {
            std::uint8_t tile_number;
            std::uint16_t tile_address;
            std::uint16_t map_address;
            std::uint8_t attributes;
            unsigned int vram_bank;
            std::uint8_t palette;
            bool x_flip;
            bool y_flip;
            bool priority;
        };

        std::array<std::array<TileData, 32>, 32> tile_data_;
    };
}

#endif