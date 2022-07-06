#ifndef __GB_APP_GUI_H
#define __GB_APP_GUI_H

#include <gandalf/gameboy.h>

namespace gui {
    struct Context {
        bool* run;
        bool* step;
        gandalf::Gameboy* gameboy;
        bool* show_debug_window;
    };

    bool SetupGUI();
    bool PollEvents();
    void RenderGUI(Context& context);
    void DestroyGUI();
}
#endif