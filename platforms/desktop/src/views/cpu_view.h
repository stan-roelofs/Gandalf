#ifndef __GUI_CPU_VIEW_H
#define __GUI_CPU_VIEW_H

#include <optional>

#include <gandalf/gameboy.h>
#include <gandalf/lcd.h>

#include "gui_element.h"

namespace gui
{
    class CPUView: public GUIElement
    {
    public:
        CPUView(const bool& debug_enabled, bool& gb_pause, bool& block_audio, bool& step, std::optional<gandalf::word>& breakpoint);

        void Render() override;

    private:
        const bool& debug_enabled_;
        bool& gb_pause_;
        bool& block_audio_;
        bool& step_;
        std::optional<gandalf::word>& breakpoint_;
    };
}

#endif