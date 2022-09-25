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
    const ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    const std::string kAppName = "Gandalf";
    const std::string kSettingsFileName = "settings.txt";
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

    MainWindow::MainWindow(std::filesystem::path boot_rom_path) :
        sdl_renderer_(nullptr),
        sdl_window_(nullptr),
        sdl_surface_(nullptr),
        sdl_texture_(nullptr),
        running_(false),
        show_debug_window_(true),
        step_(false),
        scale_(4),
        gb_pause_(false),
        block_audio_(true),
        boot_rom_path_(boot_rom_path),
        gb_thread_run_(false),
        gb_fps_(0),
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

        sdl_surface_ = SDL_CreateRGBSurfaceWithFormat(0, gandalf::kScreenWidth, gandalf::kScreenHeight, 16, SDL_PIXELFORMAT_BGR555);
        if (!sdl_surface_)
        {
            std::cerr << "Error creating SDL_Surface!" << std::endl;
            return false;
        }

        sdl_texture_ = SDL_CreateTexture(sdl_renderer_, SDL_PIXELFORMAT_BGR555,
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
        if (!gameboy_ || !show_debug_window_)
            return;

        ImGui::Begin("CPU", nullptr, ImGuiWindowFlags_NoTitleBar);
        ImGui::Checkbox("Pause", &gb_pause_);
        ImGui::Checkbox("Limit FPS", &block_audio_);
        ImGui::SameLine();

        if (!gb_pause_)
            ImGui::BeginDisabled();

        if (ImGui::Button("Step")) {
            step_ = true;
        }

        if (!gb_pause_)
            ImGui::EndDisabled();

        if (ImGui::BeginTable("Registers", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
        {
            gandalf::Registers registers = gameboy_->GetCPU().GetRegisters();
            ImGui::TableNextColumn();
            ImGui::Text("A : %02X", registers.a());
            ImGui::TableNextColumn();
            ImGui::Text("F : %02X", registers.f());
            ImGui::TableNextColumn();
            ImGui::Text("B : %02X", registers.b());
            ImGui::TableNextColumn();
            ImGui::Text("C : %02X", registers.c());
            ImGui::TableNextColumn();
            ImGui::Text("D : %02X", registers.d());
            ImGui::TableNextColumn();
            ImGui::Text("E : %02X", registers.e());
            ImGui::TableNextColumn();
            ImGui::Text("H : %02X", registers.h());
            ImGui::TableNextColumn();
            ImGui::Text("L : %02X", registers.l());

            ImGui::TableNextColumn();
            ImGui::Text("SP : %04X", registers.stack_pointer);
            ImGui::TableNextColumn();
            ImGui::Text("PC : %04X", registers.program_counter);

            ImGui::TableNextColumn();

            ImGui::EndTable();
        }

        ImGui::BeginDisabled();
        ImGui::Checkbox("IME", &gameboy_->GetCPU().GetRegisters().interrupt_master_enable);
        ImGui::EndDisabled();
        ImGui::End();

        ImGui::Begin("Memory", nullptr, ImGuiWindowFlags_NoTitleBar);

        // static bool follow_pc = false;
        // ImGui::Checkbox("Follow PC", &follow_pc);

        // if (*context.run)
        //     follow_pc = false;

        static char address[5];
        address[4] = '\0';
        ImGui::InputText("address", address, 5, ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase);
        ImGui::SameLine();

        std::optional<gandalf::word> scroll_target;
        bool should_scroll = false;
        if (ImGui::Button("Scroll to")) {
            auto address_value = std::strtoul(address, nullptr, 16);
            if (address_value < 0)
                address_value = 0;
            if (address_value > 0xFFFF)
                address_value = 0xFFFF;

            scroll_target = static_cast<gandalf::word>(address_value);
        }

        if (ImGui::BeginTable("Memory viewer", 17, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY | ImGuiTableFlags_SizingStretchProp)) {
            static float memory_item_height = 0.f;

            if (scroll_target)
                ImGui::SetScrollY((*scroll_target / 16) * memory_item_height);

            gandalf::Bus& bus = gameboy_->GetBus();
            ImGuiListClipper clipper;
            clipper.Begin(0x10000 / 16);
            while (clipper.Step())
            {
                for (int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++)
                {
                    gandalf::word address_start = line_no * 16;

                    ImGui::TableNextColumn();
                    ImGui::Text("%04X", address_start);
                    for (int column = 0; column < 16; column++)
                    {
                        const gandalf::word address = address_start + column;
                        ImGui::TableNextColumn();
                        ImGui::Text("%02X", gameboy_->GetBus().DebugRead(address));
                        if (ImGui::IsItemHovered()) {
                            ImGui::BeginTooltip();
                            ImGui::Text("%04X", address);
                            ImGui::EndTooltip();
                        }
                    }
                }

                if (clipper.ItemsHeight > 0) memory_item_height = clipper.ItemsHeight;
            }
            clipper.End();

            ImGui::EndTable();
        }

        ImGui::End();

        ImGui::Begin("Debugger", nullptr, ImGuiWindowFlags_NoTitleBar);
        gandalf::Bus& bus = gameboy_->GetBus();
        gandalf::Registers& registers = gameboy_->GetCPU().GetRegisters();


        if (ImGui::BeginTable("Debugger", 3, ImGuiTableFlags_ScrollY)) {
            static gandalf::word last_pc = registers.program_counter;
            static float debugger_item_height = 0.f;
            if (last_pc != registers.program_counter && debugger_item_height > 0) {
                ImGui::SetScrollY(registers.program_counter * debugger_item_height);
                last_pc = registers.program_counter;
            }

            ImGuiListClipper clipper;
            clipper.Begin(0x10000);
            while (clipper.Step())
            {
                for (int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++)
                {
                    // TODO: decode instructions, show name with operand values, group them (e.g. LD_RR_NN should combine into one line)
                    ImGui::TableNextRow();

                    if (line_no == registers.program_counter)
                        ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg1, ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 0.0f, 0.5f)));

                    ImGui::TableSetColumnIndex(0);

                    bool dummy = false;
                    std::string label = "##b" + std::to_string(line_no);
                    if (ImGui::Selectable(label.c_str(), breakpoint_ && *breakpoint_ == line_no)) {
                        if (breakpoint_ && *breakpoint_ == line_no)
                            breakpoint_ = std::nullopt;
                        else
                            breakpoint_ = line_no;
                    }

                    ImGui::TableSetColumnIndex(1);

                    ImGui::Text("%04X", line_no);

                    ImGui::TableSetColumnIndex(2);
                    ImGui::Text("%02X", bus.DebugRead(line_no));
                }

                if (clipper.ItemsHeight > 0) debugger_item_height = clipper.ItemsHeight;
            }
            clipper.End();

            ImGui::EndTable();
        }

        ImGui::End();

        ImGui::Begin("Cartridge");
        if (const auto& cartridge_ptr = gameboy_->GetCartridge())
        {
            if (cartridge_ptr->Loaded()) {
                const auto& header = cartridge_ptr->GetHeader();
                ImGui::Text("ROM loaded");
                ImGui::Text("Title: %s", header->GetTitleString().c_str());
                ImGui::Text("Manufacturer code: %s", header->GetManufacturerCodeString().c_str());
                ImGui::Text("Licensee: %s", header->GetLicenseeString().c_str());
                ImGui::Text("ROM Size: %s", header->GetROMSizeString().c_str());
                ImGui::Text("RAM Size: %s", header->GetRAMSizeString().c_str());
                ImGui::Text("CGB flag: %s", header->GetCGBFlagString().c_str());
                ImGui::Text("SGB flag: %s", header->GetSGBFlagString().c_str());
                ImGui::Text("Cartridge type: %s", header->GetTypeString().c_str());
                ImGui::Text("Destination: %s", header->GetDestinationString().c_str());
            }
        }
        else
            ImGui::TextUnformatted("No ROM loaded");
        ImGui::End();

        ImGui::Begin("Sound");
        static bool channel_enabled[4] = { true, true, true, true };
        if (ImGui::Checkbox("Square wave 1", &channel_enabled[0]))
            gameboy_->GetAPU().MuteChannel(0, !channel_enabled[0]);
        if (ImGui::Checkbox("Square wave 2", &channel_enabled[1]))
            gameboy_->GetAPU().MuteChannel(1, !channel_enabled[1]);
        if (ImGui::Checkbox("Wave", &channel_enabled[2]))
            gameboy_->GetAPU().MuteChannel(2, !channel_enabled[2]);
        if (ImGui::Checkbox("Noise", &channel_enabled[3]))
            gameboy_->GetAPU().MuteChannel(3, !channel_enabled[3]);

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

            auto boot_rom = LoadBootROM(boot_rom_path_);
            if (!boot_rom && ImGui::BeginPopupModal("Error"))
            {
                ImGui::TextUnformatted("Could not load boot ROM!");
                ImGui::EndPopup();
                return;
            }

            std::unique_ptr<gandalf::Gameboy> gameboy = std::make_unique<gandalf::Gameboy>(*boot_rom, file);
            std::shared_ptr<SDLAudioHandler> handler = std::make_shared<SDLAudioHandler>(block_audio_, gb_thread_run_);
            gameboy->GetAPU().SetOutputHandler(handler);
            gameboy->GetPPU().SetVBlankListener(this);

            if (gameboy->Ready()) {
                gameboy_ = std::move(gameboy);
                gb_thread_run_ = true;
                gb_thread_ = std::thread(GameboyThread, gameboy_.get(), &gb_thread_run_, &gb_pause_, &step_, &breakpoint_);
            }
            else if (ImGui::BeginPopupModal("Error")) {
                ImGui::TextUnformatted("Could not load ROM!");
                ImGui::EndPopup();
            }
        }
    }

    std::unique_ptr<gandalf::Gameboy::BootROM> MainWindow::LoadBootROM(const std::filesystem::path& path)
    {
        std::ifstream input(path, std::ios::binary);
        if (input.fail())
            return {};

        std::vector<gandalf::byte> file = std::vector<gandalf::byte>(std::istreambuf_iterator<char>(input),
            std::istreambuf_iterator<char>());

        return std::make_unique<gandalf::Gameboy::BootROM>(file);
    }

    void MainWindow::OnVBlank()
    {
        std::swap(front_buffer_, back_buffer_);
        std::copy(gameboy_->GetLCD().GetVideoBuffer().begin(), gameboy_->GetLCD().GetVideoBuffer().end(), back_buffer_->begin());

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