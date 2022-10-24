#include <iostream>
#include <memory>

#include "gui.h"


int main(int argc, char* argv[]) {
    std::unique_ptr<gui::MainWindow> main_window = std::make_unique<gui::MainWindow>();
    if (!main_window->Initialize())
        return EXIT_FAILURE;

    main_window->Run();
    main_window.release();

    return EXIT_SUCCESS;
}
