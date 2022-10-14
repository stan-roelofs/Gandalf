#ifndef __GUI_VRAM_VIEW_H
#define __GUI_VRAM_VIEW_H

#include <memory>
#include <gandalf/gameboy.h>
#include <gandalf/lcd.h>

#include "gui_element.h"

#include <SDL.h>

namespace gui
{
    class VRAMView : public GUIElement, public gandalf::PPU::VBlankListener
    {
    public:
        VRAMView(const bool& debug_enabled, SDL_Renderer& renderer);
        ~VRAMView();

        void SetGameboy(std::shared_ptr<gandalf::Gameboy> gameboy) override;
        void Render() override;

    private:
        void OnVBlank() override;

        std::array<gandalf::LCD::BGR555, gandalf::kTotalScreenHeight* gandalf::kTotalScreenWidth> vram_buffer_;
        bool visible_;
        const bool& debug_enabled_;
        SDL_Texture* texture_;
    };
}

#endif