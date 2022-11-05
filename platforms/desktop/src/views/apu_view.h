#ifndef __GUI_APU_VIEW_H
#define __GUI_APU_VIEW_H

#include "gui_element.h"

namespace gui
{
    class APUView: public GUIElement
    {
    public:
        APUView(const bool& debug_enabled);

        void Render() override;

    private:
        const bool& debug_enabled_;
    };
}

#endif