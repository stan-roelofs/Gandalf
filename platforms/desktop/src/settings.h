#ifndef __GANDALF_SETTINGS_H__
#define __GANDALF_SETTINGS_H__

#include <string>
#include <list>
#include <filesystem>
#include <fstream>

namespace settings {

    struct Settings
    {
        std::list<std::string> recent_roms;
        std::string boot_rom_location;
        bool auto_layout;
    };

    // TODO
    static Settings Read(const std::filesystem::path& path) {
        Settings settings;
        std::ifstream ifs(path);
        std::string line;
        while (std::getline(ifs, line))
            settings.recent_roms.push_back(line);

        settings.auto_layout = true;
        return settings;
    }

    static bool Write(const std::filesystem::path& path, const Settings& settings) {
        std::ofstream ofs(path);
        for (const auto& rom : settings.recent_roms) {
            ofs << rom << std::endl;
        }
        ofs.close();
        return true;
    }
}
#endif