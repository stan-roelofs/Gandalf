#ifndef __GB_APP_SETTINGS_WINDOW_H
#define __GB_APP_SETTINGS_WINDOW_H

#include "gui_context.h"
#include "settings.h"
#include <optional>

namespace gui
{
    class SettingsWindow : KeyboardHandler
    {
    public:
        SettingsWindow(GUIContext& ctx);
        ~SettingsWindow();

        void Show();
        bool Terminated() const { return terminated_; }

    private:
        void HandleKey(std::int32_t key, bool pressed) override;
        void CreateButtonRow(const char* button, std::int32_t* current_key);

        GUIContext& ctx_;
        Settings settings_copy_;
        bool show_;
        bool terminated_;

        std::optional<std::int32_t> last_key_;
    };
} // namespace gui

#endif