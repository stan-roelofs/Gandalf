#ifndef __GB_APP_GUI_H
#define __GB_APP_GUI_H

#include <optional>
#include <filesystem>
#include <thread>

#include <gandalf/gameboy.h>

#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_sdlrenderer.h>
#include "imfilebrowser.h"

#include <SDL.h>

#include "settings.h"

namespace gui {
    class MainWindow : gandalf::PPU::VBlankListener {
    public:
        MainWindow(std::filesystem::path boot_rom_path);
        ~MainWindow();

        bool Initialize();
        void Run();

    private:
        void HandleEvents();
        void HandleKey(SDL_Keycode key, bool pressed);

        void DockSpace();
        void MenuBar();
        void GameboyView();
        void DebugView();
        void VRAMViewer();

        void LoadROM(const std::filesystem::path& path);
        std::unique_ptr<gandalf::ROM> LoadBootROM(const std::filesystem::path& path);

        void OnVBlank() override;

        SDL_Renderer* sdl_renderer_;
        SDL_Window* sdl_window_;
        SDL_Texture* sdl_texture_;
        SDL_Texture* debug_texture_;
        ImGui::FileBrowser file_dialog_;
        std::unique_ptr<gandalf::Gameboy> gameboy_;
        settings::Settings settings_;
        bool running_;
        std::optional<gandalf::word> breakpoint_;
        bool show_debug_window_;
        bool step_;
        int scale_;
        bool gb_pause_;
        bool gb_thread_run_;
        unsigned int gb_fps_;
        bool block_audio_;
        std::filesystem::path boot_rom_path_;
        std::thread gb_thread_;

        std::unique_ptr<gandalf::LCD::VideoBuffer> front_buffer_;
        std::unique_ptr<gandalf::LCD::VideoBuffer> back_buffer_;
    };
}
#endif