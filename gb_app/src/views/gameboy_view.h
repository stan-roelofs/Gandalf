#ifndef __GUI_GAMEBOY_VIEW_H
#define __GUI_GAMEBOY_VIEW_H

#include <memory>
#include <gandalf/gameboy.h>
#include <gandalf/lcd.h>
#include <gandalf/ppu.h>

#include "gui_element.h"

#include <SDL_opengl.h>

namespace gui
{
    class GameboyView : public GUIElement, public gandalf::PPU::VBlankListener
    {
    public:
        GameboyView();
        ~GameboyView();

        void SetGameboy(std::shared_ptr<gandalf::Gameboy> gameboy) override;
        void Render() override;

    private:
        void OnVBlank() override;

        std::unique_ptr<gandalf::LCD::VideoBuffer> front_buffer_;
        std::unique_ptr<gandalf::LCD::VideoBuffer> back_buffer_;
        GLuint texture_;
        int scale_;
    };
}

#endif