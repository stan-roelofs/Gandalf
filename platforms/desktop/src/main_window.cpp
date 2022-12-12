#include "main_window.h"

#include <iostream>
#include <fstream>
#include <optional>

#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include <gandalf/constants.h>

#include <SDL.h>
#include <SDL_timer.h>
#include <SDL_opengl.h>
#include <nfd.hpp>

#include "audio_handler.h"
#include "settings_window.h"
#include "views/cpu_view.h"
#include "views/gameboy_view.h"
#include "views/io_view.h"
#include "views/vram_view.h"
#include "views/memory_view.h"
#include "views/cartridge_view.h"
#include "views/apu_view.h"
#include "text.h"

namespace {
    const ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    const std::string kSettingsFileName = "settings.json";
    const char* kDockSpaceName = "Gandalf";
    const unsigned int kWidth = 1280;
    const unsigned int kHeight = 720;
    const unsigned int kROMHistorySize = 10;

    const std::filesystem::path GetSettingsPath() { return std::filesystem::current_path() / kSettingsFileName; }
}

namespace gui
{
    void GameboyThread(gandalf::Gameboy* gb, const bool* thread_run, bool* gb_paused, bool* step, std::optional<gandalf::word>* breakpoint)
    {
        while (*thread_run)
        {
            if (*breakpoint && *breakpoint == gb->GetCPU().GetRegisters().program_counter)
                *gb_paused = true;

            if (!*gb_paused)
                gb->Run();
            else if (*step)
            {
                gb->Run();
                *step = false;
            }
        }
    }

    MainWindow::MainWindow() :
        sdl_gl_context_(nullptr),
        sdl_window_(nullptr),
        running_(false),
        step_(false),
        gb_pause_(false),
        gb_thread_run_(false),
        gb_fps_(0),
        block_audio_(true)
    {
        gui_context_.AddKeyboardHandler(this);
    }

    MainWindow::~MainWindow()
    {
        gb_thread_run_ = false;
        if (gb_thread_.joinable())
            gb_thread_.join();

        settings::Write(GetSettingsPath(), gui_context_.GetSettings());

        NFD::Quit();

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();

        SDL_GL_DeleteContext(sdl_gl_context_);
        SDL_DestroyWindow(sdl_window_);

        SDL_Quit();
    }

    bool MainWindow::Initialize()
    {
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
        {
            std::cerr << "Error: " << SDL_GetError() << std::endl;
            return false;
        }

        // Setup window
        SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_OPENGL);
        sdl_window_ = SDL_CreateWindow(text::Get(text::ID::kAppName), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 800, window_flags);
        assert(sdl_window_);
        sdl_gl_context_ = SDL_GL_CreateContext(sdl_window_);
        if (!sdl_gl_context_)
        {
            std::cerr << "Error: " << SDL_GetError() << std::endl;
            return false;
        }
        SDL_GL_MakeCurrent(sdl_window_, sdl_gl_context_);
        SDL_GL_SetSwapInterval(1); // enable vsync

        gui_elements_.push_back(std::move(std::make_unique<GameboyView>()));
        gui_elements_.push_back(std::move(std::make_unique<VRAMView>(gui_context_.GetSettings().show_debug)));
        gui_elements_.push_back(std::move(std::make_unique<CPUView>(gui_context_.GetSettings().show_debug, gb_pause_, block_audio_, step_, breakpoint_)));
        gui_elements_.push_back(std::move(std::make_unique<MemoryView>(gui_context_.GetSettings().show_debug)));
        gui_elements_.push_back(std::move(std::make_unique<CartridgeView>(gui_context_.GetSettings().show_debug)));
        gui_elements_.push_back(std::move(std::make_unique<APUView>(gui_context_.GetSettings().show_debug)));
        gui_elements_.push_back(std::move(std::make_unique<IOView>(gui_context_.GetSettings().show_debug)));

        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable;

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();

        ImGui_ImplSDL2_InitForOpenGL(sdl_window_, sdl_gl_context_);
        ImGui_ImplOpenGL3_Init();

        NFD::Init();
        if (!settings::Read(GetSettingsPath(), gui_context_.GetSettings()))
            std::cerr << "Error reading settings file!" << std::endl;

        SDL_SetWindowSize(sdl_window_, gui_context_.GetSettings().window_width, gui_context_.GetSettings().window_height);

        running_ = true;
        return true;
    }

    std::string SelectBootROM()
    {
        NFD::UniquePath path;
        auto result = NFD::OpenDialog(path);
        if (result == NFD_OKAY)
            return std::string(path.get());
        return "";
    }

    void MainWindow::Show()
    {
        while (running_)
        {
            HandleEvents();

            // Start frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplSDL2_NewFrame();
            ImGui::NewFrame();

            // Render our GUI elements
            DockSpace();
            //ImGui::ShowDemoWindow();


            for (auto& element : gui_elements_)
                element->Render();

            if (!show_popup_.empty())
            {
                ImGui::OpenPopup(show_popup_.c_str());
                show_popup_.clear();
            }

            if (settings_window_) {
                settings_window_->Show();
                gb_pause_ = true;
                if (settings_window_->Terminated()) {
                    settings_window_.reset();
                    gb_pause_ = false;
                }
            }

            if (ImGui::BeginPopupModal("Error##LoadBootROM", NULL, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::TextUnformatted(text::Get(text::ID::kErrorLoadBootROM));
                ImGui::Separator();

                if (ImGui::Button(text::Get(text::ID::kSelectBootROM), ImVec2(120, 0))) {
                    gui_context_.GetSettings().boot_rom_location = SelectBootROM();
                    ImGui::CloseCurrentPopup();
                }
                ImGui::SetItemDefaultFocus();
                ImGui::SameLine();
                if (ImGui::Button(text::Get(text::ID::kCancel), ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
                ImGui::EndPopup();
            }

            if (ImGui::BeginPopupModal("Error##LoadROM", NULL, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::TextUnformatted(text::Get(text::ID::kErrorLoad));
                ImGui::Separator();

                if (ImGui::Button("Ok", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
                ImGui::EndPopup();
            }

            if (ImGui::BeginPopupModal("Error##GameboyNotReady", NULL, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::TextUnformatted(text::Get(text::ID::kErrorLoad));
                ImGui::Separator();

                if (ImGui::Button(text::Get(text::ID::kOk), ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
                ImGui::EndPopup();
            }

            auto& io = ImGui::GetIO();

            // Now render everything
            ImGui::Render();
            glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
            glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
            {
                SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
                SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
                ImGui::UpdatePlatformWindows();
                ImGui::RenderPlatformWindowsDefault();
                SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
            }

            SDL_GL_SwapWindow(sdl_window_);
        }
    }

    void MainWindow::HandleEvents()
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);

            if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP)
            {
                if (auto handler = gui_context_.GetKeyboardHandler())
                    handler->HandleKey(event.key.keysym.sym, event.type == SDL_KEYDOWN);
            }
            else if (event.type == SDL_WINDOWEVENT && event.window.windowID == SDL_GetWindowID(sdl_window_))
            {
                switch (event.window.event)
                {
                case SDL_WINDOWEVENT_CLOSE:
                    running_ = false;
                    break;
                case SDL_WINDOWEVENT_RESIZED:
                    gui_context_.GetSettings().window_width = event.window.data1;
                    gui_context_.GetSettings().window_height = event.window.data2;
                    break;
                }
            }
            else if (event.type == SDL_QUIT)
                running_ = false;
        }
    }

    void MainWindow::HandleKey(std::int32_t key, bool pressed)
    {
        if (!gameboy_)
            return;

        auto& joypad = gameboy_->GetJoypad();

        const auto& settings = gui_context_.GetSettings();
        if (key == settings.key_a)
            joypad.SetButtonState(gandalf::Joypad::Button::kA, pressed);
        else if (key == settings.key_b)
            joypad.SetButtonState(gandalf::Joypad::Button::kB, pressed);
        else if (key == settings.key_start)
            joypad.SetButtonState(gandalf::Joypad::Button::kStart, pressed);
        else if (key == settings.key_select)
            joypad.SetButtonState(gandalf::Joypad::Button::kSelect, pressed);
        else if (key == settings.key_up)
            joypad.SetButtonState(gandalf::Joypad::Button::kUp, pressed);
        else if (key == settings.key_down)
            joypad.SetButtonState(gandalf::Joypad::Button::kDown, pressed);
        else if (key == settings.key_left)
            joypad.SetButtonState(gandalf::Joypad::Button::kLeft, pressed);
        else if (key == settings.key_right)
            joypad.SetButtonState(gandalf::Joypad::Button::kRight, pressed);
    }

    void MainWindow::DockSpace()
    {
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

        // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
        // because it would be confusing to have two docking targets within each others.
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
        // and handle the pass-thru hole, so we ask Begin() to not render a background.
        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin(kDockSpaceName, nullptr, window_flags);
        ImGui::PopStyleVar(3);

        // Submit the DockSpace
        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiID dockspace_id = ImGui::GetID(kDockSpaceName);

            ImVec2 dockspace_size(0.f, 0.f);
            ImGui::DockSpace(dockspace_id, ImVec2(0.f, 0.f), dockspace_flags);
        }

        MenuBar();

        ImGui::End();
    }

    void MainWindow::MenuBar()
    {
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu(text::Get(text::ID::kMenuFile)))
            {
                if (ImGui::MenuItem(text::Get(text::ID::kMenuFileOpenROM)))
                {
                    NFD::UniquePath path;
                    nfdfilteritem_t filter_item[] = { {"ROM", "gb,gbc"} };
                    auto result = NFD::OpenDialog(path, filter_item, 1);
                    if (result == NFD_OKAY)
                        LoadROM(path.get());
                }

                if (ImGui::BeginMenu(text::Get(text::ID::kMenuFileRecentROMs)))
                {
                    std::string rom_to_load;
                    for (const auto& path : gui_context_.GetSettings().recent_roms)
                    {
                        if (ImGui::MenuItem(path.c_str()))
                            rom_to_load = path;
                    }
                    ImGui::EndMenu();

                    if (!rom_to_load.empty())
                        LoadROM(rom_to_load);
                }

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu(text::Get(text::ID::kMenuSettings)))
            {
                if (ImGui::MenuItem(text::Get(text::ID::kMenuSettings)))
                    settings_window_ = std::make_unique<SettingsWindow>(gui_context_);

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu(text::Get(text::ID::kMenuEmulation)))
            {
                ImGui::MenuItem(text::Get(text::ID::kPause), nullptr, &gb_pause_);
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu(text::Get(text::ID::kMenuView)))
            {
                ImGui::MenuItem(text::Get(text::ID::kMenuViewDebug), nullptr, &gui_context_.GetSettings().show_debug);
                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }
    }

    void MainWindow::LoadROM(const std::filesystem::path& path)
    {
        gb_thread_run_ = false;
        if (gb_thread_.joinable())
            gb_thread_.join();

        auto& settings = gui_context_.GetSettings();
        auto it = std::find(settings.recent_roms.begin(), settings.recent_roms.end(), path);
        if (it != settings.recent_roms.end())
            settings.recent_roms.erase(it);
        settings.recent_roms.push_front(path.string());

        if (settings.recent_roms.size() > kROMHistorySize)
            settings.recent_roms.pop_back();

        std::ifstream input(path, std::ios::binary);
        if (input.fail()) {
            show_popup_ = "Error##LoadROM";
            return;
        }

        std::vector<gandalf::byte> file = std::vector<gandalf::byte>(std::istreambuf_iterator<char>(input),
            std::istreambuf_iterator<char>());

        auto boot_rom = LoadBootROM(settings.boot_rom_location);
        if (!boot_rom)
        {
            show_popup_ = "Error##LoadBootROM";
            return;
        }

        std::shared_ptr<SDLAudioHandler> handler = std::make_shared<SDLAudioHandler>(block_audio_, gb_thread_run_);
        std::shared_ptr<gandalf::Gameboy> gameboy = std::make_shared<gandalf::Gameboy>(*boot_rom, file, handler);
        gameboy->GetPPU().AddVBlankListener(this);

        if (!gameboy->Ready()) {
            show_popup_ = "Error##GameboyNotReady";
            return;
        }

        gameboy_ = std::move(gameboy);

        for (auto& element : gui_elements_)
            element->SetGameboy(gameboy_);

        gb_thread_run_ = true;
        gb_thread_ = std::thread(GameboyThread, gameboy_.get(), &gb_thread_run_, &gb_pause_, &step_, &breakpoint_);
    }

    std::unique_ptr<gandalf::ROM> MainWindow::LoadBootROM(const std::filesystem::path& path)
    {
        std::ifstream input(path, std::ios::binary);
        if (input.fail())
            return {};

        return std::make_unique<gandalf::ROM>(std::vector<gandalf::byte>(std::istreambuf_iterator<char>(input),
            std::istreambuf_iterator<char>()));
    }

    void MainWindow::OnVBlank()
    {
        ++gb_fps_;

        using namespace std::chrono;

        static high_resolution_clock::time_point t1 = high_resolution_clock::now();;

        high_resolution_clock::time_point t2 = high_resolution_clock::now();

        duration<double, std::milli> time_span = t2 - t1;

        if (time_span.count() > 1000) {
            t1 = t2;
            std::cout << std::to_string(gb_fps_) << std::endl;
            gb_fps_ = 0;
        }
    }
}
