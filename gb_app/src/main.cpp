#include <cstring>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string>

#include <SDL.h>

#include <gandalf/gameboy.h>
#include <gandalf/cartridge.h>

#include "gui.h"

namespace {

}

static bool ReadFile(std::string filename, std::vector<gandalf::byte>& buffer) {
    std::ifstream input(filename, std::ios::binary);
    if (input.fail()) {
        return false;
    }

    // copies all data into buffer
    buffer.clear();
    std::vector<gandalf::byte> file = std::vector<gandalf::byte>(std::istreambuf_iterator<char>(input),
        std::istreambuf_iterator<char>());
    buffer = file;

    return true;
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <boot_rom_file>" << " <rom_file>" << std::endl;
        return EXIT_FAILURE;
    }

    try
    {
        using namespace gandalf;
        Gameboy gb;

        std::vector<byte> boot_rom;
        if (!ReadFile(argv[1], boot_rom))
            return EXIT_FAILURE;

        if (boot_rom.size() != 0x100) {
            std::cerr << "Invalid boot rom size %d, expected 0x100 bytes" << std::endl;
            return EXIT_FAILURE;
        }

        std::array<byte, 0x100> boot_rom_array;
        std::copy(boot_rom.begin(), boot_rom.end(), boot_rom_array.begin());

        std::vector<byte> rom;
        if (!ReadFile(argv[2], rom)) {
            std::cerr << "Could not read ROM file" << std::endl;
            return EXIT_FAILURE;
        }

        if (!gb.Load(rom)) {
            std::cerr << "Could not load ROM" << std::endl;
            return EXIT_FAILURE;
        }

        gb.LoadBootROM(boot_rom_array);

        Cartridge::Header header = *gb.GetCartridge().GetHeader();
        std::cout << "ROM loaded" << std::endl;
        std::cout << "Title: " << header.GetTitle() << std::endl;
        std::cout << "Manufacturer code: " << header.GetManufacturerCode() << std::endl;
        std::cout << "Licensee: " << header.GetLicensee() << std::endl;
        std::cout << "ROM Size: " << header.GetROMSize() << std::endl;
        std::cout << "RAM Size: " << header.GetRAMSize() << std::endl;
        std::cout << "CGB flag: " << header.GetCGBFlag() << std::endl;
        std::cout << "SGB flag: " << header.GetSGBFlag() << std::endl;
        std::cout << "Cartridge type: " << header.GetType() << std::endl;
        std::cout << "Destination: " << header.GetDestination() << std::endl;

        if (!gui::SetupGUI()) {
            std::cerr << "Could not setup GUI" << std::endl;
            return EXIT_FAILURE;
        }

        while (true)
        {
            gb.Run();

            if (gui::PollEvents())
                break;
            gui::RenderGUI();
        }

        gui::DestroyGUI();
    }
    catch (gandalf::Exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }


    //     if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    //         std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
    //         return EXIT_FAILURE;
    //     }

    // #define SCALE 4
    // #define SCREEN_WIDTH 160 * SCALE
    // #define SCREEN_HEIGHT 144 * SCALE

    //     SDL_Window* window;
    //     SDL_Renderer* renderer;
    //     SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &window, &renderer);

    //     IMGUI_CHECKVERSION();
    //     ImGui::CreateContext();
    //     ImGuiIO& io = ImGui::GetIO();
    //     io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;


    //     ImGui::StyleColorsDark();
    //     ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    //     ImGui_ImplSDLRenderer_Init(renderer);

    //     // Load Fonts
    // // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // // - Read 'docs/FONTS.md' for more instructions and details.
    // // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    // //io.Fonts->AddFontDefault();
    // //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    // //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    // //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    // //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
    // //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    // //IM_ASSERT(font != NULL);

    // // Our state
    //     ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    //     // Main loop
    //     bool done = false;
    //     bool show_demo_window = false;
    //     while (!done)
    //     {
    //         // Poll and handle events (inputs, window resize, etc.)
    //         // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
    //         // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
    //         // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
    //         // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
    //         SDL_Event event;
    //         while (SDL_PollEvent(&event))
    //         {
    //             ImGui_ImplSDL2_ProcessEvent(&event);
    //             if (event.type == SDL_QUIT)
    //                 done = true;
    //             if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
    //                 done = true;
    //         }

    //         // Start the Dear ImGui frame
    //         ImGui_ImplSDLRenderer_NewFrame();
    //         ImGui_ImplSDL2_NewFrame();
    //         ImGui::NewFrame();

    //         // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
    //         if (show_demo_window)
    //             ImGui::ShowDemoWindow(&show_demo_window);



    //         gui::RenderGUI();

    //         // Rendering
    //         ImGui::Render();
    //         SDL_SetRenderDrawColor(renderer, (Uint8)(clear_color.x * 255), (Uint8)(clear_color.y * 255), (Uint8)(clear_color.z * 255), (Uint8)(clear_color.w * 255));
    //         SDL_RenderClear(renderer);
    //         ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
    //         SDL_RenderPresent(renderer);
    //     }

    //     // Cleanup
    //     ImGui_ImplSDLRenderer_Shutdown();
    //     ImGui_ImplSDL2_Shutdown();
    //     ImGui::DestroyContext();

    //     SDL_DestroyRenderer(renderer);
    //     SDL_DestroyWindow(window);
    //     SDL_Quit();


        // TODO check window and renderer null

        // SDL_Surface* screen = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32,
        //     0x00FF0000,
        //     0x0000FF00,
        //     0x000000FF,
        //     0xFF000000);

        // SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
        //     SDL_TEXTUREACCESS_STREAMING,
        //     SCREEN_WIDTH, SCREEN_HEIGHT);

        // while (true) {
        //     SDL_Rect rc;
        //     rc.x = rc.y = 0;
        //     rc.w = rc.h = 2048;

        //     for (int y = 0; y < SCREEN_HEIGHT; y++) {
        //         for (int x = 0; x < SCREEN_WIDTH; x++) {
        //             rc.x = x * SCALE;
        //             rc.y = y * SCALE;
        //             rc.w = SCALE;
        //             rc.h = SCALE;

        //             SDL_FillRect(screen, &rc, 0xFFFFFF);
        //         }
        //     }

        //     SDL_UpdateTexture(texture, NULL, screen->pixels, screen->pitch);
        //     SDL_RenderClear(renderer);
        //     SDL_RenderCopy(renderer, texture, NULL, NULL);
        //     SDL_RenderPresent(renderer);

        //     SDL_Event event;
        //     while (SDL_PollEvent(&event)) {
        //         if (event.type == SDL_QUIT) {
        //             return EXIT_SUCCESS;
        //         }
        //     }
        // }
// }

    return EXIT_SUCCESS;
}
