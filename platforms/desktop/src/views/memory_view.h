#ifndef __GUI_MEMORY_VIEW_H
#define __GUI_MEMORY_VIEW_H

#include "gui_element.h"

namespace gui
{
    class MemoryView: public GUIElement
    {
    public:
        MemoryView(const bool& debug_enabled);

        void Render() override;

    private:
        const bool& debug_enabled_;
    };
}

#endif