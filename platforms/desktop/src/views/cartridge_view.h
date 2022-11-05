#ifndef __GUI_CARTRIDGE_VIEW_H
#define __GUI_CARTRIDGE_VIEW_H

#include "gui_element.h"

namespace gui
{
    class CartridgeView: public GUIElement
    {
    public:
        CartridgeView(const bool& debug_enabled);

        void Render() override;

    private:
        const bool& debug_enabled_;
    };
}

#endif