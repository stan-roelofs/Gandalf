#ifndef __GANDALF_SETTINGS_H__
#define __GANDALF_SETTINGS_H__

#include <string>
#include <list>
#include <filesystem>
#include <fstream>
#include <iostream>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace settings
{

    struct Settings
    {
        std::list<std::string> recent_roms;
        std::string boot_rom_location;
        bool auto_layout;
        bool show_debug;
    };

    bool Read(const std::filesystem::path& path, Settings& settings);
    bool Write(const std::filesystem::path& path, const Settings& settings);
}
#endif