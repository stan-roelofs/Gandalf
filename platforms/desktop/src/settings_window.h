#ifndef __GB_APP_SETTINGS_WINDOW_H
#define __GB_APP_SETTINGS_WINDOW_H

#include "settings.h"

namespace gui
{
    class SettingsWindow
    {
    public:
        SettingsWindow(settings::Settings& settings);
        ~SettingsWindow();

        void Show();
        bool Terminated() const { return terminated_; }

    private:
        settings::Settings& settings_;
        settings::Settings settings_copy_;
        bool show_;
        bool terminated_;
    };
} // namespace gui

#endif