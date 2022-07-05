#ifndef __GB_APP_GUI_H
#define __GB_APP_GUI_H

namespace gui {
    bool SetupGUI();
    bool PollEvents();
    void RenderGUI();
    void DestroyGUI();
}
#endif