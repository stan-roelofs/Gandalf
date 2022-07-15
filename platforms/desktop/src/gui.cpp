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

namespace {
    SDL_Renderer* renderer;
    SDL_Window* window;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    SDL_Surface* screen;
    SDL_Texture* texture;
    std::array<gandalf::byte, gandalf::kScreenHeight* gandalf::kScreenWidth * 3> buffer;
    ImGui::FileBrowser file_dialog;
    int scale = 4;
}

namespace gui
{
    void DebugWindow(Context& context)
    {
        if (!(*context.show_debug_window))
            return;

        ImGui::Begin("CPU", nullptr, ImGuiWindowFlags_NoTitleBar);
        ImGui::Checkbox("Run", context.run);
        ImGui::Checkbox("Limit FPS", context.limit_frames);
        ImGui::SameLine();

        if (*context.run)
            ImGui::BeginDisabled();

        if (ImGui::Button("Step")) {
            *context.step = true;
        }

        if (ImGui::BeginTable("Registers", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
        {
            gandalf::Registers registers = context.gameboy->GetCPU().GetRegisters();
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

        if (*context.run)
            ImGui::EndDisabled();

        ImGui::BeginDisabled();
        ImGui::Checkbox("IME", &context.gameboy->GetCPU().GetRegisters().interrupt_master_enable);
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

            gandalf::Bus& bus = context.gameboy->GetBus();
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
                        ImGui::Text("%02X", context.gameboy->GetBus().DebugRead(address));
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
        gandalf::Bus& bus = context.gameboy->GetBus();
        gandalf::Registers& registers = context.gameboy->GetCPU().GetRegisters();

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
                    if (ImGui::Selectable(label.c_str(), *context.breakpoint && **context.breakpoint == line_no)) {
                        if (context.breakpoint && *context.breakpoint == line_no)
                            *context.breakpoint = std::nullopt;
                        else
                            *context.breakpoint = line_no;
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
        std::shared_ptr<const gandalf::Cartridge::Header> header = std::move(context.gameboy->GetCartridge().GetHeader());
        if (header)
        {
            ImGui::Text("ROM loaded");
            ImGui::Text("Title: %s", header->GetTitle().c_str());
            ImGui::Text("Manufacturer code: %s", header->GetManufacturerCode().c_str());
            ImGui::Text("Licensee: %s", header->GetLicensee().c_str());
            ImGui::Text("ROM Size: %s", header->GetROMSize().c_str());
            ImGui::Text("RAM Size: %s", header->GetRAMSize().c_str());
            ImGui::Text("CGB flag: %s", header->GetCGBFlag().c_str());
            ImGui::Text("SGB flag: %s", header->GetSGBFlag().c_str());
            ImGui::Text("Cartridge type: %s", header->GetType().c_str());
            ImGui::Text("Destination: %s", header->GetDestination().c_str());
        }
        else
            ImGui::TextUnformatted("No ROM loaded");
        ImGui::End();
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

    bool SetupGUI()
    {
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
        {
            std::cerr << "Error: " << SDL_GetError() << std::endl;
            return false;
        }

        // Setup window
        SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
        window = SDL_CreateWindow("Gandalf", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);

        // Setup SDL_Renderer instance
        renderer = SDL_CreateRenderer(window, -1, /*SDL_RENDERER_PRESENTVSYNC |*/ SDL_RENDERER_ACCELERATED);
        if (renderer == NULL)
        {
            SDL_Log("Error creating SDL_Renderer!");
            return false;
        }
        SDL_RendererInfo info;
        SDL_GetRendererInfo(renderer, &info);
        SDL_Log("Current SDL_Renderer: %s", info.name);

        screen = SDL_CreateRGBSurfaceWithFormat(0, gandalf::kScreenWidth, gandalf::kScreenHeight, 16, SDL_PIXELFORMAT_BGR565);

        texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_BGR565,
            SDL_TEXTUREACCESS_STREAMING,
            gandalf::kScreenWidth, gandalf::kScreenHeight);

        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable;

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();

        ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
        ImGui_ImplSDLRenderer_Init(renderer);

        file_dialog.SetTitle("Choose ROM");
        file_dialog.SetTypeFilters({ ".gb", ".gbc" });

        return true;
    }

    void HandleKey(bool pressed, SDL_Keycode key, gandalf::Gameboy& gameboy)
    {
        auto& joypad = gameboy.GetJoypad();
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

    bool PollEvents(Context& context)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_KEYDOWN)
                HandleKey(true, event.key.keysym.sym, *context.gameboy);
            else if (event.type == SDL_KEYUP)
                HandleKey(false, event.key.keysym.sym, *context.gameboy);
            else if (event.type == SDL_QUIT)
                return true;
            else if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
                return true;
        }

        return false;
    }

    void RenderGUI(Context& context)
    {
        // Start the Dear ImGui frame
        ImGui_ImplSDLRenderer_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        ImGui::ShowDemoWindow();

        // If you strip some features of, this demo is pretty much equivalent to calling DockSpaceOverViewport()!
            // In most cases you should be able to just call DockSpaceOverViewport() and ignore all the code below!
            // In this specific demo, we are not using DockSpaceOverViewport() because:
            // - we allow the host window to be floating/moveable instead of filling the viewport (when opt_fullscreen == false)
            // - we allow the host window to have padding (when opt_padding == true)
            // - we have a local menu bar in the host window (vs. you could use BeginMainMenuBar() + DockSpaceOverViewport() in your code!)
            // TL;DR; this demo is more complicated than what you would normally use.
            // If we removed all the options we are showcasing, this demo would become:
            //     void ShowExampleAppDockSpace()
            //     {
            //         ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
            //     }

        static bool opt_fullscreen = true;
        static bool opt_padding = false;
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

        // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
        // because it would be confusing to have two docking targets within each others.
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        if (opt_fullscreen)
        {
            const ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        }
        else
        {
            dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
        }

        // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
        // and handle the pass-thru hole, so we ask Begin() to not render a background.
        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;

        // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
        // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
        // all active windows docked into it will lose their parent and become undocked.
        // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
        // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
        if (!opt_padding)
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("DockSpace Demo", nullptr, window_flags);
        if (!opt_padding)
            ImGui::PopStyleVar();

        if (opt_fullscreen)
            ImGui::PopStyleVar(2);

        // Submit the DockSpace
        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        }

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Open ROM", "Ctrl+O"))
                    file_dialog.Open();

                ImGui::EndMenu();

            }
            if (ImGui::BeginMenu("Options"))
            {
                // Disabling fullscreen would allow the window to be moved to the front of other windows,
                // which we can't undo at the moment without finer window depth/z control.
                ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen);
                ImGui::MenuItem("Padding", NULL, &opt_padding);
                ImGui::Separator();

                ImGui::MenuItem("Debug", nullptr, context.show_debug_window);

                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }

        ImGui::End();

        file_dialog.Display();
        if (file_dialog.HasSelected())
        {
            std::ifstream input(file_dialog.GetSelected(), std::ios::binary);
            if (input.fail())
                std::cout << "Failed to open file: " << file_dialog.GetSelected().string() << std::endl;
            else {
                std::vector<gandalf::byte> file = std::vector<gandalf::byte>(std::istreambuf_iterator<char>(input),
                    std::istreambuf_iterator<char>());
                context.gameboy->Load(file);
            }

            file_dialog.ClearSelected();
        }

        DebugWindow(context);

        ImGui::Begin("fps");
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();

        SDL_UpdateTexture(texture, nullptr, *context.video_buffer, screen->pitch);

        ImGui::Begin("Gameboy");
        ImGui::SliderInt("Scale", &scale, 1, 5);
        ImGui::Image(texture, ImVec2(gandalf::kScreenWidth * scale, gandalf::kScreenHeight * scale));
        ImGui::End();

        // Rendering
        ImGui::Render();
        SDL_SetRenderDrawColor(renderer, (Uint8)(clear_color.x * 255), (Uint8)(clear_color.y * 255), (Uint8)(clear_color.z * 255), (Uint8)(clear_color.w * 255));
        SDL_RenderClear(renderer);
        ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
        SDL_RenderPresent(renderer);
    }

    void DestroyGUI()
    {
        // Cleanup
        ImGui_ImplSDLRenderer_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();

        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }
}