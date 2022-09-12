#include <iostream>

#include "gui.h"


int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <boot_rom_file> " << " <rom_file> " << std::endl;
        return EXIT_FAILURE;
    }

    gui::MainWindow* main_window = new gui::MainWindow(argv[1]);
    if (!main_window->Initialize())
        return EXIT_FAILURE;

    main_window->Run();

    delete main_window;
    main_window = nullptr;

    return EXIT_SUCCESS;
}
