#include <cstring>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string>

#include <SDL.h>
#include <SDL_timer.h>

#include <gandalf/gameboy.h>
#include <gandalf/cartridge.h>

static bool ReadFile(std::string filename, std::vector<gandalf::byte>& buffer) {
    std::ifstream input(filename, std::ios::binary);
    if (input.fail()) {
        std::cerr << "Could not open file " << filename << ": " << std::strerror(errno) << std::endl;
        return false;
    }

    // copies all data into buffer
    buffer.clear();
    std::vector<gandalf::byte> file = std::vector<gandalf::byte>(std::istreambuf_iterator<char>(input),
        std::istreambuf_iterator<char>());
    buffer = file;

    return true;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <rom_file>" << std::endl;
        return EXIT_FAILURE;
    }

    std::string file_path(argv[1]);

    {
        using namespace gandalf;

        std::vector<byte> rom;
        if (!ReadFile(argv[1], rom))
            return EXIT_FAILURE;

        Cartridge cartridge;

        if (!cartridge.Load(rom)) {
            std::cerr << "Could not load ROM" << std::endl;
            return EXIT_FAILURE;
        }

        Cartridge::Header header = *cartridge.GetHeader();
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

        if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
            std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
            return EXIT_FAILURE;
        }

#define SCALE 4
#define SCREEN_WIDTH 160 * SCALE
#define SCREEN_HEIGHT 144 * SCALE

        SDL_Window* window;
        SDL_Renderer* renderer;
        SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &window, &renderer);
        // TODO check window and renderer null

        SDL_Surface* screen = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32,
            0x00FF0000,
            0x0000FF00,
            0x000000FF,
            0xFF000000);

        SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
            SDL_TEXTUREACCESS_STREAMING,
            SCREEN_WIDTH, SCREEN_HEIGHT);

        while (true) {
            SDL_Rect rc;
            rc.x = rc.y = 0;
            rc.w = rc.h = 2048;

            for (int y = 0; y < SCREEN_HEIGHT; y++) {
                for (int x = 0; x < SCREEN_WIDTH; x++) {
                    rc.x = x * SCALE;
                    rc.y = y * SCALE;
                    rc.w = SCALE;
                    rc.h = SCALE;

                    SDL_FillRect(screen, &rc, 0xFFFFFF);
                }
            }

            SDL_UpdateTexture(texture, NULL, screen->pixels, screen->pitch);
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, texture, NULL, NULL);
            SDL_RenderPresent(renderer);

            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    return EXIT_SUCCESS;
                }
            }
        }
    }

    return EXIT_SUCCESS;
}
