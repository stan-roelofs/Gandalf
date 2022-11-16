#include "settings.h"

#include "SDL_keycode.h"

namespace settings
{
    void to_json(json& j, const Settings& s)
    {
        j = json{
            {"recent_roms", s.recent_roms},
            {"boot_rom_location", s.boot_rom_location},
            {"show_debug", s.show_debug},
            {"key_up", s.key_up},
            {"key_down", s.key_down},
            {"key_left", s.key_left},
            {"key_right", s.key_right},
            {"key_a", s.key_a},
            {"key_b", s.key_b},
            {"key_start", s.key_start},
            {"key_select", s.key_select}
        };
    }

    void from_json(const json& j, Settings& s)
    {
        s.recent_roms = j.value("recent_roms", std::list<std::string>());
        s.boot_rom_location = j.value("boot_rom_location", "");
        s.show_debug = j.value("show_debug", false);
        s.key_up = j.value("key_up", SDLK_UP);
        s.key_down = j.value("key_down", SDLK_DOWN);
        s.key_left = j.value("key_left", SDLK_LEFT);
        s.key_right = j.value("key_right", SDLK_RIGHT);
        s.key_a = j.value("key_a", SDLK_z);
        s.key_b = j.value("key_b", SDLK_x);
        s.key_start = j.value("key_start", SDLK_RETURN);
        s.key_select = j.value("key_select", SDLK_TAB);
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