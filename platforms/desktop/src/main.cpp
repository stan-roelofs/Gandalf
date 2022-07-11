#include <cstring>
#include <chrono>
#include <iostream>
#include <fstream>
#include <optional>
#include <stdlib.h>
#include <string>

#include <SDL.h>

#include <gandalf/gameboy.h>
#include <gandalf/cartridge.h>

#include "gui.h"

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
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <boot_rom_file>" << std::endl;
        return EXIT_FAILURE;
    }

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

    gb.LoadBootROM(boot_rom_array);

    if (!gui::SetupGUI()) {
        std::cerr << "Could not setup GUI" << std::endl;
        return EXIT_FAILURE;
    }

    std::optional<word> breakpoint;
    bool run = false;
    bool step = false;
    bool show_debug_window = true;
    gui::Context context;
    context.run = &run;
    context.step = &step;
    context.gameboy = &gb;
    context.show_debug_window = &show_debug_window;
    context.breakpoint = &breakpoint;

    struct : VBlankListener
    {
        void OnVBlank() {
            has_frame = true;
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
        };

        bool has_frame;
    } fps_counter;
    fps_counter.has_frame = false;

    gb.GetPPU().SetVBlankListener(&fps_counter);

    while (true)
    {
        if (gui::PollEvents(context))
            break;
        gui::RenderGUI(context);

        if (step) {
            gb.Run();
            step = false;
        }

        if (run) {
            while (!fps_counter.has_frame) {
                gb.Run();

                if (breakpoint && *breakpoint == gb.GetCPU().GetRegisters().program_counter) {
                    run = false;
                    break;
                }
            }
            fps_counter.has_frame = false;
        }

    }

    gui::DestroyGUI();


    // TODO check window and renderer null



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
