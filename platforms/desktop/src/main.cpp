#include <cstring>
#include <chrono>
#include <iostream>
#include <fstream>
#include <optional>
#include <stdlib.h>
#include <string>
#include <thread>

#include <SDL.h>

#include <gandalf/gameboy.h>
#include <gandalf/cartridge.h>

#include "gui.h"
#include "audio_handler.h"

namespace
{
    constexpr std::uint64_t expected_frame_time_nanoseconds = (1.0 / 60.0) * 1e9;
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

static void RunGameboy(gui::Context& context)
{
    while (true)
    {
        while (*context.run)
        {
            context.gameboy->Run();
            if (context.breakpoint && *context.breakpoint == context.gameboy->GetCPU().GetRegisters().program_counter)
                *context.run = false;
        }

        if (*context.step)
        {
            context.gameboy->Run();
            *context.step = false;
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <boot_rom_file>" << std::endl;
        return EXIT_FAILURE;
    }

    using namespace gandalf;
    std::unique_ptr<Gameboy> gb = std::make_unique<Gameboy>();
    auto back_buffer = new LCD::VideoBuffer();
    auto front_buffer = new LCD::VideoBuffer();

    std::vector<byte> boot_rom;
    if (!ReadFile(argv[1], boot_rom))
        return EXIT_FAILURE;

    if (boot_rom.size() != 0x100) {
        std::cerr << "Invalid boot rom size %d, expected 0x100 bytes" << std::endl;
        return EXIT_FAILURE;
    }

    std::array<byte, 0x100> boot_rom_array;
    std::copy(boot_rom.begin(), boot_rom.end(), boot_rom_array.begin());

    gb->LoadBootROM(boot_rom_array);

    if (!gui::SetupGUI()) {
        std::cerr << "Could not setup GUI" << std::endl;
        return EXIT_FAILURE;
    }

    std::shared_ptr<SDLAudioHandler> handler = std::make_shared<SDLAudioHandler>();
    gb->GetAPU().SetOutputHandler(handler);

    std::optional<word> breakpoint;
    bool run = false;
    bool step = false;
    bool show_debug_window = true;
    bool sleep = false;
    gui::Context context;
    context.run = &run;
    context.step = &step;
    context.gameboy = gb.get();
    context.show_debug_window = &show_debug_window;
    context.breakpoint = &breakpoint;
    context.video_buffer = &front_buffer;
    context.sleep = &sleep;
    context.limit_frames = &handler->limit_frames_;

    struct : VBlankListener
    {
        void OnVBlank() {
            *sleep = true;
            auto& buffer = gb->GetLCD().GetVideoBuffer();
            std::copy(buffer.begin(), buffer.end(), back_buffer->begin());
            auto temp = back_buffer;
            back_buffer = front_buffer;
            front_buffer = temp;

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

        gandalf::Gameboy* gb;
        gandalf::LCD::VideoBuffer* back_buffer;
        gandalf::LCD::VideoBuffer* front_buffer;
        bool* sleep;

    } fps_counter;
    fps_counter.gb = gb.get();
    fps_counter.back_buffer = back_buffer;
    fps_counter.front_buffer = front_buffer;
    fps_counter.sleep = &sleep;

    gb->GetPPU().SetVBlankListener(&fps_counter);

    std::thread gb_thread(RunGameboy, std::ref(context));

    using namespace std::chrono;

    while (true)
    {
        std::chrono::high_resolution_clock::time_point t1 = high_resolution_clock::now();;

        if (gui::PollEvents(context))
            break;

        RenderGUI(context);

        std::chrono::high_resolution_clock::time_point t2 = high_resolution_clock::now();;
        duration<double, std::nano> time_span = t2 - t1;

        const auto duration = std::chrono::nanoseconds((long)((1000000000 / 60) - time_span.count()));
        std::this_thread::sleep_for(duration);
    }

    delete front_buffer;
    delete back_buffer;
    gui::DestroyGUI();

    return EXIT_SUCCESS;
}
