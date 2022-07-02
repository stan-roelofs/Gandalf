#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <gandalf/gameboy.h>
#include <gandalf/cartridge.h>

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <rom_file>" << std::endl;
        return EXIT_FAILURE;
    }

    std::string file_path(argv[1]);

    {
        using namespace gandalf;
        std::vector<byte> rom;

        {
            std::ifstream ifs(argv[1], std::ios::binary | std::ios::ate);
            if (!ifs) {
                char buffer[256];
                strerror_s(buffer, 256, errno);
                throw std::runtime_error(file_path + ": " + buffer);
            }

            auto end = ifs.tellg();
            ifs.seekg(0, std::ios::beg);

            std::size_t size(end - ifs.tellg());
            if (size == 0)
                throw std::runtime_error(file_path + ": empty file");

            rom.resize(size);

            if (!ifs.read((char*)rom.data(), size)) {
                char buffer[256];
                strerror_s(buffer, 256, errno);
                throw std::runtime_error(file_path + ": " + buffer);
            }
        }

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
