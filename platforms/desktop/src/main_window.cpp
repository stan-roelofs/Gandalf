#include "main_window.h"

#include <iostream>
#include <fstream>
#include <optional>

#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_sdlrenderer.h>
#include <imgui_internal.h>

#include <gandalf/constants.h>

#include <SDL.h>
#include <SDL_timer.h>

#include <nfd.hpp>

#include "audio_handler.h"
#include "settings_window.h"
#include "views/cpu_view.h"
#include "views/gameboy_view.h"
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
        sdl_renderer_(nullptr),
        sdl_window_(nullptr),
        running_(false),
        step_(false),
        scale_(4),
        gb_pause_(false),
        block_audio_(true),
        gb_thread_run_(false),
        gb_fps_(0)
    {
    }

    MainWindow::~MainWindow()
    {
        gb_thread_run_ = false;
        if (gb_thread_.joinable())
            gb_thread_.join();

        settings::Write(GetSettingsPath(), settings_);

        NFD::Quit();

        ImGui_ImplSDLRenderer_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();

        SDL_DestroyRenderer(sdl_renderer_);
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
        SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
        sdl_window_ = SDL_CreateWindow(text::Get(text::ID::kAppName), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);

        // Setup SDL_Renderer instance
        sdl_renderer_ = SDL_CreateRenderer(sdl_window_, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
        if (!sdl_renderer_)
        {
            std::cerr << "Error creating SDL_Renderer!" << std::endl;
            return false;
        }
        SDL_RendererInfo info;
        SDL_GetRendererInfo(sdl_renderer_, &info);
        SDL_Log("Current SDL_Renderer: %s", info.name);

        gui_elements_.push_back(std::move(std::make_unique<GameboyView>(*sdl_renderer_, scale_)));
        gui_elements_.push_back(std::move(std::make_unique<VRAMView>(settings_.show_debug, *sdl_renderer_)));
        gui_elements_.push_back(std::move(std::make_unique<CPUView>(settings_.show_debug, gb_pause_, block_audio_, step_, breakpoint_)));
        gui_elements_.push_back(std::move(std::make_unique<MemoryView>(settings_.show_debug)));
        gui_elements_.push_back(std::move(std::make_unique<CartridgeView>(settings_.show_debug)));
        gui_elements_.push_back(std::move(std::make_unique<APUView>(settings_.show_debug)));


        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable;

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();

        ImGui_ImplSDL2_InitForSDLRenderer(sdl_window_, sdl_renderer_);
        ImGui_ImplSDLRenderer_Init(sdl_renderer_);

        NFD::Init();
        if (!settings::Read(GetSettingsPath(), settings_))
            std::cerr << "Error reading settings file!" << std::endl;

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
            ImGui_ImplSDLRenderer_NewFrame();
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
                if (settings_window_->Terminated())
                    settings_window_.release();
            }

            if (ImGui::BeginPopupModal("Error##LoadBootROM", NULL, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::Text(text::Get(text::ID::kErrorLoadBootROM));
                ImGui::Separator();

                if (ImGui::Button(text::Get(text::ID::kSelectBootROM), ImVec2(120, 0))) {
                    settings_.boot_rom_location = SelectBootROM();
                    ImGui::CloseCurrentPopup();
                }
                ImGui::SetItemDefaultFocus();
                ImGui::SameLine();
                if (ImGui::Button(text::Get(text::ID::kCancel), ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
                ImGui::EndPopup();
            }

            if (ImGui::BeginPopupModal("Error##LoadROM", NULL, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::Text(text::Get(text::ID::kErrorLoad));
                ImGui::Separator();

                if (ImGui::Button("Ok", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
                ImGui::EndPopup();
            }

            if (ImGui::BeginPopupModal("Error##GameboyNotReady", NULL, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::Text(text::Get(text::ID::kErrorLoad));
                ImGui::Separator();

                if (ImGui::Button(text::Get(text::ID::kOk), ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
                ImGui::EndPopup();
            }

            // Now render everything
            ImGui::Render();
            SDL_SetRenderDrawColor(sdl_renderer_, (Uint8)(clear_color.x * 255), (Uint8)(clear_color.y * 255), (Uint8)(clear_color.z * 255), (Uint8)(clear_color.w * 255));
            SDL_RenderClear(sdl_renderer_);
            ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
            SDL_RenderPresent(sdl_renderer_);
        }
    }

    void MainWindow::HandleEvents()
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);

            if (event.type == SDL_KEYDOWN)
                HandleKey(event.key.keysym.sym, true);
            else if (event.type == SDL_KEYUP)
                HandleKey(event.key.keysym.sym, false);
            else if (event.type == SDL_QUIT || (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(sdl_window_)))
                running_ = false;
        }
    }

    void MainWindow::HandleKey(SDL_Keycode key, bool pressed)
    {
        if (!gameboy_)
            return;

        auto& joypad = gameboy_->GetJoypad();

        if (key == static_cast<SDL_Keycode>(settings_.key_a))
            joypad.ButtonEvent(gandalf::Joypad::Button::kA, pressed);
        else if (key == static_cast<SDL_Keycode>(settings_.key_b))
            joypad.ButtonEvent(gandalf::Joypad::Button::kB, pressed);
        else if (key == static_cast<SDL_Keycode>(settings_.key_start))
            joypad.ButtonEvent(gandalf::Joypad::Button::kStart, pressed);
        else if (key == static_cast<SDL_Keycode>(settings_.key_select))
            joypad.ButtonEvent(gandalf::Joypad::Button::kSelect, pressed);
        else if (key == static_cast<SDL_Keycode>(settings_.key_up))
            joypad.ButtonEvent(gandalf::Joypad::Button::kUp, pressed);
        else if (key == static_cast<SDL_Keycode>(settings_.key_down))
            joypad.ButtonEvent(gandalf::Joypad::Button::kDown, pressed);
        else if (key == static_cast<SDL_Keycode>(settings_.key_left))
            joypad.ButtonEvent(gandalf::Joypad::Button::kLeft, pressed);
        else if (key == static_cast<SDL_Keycode>(settings_.key_right))
            joypad.ButtonEvent(gandalf::Joypad::Button::kRight, pressed);
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
                    for (const auto& path : settings_.recent_roms)
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
                    settings_window_ = std::make_unique<SettingsWindow>(settings_);

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu(text::Get(text::ID::kMenuEmulation)))
            {
                ImGui::MenuItem(text::Get(text::ID::kPause), nullptr, &gb_pause_);
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu(text::Get(text::ID::kMenuView)))
            {
                ImGui::MenuItem(text::Get(text::ID::kMenuViewDebug), nullptr, &settings_.show_debug);
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

        auto it = std::find(settings_.recent_roms.begin(), settings_.recent_roms.end(), path);
        if (it != settings_.recent_roms.end())
            settings_.recent_roms.erase(it);
        settings_.recent_roms.push_front(path.string());

        if (settings_.recent_roms.size() > kROMHistorySize)
            settings_.recent_roms.pop_back();

        settings::Write(GetSettingsPath(), settings_);

        std::ifstream input(path, std::ios::binary);
        if (input.fail()) {
            show_popup_ = "Error##LoadROM";
            return;
        }

        std::vector<gandalf::byte> file = std::vector<gandalf::byte>(std::istreambuf_iterator<char>(input),
            std::istreambuf_iterator<char>());

        auto boot_rom = LoadBootROM(settings_.boot_rom_location);
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
