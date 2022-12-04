#ifndef __GB_APP_GUI_CONTEXT_H
#define __GB_APP_GUI_CONTEXT_H

#include <vector>

#include "settings.h"

namespace gui
{
    class KeyboardHandler
    {
    public:
        virtual void HandleKey(std::int32_t key, bool pressed) = 0;
    };

    class GUIContext
    {
    public:
        GUIContext();
        void AddKeyboardHandler(KeyboardHandler* handler);
        void RemoveKeyboardHandler(KeyboardHandler* handler);
        KeyboardHandler* GetKeyboardHandler();

        Settings& GetSettings();

    private:
        Settings settings_;
        std::vector<KeyboardHandler*> keyboard_handlers_;
    };
} // namespace gui

#endif