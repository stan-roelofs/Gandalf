#include "settings.h"

namespace settings
{
    void to_json(json& j, const Settings& s)
    {
        j = json{
            {"auto_layout", s.auto_layout},
            {"recent_roms", s.recent_roms},
            {"boot_rom_location", s.boot_rom_location},
            {"show_debug", s.show_debug}
        };
    }

    void from_json(const json& j, Settings& s)
    {
        s.auto_layout = j.value("auto_layout", true);
        s.recent_roms = j.value("recent_roms", std::list<std::string>());
        s.boot_rom_location = j.value("boot_rom_location", "");
        s.show_debug = j.value("show_debug", false);
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