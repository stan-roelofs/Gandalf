#ifndef __GB_APP_GUI_H
#define __GB_APP_GUI_H

#include <optional>

#include <gandalf/gameboy.h>

namespace gui {
    struct Context {
        bool* run;
        bool* step;
        gandalf::Gameboy* gameboy;
        bool* show_debug_window;
        std::optional<gandalf::word>* breakpoint;
        gandalf::LCD::VideoBuffer** video_buffer;
        bool* sleep;
        bool* limit_frames;
    };

    bool SetupGUI();
    bool PollEvents(Context& context);
    void RenderGUI(Context& context);
    void DestroyGUI();
}
#endif