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

    bool run = false;
    bool step = false;
    gui::Context context;
    context.run = &run;
    context.step = &step;
    context.gameboy = &gb;
    while (true)
    {
        try {
            if (gui::PollEvents())
                break;
            gui::RenderGUI(context);

            if (step) {
                gb.Run();
                step = false;
            }

            if (run) {
                gb.Run();
            }
        }
        catch (gandalf::Exception& e) {
            std::cerr << e.what() << std::endl;
            run = false;
        }
    }

    gui::DestroyGUI();


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
