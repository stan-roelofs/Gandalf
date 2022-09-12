#include "gui.h"

#include <iostream>
#include <fstream>
#include <optional>

#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_sdlrenderer.h>
#include "imfilebrowser.h"

#include <SDL.h>
#include <SDL_timer.h>

#include <gandalf/constants.h>
#include "audio_handler.h"

namespace {
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    int scale = 4;
    const std::string kAppName = "Gandalf";
    const std::string kSettingsFileName = "settings.txt";
}

namespace gui
{
    void DebugWindow(Context& context)
    {
        
    }

    void HelpMarker(const char* desc)
    {
        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
            ImGui::TextUnformatted(desc);
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }
    }



    //void RenderGUI(Context& context)
    //{
    //    // Start the Dear ImGui frame
    //    ImGui_ImplSDLRenderer_NewFrame();
    //    ImGui_ImplSDL2_NewFrame();
    //    ImGui::NewFrame();

    //    ImGui::ShowDemoWindow();

    //    // If you strip some features of, this demo is pretty much equivalent to calling DockSpaceOverViewport()!
    //        // In most cases you should be able to just call DockSpaceOverViewport() and ignore all the code below!
    //        // In this specific demo, we are not using DockSpaceOverViewport() because:
    //        // - we allow the host window to be floating/moveable instead of filling the viewport (when opt_fullscreen == false)
    //        // - we allow the host window to have padding (when opt_padding == true)
    //        // - we have a local menu bar in the host window (vs. you could use BeginMainMenuBar() + DockSpaceOverViewport() in your code!)
    //        // TL;DR; this demo is more complicated than what you would normally use.
    //        // If we removed all the options we are showcasing, this demo would become:
    //        //     void ShowExampleAppDockSpace()
    //        //     {
    //        //         ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
    //        //     }

    //    static bool opt_fullscreen = true;
    //    static bool opt_padding = false;
    //    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

    //    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
    //    // because it would be confusing to have two docking targets within each others.
    //    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    //    if (opt_fullscreen)
    //    {
    //        const ImGuiViewport* viewport = ImGui::GetMainViewport();
    //        ImGui::SetNextWindowPos(viewport->WorkPos);
    //        ImGui::SetNextWindowSize(viewport->WorkSize);
    //        ImGui::SetNextWindowViewport(viewport->ID);
    //        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    //        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    //        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    //        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    //    }
    //    else
    //    {
    //        dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
    //    }

    //    // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
    //    // and handle the pass-thru hole, so we ask Begin() to not render a background.
    //    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
    //        window_flags |= ImGuiWindowFlags_NoBackground;

    //    // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
    //    // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
    //    // all active windows docked into it will lose their parent and become undocked.
    //    // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
    //    // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
    //    if (!opt_padding)
    //        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    //    ImGui::Begin("DockSpace Demo", nullptr, window_flags);
    //    if (!opt_padding)
    //        ImGui::PopStyleVar();

    //    if (opt_fullscreen)
    //        ImGui::PopStyleVar(2);

    //    // Submit the DockSpace
    //    ImGuiIO& io = ImGui::GetIO();
    //    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    //    {
    //        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
    //        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    //    }

    //    if (ImGui::BeginMenuBar())
    //    {
    //        if (ImGui::BeginMenu("File"))
    //        {
    //            if (ImGui::MenuItem("Open ROM", "Ctrl+O"))
    //                file_dialog.Open();

    //            ImGui::EndMenu();

    //        }
    //        if (ImGui::BeginMenu("Options"))
    //        {
    //            // Disabling fullscreen would allow the window to be moved to the front of other windows,
    //            // which we can't undo at the moment without finer window depth/z control.
    //            ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen);
    //            ImGui::MenuItem("Padding", NULL, &opt_padding);
    //            ImGui::Separator();

    //            ImGui::MenuItem("Debug", nullptr, context.show_debug_window);

    //            ImGui::EndMenu();
    //        }

    //        ImGui::EndMenuBar();
    //    }

    //    ImGui::End();

    //    file_dialog.Display();
    //    if (file_dialog.HasSelected())
    //    {
    //        LoadROM(*context.gameboy, file_dialog.GetSelected());

    //        file_dialog.ClearSelected();
    //    }

    //    DebugWindow(context);

    //    ImGui::Begin("fps");
    //    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    //    ImGui::End();

    //    SDL_UpdateTexture(texture, nullptr, *context.video_buffer, screen->pitch);



    //    // Rendering
    //    ImGui::Render();
    //    SDL_SetRenderDrawColor(renderer, (Uint8)(clear_color.x * 255), (Uint8)(clear_color.y * 255), (Uint8)(clear_color.z * 255), (Uint8)(clear_color.w * 255));
    //    SDL_RenderClear(renderer);
    //    ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
    //    SDL_RenderPresent(renderer);
    //}



    void GameboyThread(gandalf::Gameboy* gb, const bool* thread_run, const bool* gb_paused)
    {
        while (*thread_run)
        {
            if (!*gb_paused)
                gb->Run();
        }
    }

    MainWindow::MainWindow(std::filesystem::path boot_rom_path) :
        sdl_renderer_(nullptr),
        sdl_window_(nullptr),
        sdl_surface_(nullptr),
        sdl_texture_(nullptr),
        running_(false),
        show_debug_window_(false),
        scale_(4),
        gb_pause_(false),
        block_audio_(true),
        boot_rom_path_(boot_rom_path),
        gb_thread_run_(false),
        back_buffer_(std::make_unique<gandalf::LCD::VideoBuffer>()),
        front_buffer_(std::make_unique < gandalf::LCD::VideoBuffer>())
    {
    }

    MainWindow::~MainWindow()
    {
        gb_thread_run_ = false;
        if (gb_thread_.joinable())
            gb_thread_.join();

        settings::Write(std::filesystem::current_path() / kSettingsFileName, settings_);

        ImGui_ImplSDLRenderer_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();

        SDL_DestroyRenderer(sdl_renderer_);
        SDL_DestroyWindow(sdl_window_);

        SDL_DestroyTexture(sdl_texture_);
        SDL_FreeSurface(sdl_surface_);

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

        sdl_surface_ = SDL_CreateRGBSurfaceWithFormat(0, gandalf::kScreenWidth, gandalf::kScreenHeight, 16, SDL_PIXELFORMAT_BGR565);
        if (!sdl_surface_)
        {
            std::cerr << "Error creating SDL_Surface!" << std::endl;
            return false;
        }

        sdl_texture_ = SDL_CreateTexture(sdl_renderer_, SDL_PIXELFORMAT_BGR565,
            SDL_TEXTUREACCESS_STREAMING,
            gandalf::kScreenWidth, gandalf::kScreenHeight);

        if (!sdl_texture_)
        {
            std::cerr << "Error creating SDL_Texture" << std::endl;
            return false;
        }

        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable;

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();

        ImGui_ImplSDL2_InitForSDLRenderer(sdl_window_, sdl_renderer_);
        ImGui_ImplSDLRenderer_Init(sdl_renderer_);

        file_dialog_.SetTitle("Choose ROM");
        file_dialog_.SetTypeFilters({ ".gb", ".gbc" });

        settings_ = settings::Read(std::filesystem::current_path() / kSettingsFileName);

        running_ = true;
        return true;
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
            MenuBar();
            GameboyView();
            DebugView();
            //ImGui::ShowDemoWindow();

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

    void MainWindow::GameboyView()
    {
        SDL_UpdateTexture(sdl_texture_, nullptr, front_buffer_.get(), sdl_surface_->pitch);

        ImGui::Begin("Gameboy");
        ImGui::SliderInt("Scale", &scale_, 1, 5);
        ImGui::Image(sdl_texture_, ImVec2(gandalf::kScreenWidth * scale_, gandalf::kScreenHeight * scale_));
        ImGui::End();
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
        ImGui::Begin("Gandalf", nullptr, window_flags);
        ImGui::PopStyleVar(3);

        // Submit the DockSpace
        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiID dockspace_id = ImGui::GetID("Gandalf");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        }
    }

    void MainWindow::MenuBar()
    {
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Open ROM", "Ctrl+O"))
                    file_dialog_.Open();

                if (ImGui::BeginMenu("Recent ROMs"))
                {
                    for (const auto& path : settings_.recent_roms)
                    {
                        if (ImGui::MenuItem(path.c_str())) {
                            LoadROM(path);
                        }
                    }
                    ImGui::EndMenu();
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
                ImGui::MenuItem("Debug", nullptr, &show_debug_window_);
                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }

        ImGui::End();

        file_dialog_.Display();
        if (file_dialog_.HasSelected())
        {
            LoadROM(file_dialog_.GetSelected());
            file_dialog_.ClearSelected();
        }
    }

    void MainWindow::DebugView()
    {
        ImGui::Begin("debug");

        ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);

        ImGui::Checkbox("Limit FPS", &block_audio_);


        ImGui::End();
    }

    void MainWindow::LoadROM(const std::filesystem::path& path)
    {
        gb_thread_run_ = false;
        if (gb_thread_.joinable())
            gb_thread_.join();

        auto it = std::find(settings_.recent_roms.begin(), settings_.recent_roms.end(), path);
        if (it != settings_.recent_roms.end())
            std::swap(*settings_.recent_roms.begin(), *it);
        else
            settings_.recent_roms.push_front(path.string());

        std::ifstream input(path, std::ios::binary);
        if (input.fail()) {
            if (ImGui::BeginPopupModal("Error")) {
                ImGui::TextUnformatted("Could not open file!");
                ImGui::EndPopup();
            }
        }
        else {
            std::vector<gandalf::byte> file = std::vector<gandalf::byte>(std::istreambuf_iterator<char>(input),
                std::istreambuf_iterator<char>());

            gameboy_ = std::make_unique<gandalf::Gameboy>();
            std::shared_ptr<SDLAudioHandler> handler = std::make_shared<SDLAudioHandler>(block_audio_);
            gameboy_->GetAPU().SetOutputHandler(handler);
            gameboy_->GetPPU().SetVBlankListener(this);

            auto boot_rom = LoadBootROM(boot_rom_path_);
            if (!boot_rom && ImGui::BeginPopupModal("Error"))
            {
                ImGui::TextUnformatted("Could not load boot ROM!");
                ImGui::EndPopup();
                return;
            }

            gameboy_->LoadBootROM(*boot_rom);
            if (!gameboy_->Load(file)) {
                if (ImGui::BeginPopupModal("Error")) {
                    ImGui::TextUnformatted("Could not load ROM!");
                    ImGui::EndPopup();
                }
            }

            gb_thread_run_ = true;
            gb_thread_ = std::thread(GameboyThread, gameboy_.get(), &gb_thread_run_, &gb_pause_);
        }
    }

    std::unique_ptr<gandalf::Gameboy::BootROM> MainWindow::LoadBootROM(const std::filesystem::path& path)
    {
        std::ifstream input(path, std::ios::binary);
        if (input.fail())
            return {};

        std::vector<gandalf::byte> file = std::vector<gandalf::byte>(std::istreambuf_iterator<char>(input),
            std::istreambuf_iterator<char>());

        auto boot_rom = std::make_unique<gandalf::Gameboy::BootROM>();
        if (file.size() < boot_rom->size())
            return {};
        else if (file.size() > boot_rom->size())
            std::cout << "Warning: unexpected boot ROM size (" << file.size() << " bytes)" << std::endl;

        std::copy(file.begin(), file.end(), boot_rom->begin());
        return boot_rom;
    }

    void MainWindow::OnVBlank()
    {
        std::swap(front_buffer_, back_buffer_);
        std::copy(gameboy_->GetLCD().GetVideoBuffer().begin(), gameboy_->GetLCD().GetVideoBuffer().end(), back_buffer_->begin());

        static int frames = 0;
        // Some computation here
        ++frames;

        using namespace std::chrono;

        static high_resolution_clock::time_point t1 = high_resolution_clock::now();;

        high_resolution_clock::time_point t2 = high_resolution_clock::now();

        duration<double, std::milli> time_span = t2 - t1;

        if (time_span.count() > 1000) {
            t1 = t2;
            std::cout << std::to_string(frames) << std::endl;
            frames = 0;
        }
    }
}