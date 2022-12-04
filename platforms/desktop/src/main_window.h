#ifndef __GB_APP_MAIN_WINDOW_H
#define __GB_APP_MAIN_WINDOW_H

#include <optional>
#include <filesystem>
#include <thread>

#include <gandalf/gameboy.h>

#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_sdlrenderer.h>

#include <SDL.h>

#include "gui_context.h"
#include "settings.h"
#include "settings_window.h"
#include "views/gui_element.h"

namespace gui {
    class MainWindow : gandalf::PPU::VBlankListener, KeyboardHandler {
    public:
        MainWindow();
        ~MainWindow();

        bool Initialize();
        void Show();

    private:
        void HandleEvents();

        void HandleKey(std::int32_t key, bool pressed) override;

        void DockSpace();
        void MenuBar();

        void LoadROM(const std::filesystem::path& path);
        std::unique_ptr<gandalf::ROM> LoadBootROM(const std::filesystem::path& path);

        void OnVBlank() override;

        SDL_Renderer* sdl_renderer_;
        SDL_Window* sdl_window_;
        std::shared_ptr<gandalf::Gameboy> gameboy_;
        bool running_;
        std::optional<gandalf::word> breakpoint_;
        bool step_;
        int scale_;
        bool gb_pause_;
        bool gb_thread_run_;
        unsigned int gb_fps_;
        bool block_audio_;
        std::thread gb_thread_;

        std::string show_popup_;

        std::list<std::unique_ptr<GUIElement>> gui_elements_;
        std::unique_ptr<SettingsWindow> settings_window_;
        GUIContext gui_context_;
    };
}
#endif