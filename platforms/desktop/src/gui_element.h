#ifndef __GB_APP_GUI_ELEMENT_H
#define __GB_APP_GUI_ELEMENT_H

#include <gandalf/gameboy.h>

namespace gui
{
    class GUIElement
    {
    public:
        virtual void Render() = 0;
        virtual void SetGameboy(std::shared_ptr<gandalf::Gameboy> gb) { 
            if (!gb)
                throw std::runtime_error("Gameboy was nullptr!");
            gameboy_ = gb;
        }

    protected:
        std::shared_ptr<gandalf::Gameboy> gameboy_;
    };
}

#endif