#ifndef __GB_APP_MAIN_WINDOW_H
#define __GB_APP_MAIN_WINDOW_H

#include <optional>
#include <filesystem>
#include <thread>

#include <gandalf/gameboy.h>

#include "gui_context.h"
#include "settings.h"
#include "settings_window.h"
#include "views/gui_element.h"

namespace gui {
    class MainWindow : gandalf::PPU::VBlankListener, KeyboardHandler {
    public:
        MainWindow(GUIContext& context);
        ~MainWindow();

        bool Initialize();
        void Render();

    private:
        void HandleKey(std::int32_t key, bool pressed) override;

        void DockSpace();
        void MenuBar();

        void LoadROM(const std::filesystem::path& path);
        std::unique_ptr<gandalf::ROM> LoadBootROM(const std::filesystem::path& path);

        void OnVBlank() override;

        std::shared_ptr<gandalf::Gameboy> gameboy_;
        std::optional<gandalf::word> breakpoint_;
        bool step_;
        bool gb_pause_;
        bool gb_thread_run_;
        unsigned int gb_fps_;
        bool block_audio_;
        std::thread gb_thread_;

        std::string show_popup_;

        std::list<std::unique_ptr<GUIElement>> gui_elements_;
        std::unique_ptr<SettingsWindow> settings_window_;
        GUIContext& gui_context_;
    };
}
#endif