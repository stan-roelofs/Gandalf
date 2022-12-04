#include "gui_context.h"

namespace gui
{
    GUIContext::GUIContext() = default;

    void GUIContext::AddKeyboardHandler(KeyboardHandler* handler)
    {
        keyboard_handlers_.push_back(handler);
    }

    void GUIContext::RemoveKeyboardHandler(KeyboardHandler* handler)
    {
        auto it = std::find(keyboard_handlers_.begin(), keyboard_handlers_.end(), handler);
        if (it != keyboard_handlers_.end())
            keyboard_handlers_.erase(it);
    }

    KeyboardHandler* GUIContext::GetKeyboardHandler()
    {
        if (keyboard_handlers_.empty())
            return nullptr;

        return keyboard_handlers_.back();
    }

    Settings& GUIContext::GetSettings() { return settings_; }
} // namespace gui