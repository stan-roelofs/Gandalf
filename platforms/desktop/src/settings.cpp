#include "settings.h"

namespace settings
{
    void to_json(json& j, const Settings& s)
    {
        j = json{
            {"auto_layout", s.auto_layout},
            {"recent_roms", s.recent_roms},
            {"boot_rom_location", s.boot_rom_location}
        };
    }

    void from_json(const json& j, Settings& s)
    {
        j.at("auto_layout").get_to(s.auto_layout);
        j.at("recent_roms").get_to(s.recent_roms);
        j.at("boot_rom_location").get_to(s.boot_rom_location);
    }

    bool Read(const std::filesystem::path& path, Settings& settings)
    {
        std::ifstream ifs(path);
        std::string str(std::istreambuf_iterator<char>{ifs}, {});

        json j;
        try {
            j = json::parse(str);
            settings = j;
        }
        catch (json::parse_error& err)
        {
            std::cerr << "Parse error " << err.what() << std::endl;
            return false;
        }

        return true;
    }

    bool Write(const std::filesystem::path& path, const Settings& settings)
    {
        std::ofstream ofs(path);
        json j = settings;
        ofs << j;
        ofs.close();
        return true;
    }
}