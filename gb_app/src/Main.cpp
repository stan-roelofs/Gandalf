#include <cstring>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string>

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
    }

    return EXIT_SUCCESS;
}
