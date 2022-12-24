#ifndef __GANDALF_SETTINGS_H__
#define __GANDALF_SETTINGS_H__

#include <string>
#include <list>
#include <filesystem>
#include <fstream>
#include <iostream>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace gui
{
    struct Settings
    {
        Settings();
        std::list<std::string> recent_roms;
        std::string boot_rom_location;
        bool show_debug;
        std::int32_t key_up;
        std::int32_t key_down;
        std::int32_t key_left;
        std::int32_t key_right;
        std::int32_t key_a;
        std::int32_t key_b;
        std::int32_t key_start;
        std::int32_t key_select;
        std::uint16_t window_width;
        std::uint16_t window_height;
    };

    Settings DefaultSettings();

    namespace settings
    {
        bool Read(const std::filesystem::path& path, Settings& settings);
        bool Write(const std::filesystem::path& path, const Settings& settings);
    }
}
#endif