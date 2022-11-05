#include "gui.h"

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
#include "views/cpu_view.h"
#include "views/gameboy_view.h"
#include "views/vram_view.h"
#include "views/memory_view.h"
#include "views/cartridge_view.h"
#include "views/apu_view.h"


namespace {
    const ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    const std::string kAppName = "Gandalf";
    const std::string kSettingsFileName = "settings.json";
    const unsigned int kWidth = 1280;
    const unsigned int kHeight = 720;
    const unsigned int kROMHistorySize = 10;

    const char* kDockSpaceName = "Gandalf";
    const char* kGameboyNodeName = "Gameboy";
    const char* kCPUNodeName = "CPU";
    const char* kMemoryNodeName = "Memory";
    const char* kAPUNodeName = "APU";
    const char* kPPUNodeName = "PPU";
    const char* kCartridgeNodeName = "Cartridge";

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

    MainWindow::MainWindow():
        sdl_renderer_(nullptr),
        sdl_window_(nullptr),
        running_(false),
        step_(false),
        scale_(4),
        gb_pause_(false),
        block_audio_(true),
        gb_thread_run_(false),
        gb_fps_(0),
        update_layout_(true)
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
        sdl_window_ = SDL_CreateWindow(kAppName.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);

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

    void MainWindow::Run()
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

            if (!show_error_.empty())
            {
                ImGui::OpenPopup(show_error_.c_str());
                show_error_.clear();
            }

            if (ImGui::BeginPopupModal("Error##LoadBootROM", NULL, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::Text("Error - could not load boot ROM");
                ImGui::Separator();

                if (ImGui::Button("Select file", ImVec2(120, 0))) {
                    settings_.boot_rom_location = SelectBootROM();
                    ImGui::CloseCurrentPopup();
                }
                ImGui::SetItemDefaultFocus();
                ImGui::SameLine();
                if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
                ImGui::EndPopup();
            }

            if (ImGui::BeginPopupModal("Error##LoadROM", NULL, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::Text("Error - could not load ROM");
                ImGui::Separator();

                if (ImGui::Button("Ok", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
                ImGui::EndPopup();
            }

            if (ImGui::BeginPopupModal("Error##GameboyNotReady", NULL, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::Text("Error - gameboy not ready");
                ImGui::Separator();

                if (ImGui::Button("Ok", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
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
        switch (key)
        {
        case SDLK_z: joypad.ButtonEvent(gandalf::Joypad::Button::kA, pressed); break;
        case SDLK_x: joypad.ButtonEvent(gandalf::Joypad::Button::kB, pressed); break;
        case SDLK_TAB: joypad.ButtonEvent(gandalf::Joypad::Button::kSelect, pressed); break;
        case SDLK_RETURN: joypad.ButtonEvent(gandalf::Joypad::Button::kStart, pressed); break;
        case SDLK_LEFT: joypad.ButtonEvent(gandalf::Joypad::Button::kLeft, pressed); break;
        case SDLK_RIGHT: joypad.ButtonEvent(gandalf::Joypad::Button::kRight, pressed); break;
        case SDLK_UP: joypad.ButtonEvent(gandalf::Joypad::Button::kUp, pressed); break;
        case SDLK_DOWN: joypad.ButtonEvent(gandalf::Joypad::Button::kDown, pressed); break;
        }
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
            if (settings_.auto_layout && update_layout_)
            {
                if (settings_.show_debug) {
                    scale_ = 2;
                    SDL_SetWindowSize(sdl_window_, kWidth, kHeight);
                    dockspace_size = ImVec2(kWidth, kHeight);
                }
                else {
                    scale_ = 4;
                    SDL_SetWindowSize(sdl_window_, gandalf::kScreenWidth * scale_, kHeight);
                    dockspace_size = ImVec2(gandalf::kScreenWidth * scale_, kHeight);
                }
            }

            ImGui::DockSpace(dockspace_id, dockspace_size, dockspace_flags);

            if (settings_.auto_layout && update_layout_)
            {
                update_layout_ = false;

                ImGui::DockBuilderRemoveNode(dockspace_id); // clear any previous layout
                ImGui::DockBuilderAddNode(dockspace_id, dockspace_flags | ImGuiDockNodeFlags_DockSpace);
                ImGui::DockBuilderSetNodeSize(dockspace_id, dockspace_size);

                if (settings_.show_debug)
                {
                    ImGuiID left_half, right_half;
                    ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.25f, &left_half, &right_half);
                    ImGui::DockBuilderDockWindow(kGameboyNodeName, left_half);
                    scale_ = 2;

                    // we now dock our windows into the docking node we made above
                    ImGui::DockBuilderDockWindow(kCPUNodeName, right_half);
                    ImGui::DockBuilderDockWindow(kCartridgeNodeName, right_half);
                    ImGui::DockBuilderDockWindow(kAPUNodeName, right_half);
                    ImGui::DockBuilderDockWindow(kMemoryNodeName, right_half);
                    ImGui::DockBuilderDockWindow(kPPUNodeName, right_half);
                }
                else {
                    ImGui::DockBuilderDockWindow(kGameboyNodeName, dockspace_id);
                }

                ImGui::DockBuilderFinish(dockspace_id);
            }
        }

        MenuBar();

        ImGui::End();
    }

    void MainWindow::MenuBar()
    {
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Open ROM", "Ctrl+O"))
                {
                    NFD::UniquePath path;
                    nfdfilteritem_t filter_item[] = { {"ROM", "gb,gbc"} };
                    auto result = NFD::OpenDialog(path, filter_item, 1);
                    if (result == NFD_OKAY)
                        LoadROM(path.get());
                }

                if (ImGui::BeginMenu("Recent ROMs"))
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

            if (ImGui::BeginMenu("Gameboy"))
            {
                ImGui::MenuItem("Pause", nullptr, &gb_pause_);
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("View"))
            {
                if (ImGui::MenuItem("Debug", nullptr, &settings_.show_debug))
                    update_layout_ = true;
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
            show_error_ = "Error##LoadROM";
            return;
        }

        std::vector<gandalf::byte> file = std::vector<gandalf::byte>(std::istreambuf_iterator<char>(input),
            std::istreambuf_iterator<char>());

        auto boot_rom = LoadBootROM(settings_.boot_rom_location);
        if (!boot_rom)
        {
            show_error_ = "Error##LoadBootROM";
            return;
        }

        std::shared_ptr<SDLAudioHandler> handler = std::make_shared<SDLAudioHandler>(block_audio_, gb_thread_run_);
        std::shared_ptr<gandalf::Gameboy> gameboy = std::make_shared<gandalf::Gameboy>(*boot_rom, file, handler);
        gameboy->GetPPU().AddVBlankListener(this);

        if (!gameboy->Ready()) {
            show_error_ = "Error##GameboyNotReady";
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
