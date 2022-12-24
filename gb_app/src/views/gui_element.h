#ifndef __GB_APP_GUI_ELEMENT_H
#define __GB_APP_GUI_ELEMENT_H

#include <gandalf/gameboy.h>

namespace gui
{
    class GUIElement
    {
    public:
        virtual void Render() = 0;
        void SetGameboy(std::shared_ptr<gandalf::Gameboy> gb) {
            assert(gb);
            gameboy_ = gb;
            OnGameboyChanged();
        }

        virtual void OnGameboyChanged() {}

    protected:
        std::shared_ptr<gandalf::Gameboy> gameboy_;
    };
}

#endif