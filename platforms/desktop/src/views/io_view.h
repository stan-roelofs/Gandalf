#ifndef __GUI_IO_VIEW_H
#define __GUI_IO_VIEW_H

#include "gui_element.h"

namespace gui
{
    class IOView: public GUIElement
    {
    public:
        IOView(const bool& debug_enabled);

        void Render() override;

    private:
        const bool& debug_enabled_;
    };
}

#endif